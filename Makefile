KVERS = $(shell uname -r)
# Kernel modules
obj-m += hello.o
# obj-m += mtd_read_test.o
obj-m += globalmem.o
obj-m += kprobe_exam.o
obj-m += globalfifo.o
obj-m += kobject_create_and_add.o
obj-m += test_timer.o
obj-m += netlink_module.o
obj-m += tracepoint-sample.o
# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g O0

build: kernel_modules
kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
	gcc -o sudoku sudoku.c
	gcc -o netlink_test netlink_test.c
	gcc -o cal_24 cal_24.c
	nasm tiny_kernel.S -o tiny_kernel.bin
	gcc -o tiny_qemu tiny_qemu.c
clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
	rm -rf sudoku netlink_test cal_24 tiny_kernel.bin tiny_qemu
