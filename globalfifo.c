#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/poll.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#include <linux/sched/signal.h>
#endif

#define GLOBALFIFO_SIZE		0x1000
#define GLOBALFIFO_MAJOR	231

#define GLOBALFIFO_PRINT(s) \
	printk("%s:%d %s\n", __func__, __LINE__, s)

struct globalfifo_dev {
	struct cdev cdev;
	unsigned int current_len;
	unsigned char mem[GLOBALFIFO_SIZE];
	struct mutex mutex;
	wait_queue_head_t rq;
	wait_queue_head_t wq;
};

static struct globalfifo_dev *globalfifo_devp;
static int globalfifo_major = GLOBALFIFO_MAJOR;

static int globalfifo_open(struct inode *inode, struct file *filp)
{
	filp->private_data = globalfifo_devp;
	return 0;
}

static int globalfifo_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t globalfifo_read(struct file *filp, char __user *buf,
			       size_t count, loff_t *ppos)
{
	int ret = 0;
	struct globalfifo_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->rq, &wait);

	while (dev->current_len == 0) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}

		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);

		schedule();
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			goto out2;
		}

		mutex_lock(&dev->mutex);
	}

	if (count > dev->current_len)
		count = dev->current_len;

	if (copy_to_user(buf, dev->mem, count)) {
		ret = -EFAULT;
		goto out;
	}

	memcpy(dev->mem, dev->mem + count, dev->current_len - count);
	dev->current_len -= count;
	printk(KERN_INFO "%s:%d read %ld byte(s), current_len is %d\n",
	       __func__, __LINE__, count, dev->current_len);

	wake_up_interruptible(&dev->wq);

	ret = count;

out:
	mutex_unlock(&dev->mutex);

out2:
	remove_wait_queue(&dev->rq, &wait);
	set_current_state(TASK_RUNNING);

	return ret;
}

static ssize_t globalfifo_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	int ret = 0;
	struct globalfifo_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->wq, &wait);

	while (dev->current_len == GLOBALFIFO_SIZE) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}

		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);

		schedule();
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			goto out2;
		}

		mutex_lock(&dev->mutex);
	}

	if (count > GLOBALFIFO_SIZE - dev->current_len)
		count = GLOBALFIFO_SIZE - dev->current_len;

	if (copy_from_user(dev->mem + dev->current_len, buf, count)) {
		ret = -EFAULT;
		goto out;
	}

	dev->current_len += count;
	printk(KERN_INFO "%s:%d write %ld byte(s), current_len is %d\n",
	       __func__, __LINE__, count, dev->current_len);

	wake_up_interruptible(&dev->rq);

	ret = count;

out:
	mutex_unlock(&dev->mutex);

out2:
	remove_wait_queue(&dev->wq, &wait);
	set_current_state(TASK_RUNNING);

	return ret;
}

static unsigned int globalfifo_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct globalfifo_dev *dev = filp->private_data;

	mutex_lock(&dev->mutex);
	poll_wait(filp, &dev->rq, wait);
	poll_wait(filp, &dev->wq, wait);

	if (dev->current_len != 0)
		mask |= POLLIN | POLLRDNORM;

	if (dev->current_len != GLOBALFIFO_SIZE)
		mask |= POLLOUT | POLLWRNORM;

	mutex_unlock(&dev->mutex);

	return mask;
}

static const struct file_operations globalfifo_fops = {
	.owner          = THIS_MODULE,
	.open           = globalfifo_open,
	.release        = globalfifo_release,
	.read           = globalfifo_read,
	.write          = globalfifo_write,
	.poll           = globalfifo_poll,
};

static void globalfifo_setup_cdev(void)
{
	int err;
	struct cdev *p_cdev = &globalfifo_devp->cdev;

	cdev_init(p_cdev, &globalfifo_fops);
	p_cdev->owner = THIS_MODULE;
	err = cdev_add(p_cdev, MKDEV(globalfifo_major, 0), 1);
	if (err)
		GLOBALFIFO_PRINT("exit");
}

static int __init globalfifo_init(void)
{
	int ret;
	dev_t devno = MKDEV(globalfifo_major, 0);

	GLOBALFIFO_PRINT("init");

	if (devno) {
		ret = register_chrdev_region(devno, 1, "globalfifo");
	} else {
		ret = alloc_chrdev_region(&devno, 0, 1, "globalfifo");
		globalfifo_major = MAJOR(devno);
	}

	if (ret < 0)
		goto fail_register_chrdev;

	globalfifo_devp = kzalloc(sizeof(struct globalfifo_dev), GFP_KERNEL);
	if (!globalfifo_devp) {
		ret = -ENOMEM;
		goto fail_kzalloc;
	}

	globalfifo_setup_cdev();
	mutex_init(&globalfifo_devp->mutex);
	init_waitqueue_head(&globalfifo_devp->rq);
	init_waitqueue_head(&globalfifo_devp->wq);

	return 0;

fail_kzalloc:
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), 1);

fail_register_chrdev:
	return ret;
}

static void __exit globalfifo_exit(void)
{
	GLOBALFIFO_PRINT("exit");
	cdev_del(&globalfifo_devp->cdev);
	kfree(globalfifo_devp);
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), 1);
}

module_init(globalfifo_init);
module_exit(globalfifo_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Linggang Zeng");
