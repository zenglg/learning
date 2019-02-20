#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
MODULE_LICENSE("Dual BSD/GPL");

static struct kobject *kobj = NULL;

static int __init kobject_create_and_add_init(void)
{
	printk(KERN_INFO "Init kobject_create_and_add!\n");

	kobj = kobject_create_and_add("kobject_create_and_add", NULL);
	if (!kobj) {
		printk(KERN_ALERT "error kobject_create_and_add!\n");
		return ENOMEM;
	}

	printk(KERN_INFO "kobject_create_and_add is done!\n");

	return 0;
}

static void __exit kobject_create_and_add_exit(void)
{
	printk(KERN_ALERT "Goodbye, kobject_create_and_add!\n");

	if (kobj)
		kobject_del(kobj);
}

module_init(kobject_create_and_add_init);
module_exit(kobject_create_and_add_exit);

MODULE_AUTHOR("Linggang Zeng");
