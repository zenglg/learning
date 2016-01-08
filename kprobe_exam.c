/* kprobe_exam.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/time.h>

static struct kprobe kp;

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	printk("current task on CPU#%d: %s (before scheduling)\n",
	       smp_processor_id(), current->comm);
	printk("regs->di: %lx\n", regs->di);
	printk("regs->si: %lx\n", regs->si);
	printk("regs->dx: %lx\n", regs->dx);
	printk("regs->cx: %lx\n", regs->cx);
	printk("regs->ax: %lx\n", regs->ax);
	printk("flags: %x, PF_EXITING: %x\n",
	       ((struct task_struct *)(regs->di))->flags, PF_EXITING);
	printk("pid: %d\n", ((struct task_struct *)(regs->di))->pid);
	printk("io_context: %p\n",
	       ((struct task_struct *)(regs->di))->io_context);

	return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs,
			 unsigned long flags)
{
	printk("current task on CPU#%d: %s (after scheduling)\n",
	       smp_processor_id(), current->comm);
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	printk(KERN_INFO "A fault happened during probing.\n");

	return 0;
}

static __init int init_kprobe_exam(void)
{
	int ret;

	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;
	kp.addr = (kprobe_opcode_t *) kallsyms_lookup_name("cgroup_exit");
	kp.symbol_name = "cgroup_exit";

	if ((ret = register_kprobe(&kp)) < 0) {
		printk("register_kprobe failed, returned %d\n", ret);
		return -1;
	}

	printk(KERN_INFO "kprobe registered\n");

	return 0;
}

static __exit void cleanup_kprobe_exam(void)
{
	unregister_kprobe(&kp);

	printk(KERN_INFO "kprobe unregistered\n");
}

module_init(init_kprobe_exam);
module_exit(cleanup_kprobe_exam);

MODULE_LICENSE("GPL");
