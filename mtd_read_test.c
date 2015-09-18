#include <linux/init.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/version.h>
#include <linux/slab.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
# define MTD_READ_TEST_HAVE_WRITEBUFSIZE
#endif

static int dev = -EINVAL;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static struct mtd_info *mtd;

static inline int mtdtest_read(struct mtd_info *mtd, loff_t addr, size_t size,
			       void *buf)
{
	size_t read;
	int err;

	err = mtd_read(mtd, addr, size, &read, buf);
	/* Ignore corrected ECC errors */
	if (mtd_is_bitflip(err))
		err = 0;
	if (!err && read != size)
		err = -EIO;
	if (err)
		pr_err("error: read failed at %#llx\n", addr);

	return err;
}

static int __init mtd_read_test_init(void)
{
	int i;
	int blocks;
	int err;
	char *buf;
	ktime_t begin, after, time_consuming;

	if (dev < 0) {
		pr_info("Please specify a valid mtd-device via module parameter\n");
		pr_info("For example: insmod <ko file> <dev=[0]>\n");
		return -EINVAL;
        }

	mtd = get_mtd_device(NULL, dev);
	if (IS_ERR(mtd)) {
		err = PTR_ERR(mtd);
		pr_err("error: Cannot get MTD device\n");
		return err;
	}

	err = -ENOMEM;
	buf = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (!buf)
		goto out;

	pr_info("type: %d\n", mtd->type);
	pr_info("flags: %u\n", mtd->flags);
	pr_info("size: %llu\n", mtd->size);
	pr_info("erasesize: %u\n", mtd->erasesize);
	pr_info("writesize: %u\n", mtd->writesize);
#ifdef MTD_READ_TEST_HAVE_WRITEBUFSIZE
	pr_info("writebufsize: %u\n", mtd->writebufsize);
#endif
	pr_info("oobsize: %u\n", mtd->oobsize);
	pr_info("oobavail: %u\n", mtd->oobavail);
	pr_info("name: %s\n", mtd->name);

	blocks = mtd->size / mtd->erasesize;

	begin = ktime_get();

	for (i = 0; i < blocks; i++) {
		err = mtdtest_read(mtd, 0, mtd->erasesize, buf);
		if (err < 0) {
			pr_info("mtd_read error\n");
			goto out;
		}
	}

	after = ktime_get();
	
	time_consuming = ktime_sub(after, begin);

	pr_info("Time consuming: %llums -- %lluus\n",
		time_consuming.tv64 / 1000000, time_consuming.tv64 / 1000);

out:
	kfree(buf);
	put_mtd_device(mtd);
	return err;
}

static void __exit mtd_read_test_exit(void)
{
	printk(KERN_INFO "mtd_read_test_exit\n");
}

module_init(mtd_read_test_init);
module_exit(mtd_read_test_exit);

MODULE_DESCRIPTION("MTD Read test module");
MODULE_AUTHOR("Zeng Linggang");
MODULE_LICENSE("GPL");
