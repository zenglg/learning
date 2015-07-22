#!/bin/sh

mk_tmp()
{
	TMP=/tmp/fnand2_test/
	if [ ! -e ${TMP} ]; then
		mkdir -p ${TMP}
	fi
}

rm_tmp()
{
	rm -rf ${TMP}
}

setup()
{
	which mtd_debug >/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "mtd_debug is not install?"
		exit 1
	fi

	ERASE_SIZE=$(mtd_debug info ${MTD_DEVICE} | grep erasesize | \
			awk '{print $3}')

	WRITE_SIZE=$(mtd_debug info ${MTD_DEVICE} | grep writesize | \
			awk '{print $3}')

	mk_tmp
}

cleanup()
{
	rm_tmp
}

md5sum_check()
{
	local file1=$1
	local file2=$2

	ret1=$(md5sum ${file1} | awk '{print $1}')
	ret2=$(md5sum ${file2} | awk '{print $1}')
	if [ ${ret1} = ${ret2} ]; then
		return 0
	else
		echo "${ret1} is diff with ${ret2}"
		return 1
	fi
}

fnand2_debug_read()
{
	local device=$1
	local offset=$(($2*${ERASE_SIZE}))

	mtd_debug read ${device} ${offset} ${WRITE_SIZE} ${TMP}/read.data \
		>/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "mtd_debug read ${offset} <${WRITE_SIZE}>: failed."
		echo "	--'dmesg' for the detail."
		return 1
	fi

	md5sum_check ${TMP}/read.data ${TMP}/write.data
	if [ $? -ne 0 ]; then
		echo "mtd_debug read: md5sum_check failed."
	else
		echo "mtd_debug read ${offset} <${WRITE_SIZE}>: passed."
	fi
}

fnand2_debug_write()
{
	local device=$1
	local offset=$(($2*${ERASE_SIZE}))

	dd if=/dev/urandom of=${TMP}/write.data bs=1024 \
		count=$((${WRITE_SIZE}/1024)) >/dev/null 2>&1

	mtd_debug write ${device} ${offset} ${WRITE_SIZE} ${TMP}/write.data \
		>/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "mtd_debug write ${offset} <${WRITE_SIZE}>: failed."
		echo "	--'dmesg' for the detail."
	else
		echo "mtd_debug write ${offset} <${WRITE_SIZE}>: passed."
	fi
}

fnand2_debug_erase()
{
	local device=$1
	local offset=$(($2*${ERASE_SIZE}))

	mtd_debug erase ${device} ${offset} ${ERASE_SIZE} >/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "mtd_debug erase ${offset} <${ERASE_SIZE}>: failed."
		echo "	--'dmesg' for the detail."
	else
		echo "mtd_debug erase ${offset} <${ERASE_SIZE}>: passed."
	fi
}

fnand2_debug_test()
{
	for i in ${MTD_TEST_BLOCKS}
	do
		echo "TEST $i blocks:"
		fnand2_debug_erase ${MTD_DEVICE} $i
		fnand2_debug_write ${MTD_DEVICE} $i
		fnand2_debug_read ${MTD_DEVICE} $i
		echo
	done
}

if [ $# != 1 ]; then
	echo "Usage: $0 <mtd-device>"
	exit 1
fi

if [ ! -e $1 ]; then
	echo "$1 is not exist"
	exit 1
fi

MTD_DEVICE=$1

MTD_TEST_BLOCKS="0 8 16 1024 2048 4096"

setup

fnand2_debug_test

cleanup

exit
