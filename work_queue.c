#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#define NUM_WORK	10

static struct test {
	struct workqueue_struct	*queue;
	struct work_struct	*work;
} *t;

static void work_queue_handler(struct work_struct *w)
{
	printk(KERN_ALERT "work handler function.\n");
	udelay(100);
}

static int __init work_queue_init(void)
{
	int ret = 0;
	int i;

	t = kmalloc(sizeof(struct test), GFP_KERNEL);
	if (!t) {
		ret = -ENOMEM;
		goto out;
	}

	t->queue = create_singlethread_workqueue("work_queue");
	if (!t->queue) {
		ret = -ENOMEM;
		goto free_t;
	}

	t->work = kmalloc(sizeof(struct work_struct) * NUM_WORK, GFP_KERNEL);
	if (!t->work) {
		ret = -ENOMEM;
		goto destroy_queue;
	}

	for (i = 0; i < NUM_WORK; i++) {
		INIT_WORK(&t->work[i], work_queue_handler);
		queue_work(t->queue, &t->work[i]);
	}

	return ret;

destroy_queue:
	destroy_workqueue(t->queue);

free_t:
	if (t)
		kfree(t);

out:
	return ret;
}

static void __exit work_queue_exit(void)
{
	if (t->work)
		kfree(t->work);

	destroy_workqueue(t->queue);

	if (t)
		kfree(t);
}

module_init(work_queue_init);
module_exit(work_queue_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zeng Linggang");
