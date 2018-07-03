#!/bin/sh

rod()
{
	$@
	if [ $? -ne 0 ]; then
		echo "'$@' failed"
		exit 1
	fi
}

rod_silent()
{
	$@ >/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "'$@' failed"
		exit 1
	fi
}

test_hello()
{
	rod_silent sudo dmesg -c

	rod ls -al /lib/modules/$(uname -r) | grep build

	rod make

	rod lsmod | grep hello

	rod sudo insmod hello.ko

	rod lsmod | grep hello

	rod sudo rmmod hello

	rod make clean

	rod dmesg
}

test_suduku()
{
	rod gcc -o sudoku sudoku.c

	rod ./sudoku sudoku_data > sudoku_out

	rod diff sudoku_data_out sudoku_out

	rod rm -rf sudoku sudoku_out
}

test_hello

test_suduku
