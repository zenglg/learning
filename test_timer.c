#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list test_timer;

static void test_timer_function(struct timer_list  *timer)
{
	printk("test_timer_function() is called.\n");
	/* Modify the next time that test_timer_function() be called.*/
	mod_timer(&test_timer, jiffies + 2 * HZ);
}

static int timer_init(void)
{
	test_timer.expires  = jiffies + 2 * HZ;
	timer_setup(&test_timer, test_timer_function, 0);

	add_timer(&test_timer);

	printk(KERN_INFO "timer_init() is called.\n");

	return 0;
}

static void timer_exit(void)
{
	printk(KERN_INFO "timer_exit() is called.\n");
	del_timer(&test_timer);
}

module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");
