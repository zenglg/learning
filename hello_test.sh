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

setup()
{
	rod make
}

cleanup()
{
	rod make clean
}

test_hello_setup()
{
	rod_silent sudo dmesg -c

	rod lsmod | grep hello
}

test_hello_cleanup()
{
	rod lsmod | grep hello

	rod sudo rmmod hello

	rod dmesg
}

test_hello()
{
	test_hello_setup

	rod sudo insmod hello.ko

	test_hello_cleanup
}

test_sudoku_setup()
{
	rod ./sudoku sudoku_data > sudoku_out
}

test_sudoku_cleanup()
{
	rod rm -rf sudoku sudoku_out
}

test_sudoku()
{
	test_sudoku_setup

	rod diff sudoku_data_out sudoku_out

	test_sudoku_cleanup
}

test_globalmem_setup()
{
	rod_silent sudo dmesg -c

	rod lsmod | grep globalmem

	rod sudo insmod globalmem.ko

	rod lsmod | grep globalmem

	rod sudo mknod /dev/globalmem c 230 0
}

test_globalmem_cleanup()
{
	rod sudo rm -rf /dev/globalmem

	rod sudo rmmod globalmem

	rod dmesg
}

test_globalmem()
{
	test_globalmem_setup

	rod cat /dev/globalmem

	test_globalmem_cleanup
}

setup

test_hello

test_sudoku

test_globalmem

cleanup
