/*
 * From: https://www.cnblogs.com/LoyenWang/p/13796537.html
 *
 * 1. Build tiny_kernel
 * # nasm tiny_kernel.S -o tiny_kernel.bin
 *
 * 2. Build tiny_qemu
 * # gcc -o tiny_qemu tiny_qemu.c
 *
 * 3. Run
 * # ./tiny_qemu
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <sys/mman.h>

#define KVM_DEV			"/dev/kvm"
#define TINY_KERNEL_FILE	"./tiny_kernel.bin"
#define PAGE_SIZE		0x1000

int main(void)
{
	int kvm_fd;
	int vm_fd;
	int vcpu_fd;
	int tiny_kernel_fd;
	int ret;
	int mmap_size;
	
	struct kvm_sregs sregs;
	struct kvm_regs regs;
	struct kvm_userspace_memory_region mem;
	struct kvm_run *kvm_run;
	void *userspace_addr;

	/* open kvm device */
	kvm_fd = open(KVM_DEV, O_RDWR);
	assert(kvm_fd > 0);

	/* create VM */
	vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0);
	assert(vm_fd >= 0);

	/* create VCPU */
	vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0);
	assert(vcpu_fd >= 0);

	/* open tiny_kernel binary file */
	tiny_kernel_fd = open(TINY_KERNEL_FILE, O_RDONLY);
	assert(tiny_kernel_fd > 0);
	/* map 4K into memory */
	userspace_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	assert(userspace_addr > 0);
	/* read tiny_kernel binary into the memory */
	ret = read(tiny_kernel_fd, userspace_addr, PAGE_SIZE);
	assert(ret >= 0);

	/* set user memory region */
	mem.slot = 0;
	mem.flags = 0;
	mem.guest_phys_addr = 0;
	mem.memory_size = PAGE_SIZE;
	mem.userspace_addr = (unsigned long)userspace_addr;
	ret = ioctl(vm_fd, KVM_SET_USER_MEMORY_REGION, &mem);
	assert(ret >= 0);

	/* get kvm_run */
	mmap_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, NULL);
	assert(mmap_size >= 0);
	kvm_run = (struct kvm_run *)mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0);
	assert(kvm_run >= 0);

	/* set cpu registers */
	ret = ioctl(vcpu_fd, KVM_GET_SREGS, &sregs);
	assert(ret >= 0);
	sregs.cs.base = 0;
	sregs.cs.selector = 0;
	ret = ioctl(vcpu_fd, KVM_SET_SREGS, &sregs);
	memset(&regs, 0, sizeof(struct kvm_regs));
	regs.rip = 0;
	ret = ioctl(vcpu_fd, KVM_SET_REGS, &regs);
	assert(ret >= 0);

	/* vcpu run */
	while (1) {
		ret = ioctl(vcpu_fd, KVM_RUN, NULL);
		assert(ret >= 0);

		switch (kvm_run->exit_reason) {
		case KVM_EXIT_HLT:
			printf("----KVM EXIT HLT----\n");
			close(kvm_fd);
			close(tiny_kernel_fd);
			return 0;
		case KVM_EXIT_IO:
			putchar(*(((char *)kvm_run) + kvm_run->io.data_offset));
			break;
		default:
			printf("Unknow exit reason: %d\n", kvm_run->exit_reason);
			break;
		}
	}

	return 0;
}
