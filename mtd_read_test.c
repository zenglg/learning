#include <linux/init.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
# define MTD_READ_TEST_HAVE_WRITEBUFSIZE
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
# define FNAND2_HAS_GENERIC_MTD_INTERFACE
#endif

#ifndef FNAND2_HAS_GENERIC_MTD_INTERFACE
static inline int mtd_read(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, u_char *buf)
{
	return mtd->read(mtd, from, len, retlen, buf);
}
static inline int mtd_write(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, const u_char *buf)
{
	return mtd->write(mtd, from, len, retlen, buf);
}
static inline int mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	return mtd->erase(mtd, instr);
}
static inline int mtd_is_bitflip(int err)
{
	return err == -EUCLEAN;
}
static inline int mtd_block_isbad(struct mtd_info *mtd, loff_t ofs)
{
	if (!mtd->block_isbad)
		return 0;
	return mtd->block_isbad(mtd, ofs);
}
#endif

static int dev = -EINVAL;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static unsigned long total_size = 4UL*1024*1024*1024;
module_param(total_size, ulong, 0);
MODULE_PARM_DESC(totoal_size, "Total device size for testing MTD device");

static unsigned long offs = 4UL*1024;
module_param(offs, ulong, 0);

static unsigned long len = 4UL*1024;
module_param(len, ulong, 0);

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

static inline int mtdtest_write(struct mtd_info *mtd, loff_t addr, size_t size,
				const void *buf)
{
	size_t written;
	int err;

	err = mtd_write(mtd, addr, size, &written, buf);
	if (!err && written != size)
		err = -EIO;
	if (err)
		pr_err("error: write failed at %#llx\n", addr);

	return err;
}

static int __init mtd_read_test_init(void)
{
	int i;
	int blocks;
	int err = 0;
	int count = 0;
	char *buf = NULL;
	static unsigned long long  time = 0;
	struct erase_info ei;

	ktime_t start, end;

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

	buf = kmalloc(len, GFP_KERNEL);
	if (!buf) {
		pr_info("buf - erasesize: %d\n", mtd->erasesize);
		err = -ENOMEM;
		goto out;
	}

	blocks = (total_size < mtd->size ? total_size : mtd->size) / len;

	pr_info("blocks: %d\n", blocks);

	for (i = 0; i < blocks; i++) {
		if (mtd_block_isbad(mtd, i * len))
			continue;

		start = ktime_get();

		err = mtdtest_read(mtd, i * len, len, buf);
		if (err < 0) {
			pr_info("i: %d, len: %lu, i * len: %lu\n", i, len, i * len);
			pr_info("mtd_read error, err: %d\n", err);
			goto out;
		}

		end = ktime_get();

		time += (end.tv64 - start.tv64);
		count++;
	}

	pr_info("count: %d\n", count);
	pr_info("size: %lu\n", count * len);
	pr_info("Time : %llu %lluns\n", time, time / count);

	memset(&ei, 0, sizeof(struct erase_info));
	ei.mtd  = mtd;
	ei.addr = 0;
	ei.len  = mtd->erasesize;

	mtd_erase(mtd, &ei);
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
