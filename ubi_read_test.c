#include <linux/init.h>
#include <linux/module.h>
#include <linux/mtd/ubi.h>
#include <linux/slab.h>
#include <linux/version.h>

#include "ubi.h"

static int ubi_num = 0;
module_param(ubi_num, int, S_IRUGO);
MODULE_PARM_DESC(ubi_num, "UBI device number to use");

static int vol_id = 0;
module_param(vol_id, int, S_IRUGO);
MODULE_PARM_DESC(vol_id, "UBI device vol id to use");

static int mode = UBI_READONLY;
module_param(mode, int, S_IRUGO);
MODULE_PARM_DESC(mode, "UBI device open mode");

static int __init ubi_read_test_init(void)
{
	int err;
	int len;
	int lnum = 0;
	int offset = 0;
	char *buf = NULL;
	char *write_buf = NULL;
	struct ubi_volume_desc *desc;
	ktime_t begin, after, time_consuming;

	desc = ubi_open_volume(ubi_num, vol_id, mode);
	if (IS_ERR(desc)) {
		err = PTR_ERR(desc);
		goto out;
	}

	pr_info("reserved_pebs: %d\n", desc->vol->reserved_pebs);
	pr_info("vol_type: %d\n", desc->vol->vol_type);
	pr_info("usable_leb_size: %d\n", desc->vol->usable_leb_size);
	pr_info("used_ebs: %d\n", desc->vol->used_ebs);
	pr_info("used_bytes: %lld\n", desc->vol->used_bytes);

	len = desc->vol->usable_leb_size;
	err = -ENOMEM;
	buf = kmalloc(len, GFP_KERNEL);
	if (!buf)
		goto out;

	write_buf = kmalloc(len, GFP_KERNEL);
	if (!write_buf)
		goto out;

	memset(write_buf, 'a', len);

	for (lnum = 0; lnum < desc->vol->used_ebs; lnum++) {
		err = ubi_leb_write(desc, lnum, write_buf, offset, len);
		if (err < 0) {
			pr_info("ubi_leb_write error\n");
			goto out;
		}
	}

	begin = ktime_get();

	for (lnum = 0; lnum < desc->vol->used_ebs; lnum++) {
		err = ubi_read(desc, lnum, buf, offset, len);
		if (err < 0) {
			pr_info("ubi_read error\n");
			goto out;
		}
	}

	after = ktime_get();

	pr_info("insmod ubi_read_test success.\n");

	time_consuming = ktime_sub(after, begin);

	pr_info("Time consuming: %llums -- %lluus\n",
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
		time_consuming / 1000000, time_consuming / 1000);
#else
		time_consuming.tv64 / 1000000, time_consuming.tv64 / 1000);
#endif

out:
	kfree(write_buf);
	kfree(buf);
	if (!IS_ERR(desc))
		ubi_close_volume(desc);
	return err;
}

static void __exit ubi_read_test_exit(void)
{
	pr_info("rmmod ubi_read_test success.\n");
}

module_init(ubi_read_test_init);
module_exit(ubi_read_test_exit);

MODULE_DESCRIPTION("UBI Read test module");
MODULE_AUTHOR("Linggang Zeng");
MODULE_LICENSE("GPL");
