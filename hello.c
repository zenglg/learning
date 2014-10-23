#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static char __initdata *whom = "world";
static int __initdata howmany = 1;
module_param(whom, charp, S_IRUGO);
module_param(howmany, int, S_IRUGO);

static int hello_init(void)
{
	while (howmany-- > 0)
		printk(KERN_ALERT "Hello, %s\n", whom);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
