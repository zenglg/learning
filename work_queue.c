#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#define NUM_WORK	10

struct data {
	int			data;
	struct list_head	list;
};

static struct test {
	struct workqueue_struct	*queue;
	struct work_struct	work;
	struct data		head;
	spinlock_t		lock;
} *t;

static void work_queue_handler(struct work_struct *w)
{
	struct test *tmp = container_of(w, struct test, work);
	struct data *h = &tmp->head;
	struct list_head *p, *n;
	struct data *d;

	list_for_each_safe(p, n, &h->list) {
		d = list_entry(p, struct data, list);
		spin_lock(&t->lock);
		pr_info("d->data:%d\n", d->data);
		list_del(p);
		spin_unlock(&t->lock);
		if (d)
			kfree(d);
	}
}

static int __init work_queue_init(void)
{
	int ret = 0;
	int i;
	struct list_head *p, *n;
	struct data *d;

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

	INIT_WORK(&t->work, work_queue_handler);

	INIT_LIST_HEAD(&t->head.list);

	spin_lock_init(&t->lock);

	t->head.data = 13;

	for (i = 0; i < NUM_WORK; i++) {
		struct data *data;
		data = kmalloc(sizeof(struct data), GFP_KERNEL);
		if (!data) {
			ret = -ENOMEM;
			goto free_data;
		}

		spin_lock(&t->lock);
		data->data = i;
		list_add_tail(&(data->list), &(t->head.list));
		spin_unlock(&t->lock);
	}

	queue_work(t->queue, &t->work);

	for (i = 0; i < NUM_WORK; i++) {
		struct data *data;
		data = kmalloc(sizeof(struct data), GFP_KERNEL);
		if (!data) {
			ret = -ENOMEM;
			goto free_data;
		}

		spin_lock(&t->lock);
		data->data = 10 + i;
		list_add_tail(&(data->list), &(t->head.list));
		spin_unlock(&t->lock);
	}

	queue_work(t->queue, &t->work);

	return ret;

free_data:
	list_for_each_safe(p, n, &(t->head.list)) {
		d = list_entry(p, struct data, list);
		list_del(p);
		if (d)
			kfree(d);
	}

	destroy_workqueue(t->queue);

free_t:
	if (t)
		kfree(t);

out:
	return ret;
}

static void __exit work_queue_exit(void)
{
	destroy_workqueue(t->queue);

	if (t)
		kfree(t);
}

module_init(work_queue_init);
module_exit(work_queue_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zeng Linggang");
