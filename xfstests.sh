#!/bin/bash

# Author:	Zeng Linggang <zenglg.jy@cn.fujitsu.com>
# Date:		2014-09-03
# Edit:
#		2015-09-10	Zeng Linggang <zenglg.jy@cn.fujitsu.com>
# File:
#		1. /root/local.config : xfstests config file.
#		2. /root/filter.sh : filter the issue test(s).

fail()
{
	echo $@
	exit 1
}

if [ $# -ne 1 ]; then
	fail "Usage: $0 <ext4|xfs>"
fi

if [ $1 != "ext4" -a $1 != "xfs" ]; then
	fail "$1 should be ext4 or xfs"
fi
fstype=$1

XFSTEST_CONFIG="/root/local.config"
if [ ! -f "$XFSTEST_CONFIG" ]; then
	fail "$XFSTEST_CONFIG is not exist"
fi

TEST_DEV=`grep "TEST_DEV=/dev/" $XFSTEST_CONFIG | awk -F '=' '{print $2}'`
if [ -z $TEST_DEV ]; then
	fail "$TEST_DEV is not set in $XFSTEST_CONFIG"
fi

SCRATCH_DEV=`grep "SCRATCH_DEV=/dev/" $XFSTEST_CONFIG | awk -F '=' '{print $2}'`
if [ -z $SCRATCH_DEV ]; then
	fail "$SCRATCH_DEV is not set in $XFSTEST_CONFIG"
fi

XFSTEST_CHECK_LOG="/root/xfstest_check.log"
if [ -f $XFSTEST_CHECK_LOG ]; then
	rm -rf $XFSTEST_CHECK_LOG
fi

rpm -qi libuuid-devel &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "libuuid-devel is not installed"
fi

rpm -qi xfsprogs-devel &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "xfsprogs-devel is not installed"
fi

rpm -qi libattr-devel &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "libattr-devel is not installed"
fi

rpm -qi libacl-devel &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "libacl-devel is not installed"
fi

rpm -qi libaio-devel &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "libaio-devel is not installed"
fi

rpm -qi fio &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "fio is not installed"
fi

rpm -qi yp-tools &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "yp-tools is not installed"
fi

rpm -qi dump &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "dump is not installed"
fi

grep -q fsgqa /etc/passwd
if [ $? -ne 0 ]; then
	useradd fsgqa
fi

# Check dbench. If not installed, some tests return [not run].
#rpm -qi dbench &>> $XFSTEST_CHECK_LOG
#if [ $? -ne 0 ]; then
#	echo "dbench is not installed"
#	exit 1
#fi

if [ ! -d "/root/xfstests" ]; then
	git clone git://10.167.225.115/git2/oss.sgi.com/xfs/cmds/xfstests.git /root/xfstests
	if [ $? -ne 0 ]; then
		fail "git clone"
	fi
fi

if [ -d "/var/lib/xfstests/" ]; then
	rm -rf "/var/lib/xfstests/"
fi

if [ -f "/root/xfstests.log" ]; then
	rm -rf "/root/xfstests.log"
fi

cd "/root/xfstests"

git pull
if [ $? -ne 0 ]; then
	fail "git pull ERROR!"
fi

make clean
if [ $? -ne 0 ]; then
	fail "make clean ERROR!"
fi

make &>> $XFSTEST_CHECK_LOG
if [ $? -ne 0 ]; then
	fail "make ERROR! Please check $XFSTEST_CHECK_LOG"
fi

make install
if [ $? -ne 0 ]; then
	fail "make install ERROR!"
fi

if [ -f /root/filter.sh ]; then
	/root/filter.sh
fi

cp $XFSTEST_CONFIG "/var/lib/xfstests/"

cd /var/lib/xfstests/

cat /proc/self/mounts | grep -q "$TEST_DEV"
if [ $? -eq 0 ]; then
	if ! umount "$TEST_DEV"; then
		fail "umount ERROR! Please check $TEST_DEV in /proc/self/mounts"
	fi
fi

if [ ${fstype} == "ext4" ]; then
	mkfs -t ${fstype} "$TEST_DEV"
elif [ ${fstype} == "xfs" ]; then
	mkfs -t ${fstype} -f "$TEST_DEV"
fi

cat /proc/self/mounts | grep -q "$SCRATCH_DEV"
if [ $? -eq 0 ]; then
	if ! umount "$SCRATCH_DEV"; then
		fail "umount ERROR! Please check $SCRATCH_DEV in /proc/self/mounts"
	fi
fi
if [ ${fstype} == "ext4" ]; then
	mkfs -t ${fstype} "$SCRATCH_DEV"
elif [ ${fstype} == "xfs" ]; then
	mkfs -t ${fstype} -f "$SCRATCH_DEV"
fi

cat /proc/self/mounts | grep -q "/mnt"
if [ $? -eq 0 ]; then
	if ! umount "/mnt"; then
		fail "umount ERROR! Please check /mnt in /proc/self/mounts"
	fi
fi

if [ ! -d "/mnt/xfstests/test" ]; then
	mkdir -p "/mnt/xfstests/test"
fi

if [ ! -d "/mnt/xfstests/scratch" ]; then
	mkdir -p "/mnt/xfstests/scratch"
fi

xfstests_log="$(date +"%Y%m%d")_${fstype}_$(hostname).log"
echo "/var/lib/xfstests/check &> /home/yl/${xfstests_log}" > /root/at.log

at now +1 minutes -f /root/at.log

rm -rf /root/at.log
