#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");

#define print(fmt, arg...)	\
	pr_info("%s:%d pid-%d, state-%ld" fmt, __func__, __LINE__, \
		current->pid, current->state, ##arg)

#ifndef	SLEEP_MILLI_SEC

# define SLEEP_MILLI_SEC(nMilliSec) \
do { \
	long timeout = (nMilliSec) * HZ / 1000; \
	while (timeout > 0) { \
		timeout = schedule_timeout(timeout); \
	} \
} while (0)

#endif

static struct task_struct *kthread;

static int tmp_printk(void *data)
{
	int times = 0;

	while (!kthread_should_stop() && times < 10) {
		SLEEP_MILLI_SEC(1000);
		print("%2d: %s\n", times++, (char *)data);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}

	kthread = NULL;

	return 0;
}

static int __init init_kthread(void)
{
	kthread = kthread_create(tmp_printk, "hello world", "mythread");
	wake_up_process(kthread);

	return 0;
}

static void __exit exit_kthread(void)
{
	if (kthread)
		kthread_stop(kthread);
}

module_init(init_kthread);
module_exit(exit_kthread);
MODULE_AUTHOR("Zeng");
