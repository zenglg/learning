#!/bin/sh

sudo dmesg -c

make

lsmod | grep hello

sudo insmod hello.ko

lsmod | grep hello

sudo rmmod hello

make clean

dmesg
