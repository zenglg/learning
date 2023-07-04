#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/wait.h>

static struct task_struct *my_thread;
// 声明等待队列头
DECLARE_WAIT_QUEUE_HEAD(my_wait_queue);
static int condition = false;

static int my_thread_func(void *data)
{
	pr_info("My thread started\n");

#if 1
	pr_info("Putting the current thread to sleep\n");
	// 当前线程进入睡眠状态，等待唤醒
	wait_event_interruptible(my_wait_queue,
				 kthread_should_stop() || condition);
#else
	// 模拟一些操作
	msleep(5000);

	pr_info("Waking up the waiting thread\n");
	condition = true;
	// 唤醒等待队列中的线程
	wake_up_interruptible(&my_wait_queue);
	pr_info("Waked up the waiting thread\n");
#endif

	my_thread = NULL;

	pr_info("my thread is Done\n");

	return 0;
}

static int __init my_module_init(void)
{
	pr_info("Initializing my module\n");

	// 创建等待队列线程
	my_thread = kthread_run(my_thread_func, NULL, "my_thread");
	if (IS_ERR(my_thread)) {
		pr_err("Failed to create my thread\n");
		return PTR_ERR(my_thread);
	}

#if 1
	msleep(5000);

	pr_info("Waking up the waiting thread\n");
	condition = true;
	// 唤醒等待队列中的线程
	wake_up_interruptible(&my_wait_queue);
	pr_info("Waked up the waiting thread\n");
#else
	pr_info("Putting the current thread to sleep\n");
	// 当前线程进入睡眠状态，等待唤醒
	wait_event_interruptible(my_wait_queue, condition);
#endif

	pr_info("My module initialized\n");
	return 0;
}

static void __exit my_module_exit(void)
{
	pr_info("Exiting my module\n");

	if (my_thread) {
		pr_info("Stopping the waiting thread\n");
		// 停止等待队列线程
		kthread_stop(my_thread);
	}

	pr_info("My module exited\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
MODULE_LICENSE("GPL");
