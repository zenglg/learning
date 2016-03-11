#!/bin/sh

dmesg -c

make

lsmod | grep hello

insmod hello.ko

lsmod | grep hello

rmmod hello

make clean

dmesg
