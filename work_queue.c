#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#define NUM_WORK	10

static struct workqueue_struct *queue = NULL;
static struct work_struct *work = NULL;

static void work_queue_handler(struct work_struct *w)
{
	printk(KERN_ALERT "work handler function.\n");
	udelay(100);
}

static int __init work_queue_init(void)
{
	int ret = 0;
	int i;

	/* 创建一个单线程的工作队列 */
	queue = create_singlethread_workqueue("work_queue");
	if (!queue) {
		ret = -ENOMEM;
		goto out;
	}

	work = kmalloc(sizeof(struct work_struct) * NUM_WORK, GFP_KERNEL);
	if (!work) {
		ret = -ENOMEM;
		goto destroy_work_queue;
	}

	for (i = 0; i < NUM_WORK; i++) {
		INIT_WORK(&work[i], work_queue_handler);
		queue_work(queue, &work[i]);
		printk(KERN_INFO "Work item %d queued.\n", i);
	}

	return ret;

destroy_work_queue:
	destroy_workqueue(queue);
out:
	return ret;
}

static void __exit work_queue_exit(void)
{
	if (queue)
		flush_workqueue(queue);

	if (work)
		kfree(work);

	if (queue)
		destroy_workqueue(queue);

	printk(KERN_INFO "Workqueue module exited.\n");
}

module_init(work_queue_init);
module_exit(work_queue_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Linggang Zeng");
MODULE_DESCRIPTION("A simple workqueue example");
