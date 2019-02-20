KVERS = $(shell uname -r)
# Kernel modules
obj-m += hello.o
obj-m += mtd_read_test.o
obj-m += globalmem.o
obj-m += kprobe_exam.o
obj-m += globalfifo.o
obj-m += kobject_create_and_add.o
# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g O0

build: kernel_modules
kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
	gcc -o sudoku sudoku.c
clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
	rm -rf sudoku
