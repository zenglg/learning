#!/bin/sh

function rod()
{
	$@
	if [ $? -ne 0 ]; then
		echo "'$@' failed"
		exit 1
	fi
}

function rod_silent()
{
	$@ >/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "'$@' failed"
		exit 1
	fi
}

rod_silent sudo dmesg -c

ls -al /lib/modules/$(uname -r)

rod ls -al /lib/modules/$(uname -r) | grep build

# I donot know how to install the kernel-dev package in ubuntu
# So, exit directly.
# I will fix it.
exit 0

rod make

rod lsmod | grep hello

rod sudo insmod hello.ko

rod lsmod | grep hello

rod sudo rmmod hello

rod make clean

rod dmesg
