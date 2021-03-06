#!/bin/bash

# Author:	Linggang Zeng <linggang.zeng@easystack.cn>
# Date:		2014-09-03
# Edit:
#		2015-09-10	Linggang Zeng <linggang.zeng@easystack.cn>
#		2015-11-17	Linggang Zeng <linggang.zeng@easystack.cn>
# File:
#		1. /root/local.config : xfstests config file.
#		2. /root/filter.sh :	filter the issue test(s).

fail()
{
	echo "'$@' failed"
	exit 1
}

rod()
{
	$@ >${XFSTEST_CHECK_LOG} 2>&1
	if [ $? -ne 0 ]; then
		cat ${XFSTEST_CHECK_LOG}
		fail $@
	fi
}

if [ $# -ne 1 ]; then
	fail "Usage: $0 <ext4|xfs>"
fi

if [ $1 != "ext4" -a $1 != "xfs" ]; then
	fail "$1 should be ext4 or xfs"
fi
fstype=$1

XFSTEST_CONFIG="/root/local.config"
if [ ! -f "${XFSTEST_CONFIG}" ]; then
	fail "${XFSTEST_CONFIG} is not exist"
fi

TEST_DEV=$(awk -F '=' '/^TEST_DEV=/ {print $2}' ${XFSTEST_CONFIG})
if [ -z ${TEST_DEV} ]; then
	fail "TEST_DEV is not set in ${XFSTEST_CONFIG}"
fi

TEST_DIR=$(awk -F '=' '/^TEST_DIR=/ {print $2}' ${XFSTEST_CONFIG})
if [ -z ${TEST_DIR} ]; then
	fail "TEST_DIR is not set in ${XFSTEST_CONFIG}"
fi

SCRATCH_DEV=$(awk -F '=' '/^SCRATCH_DEV=/ {print $2}' ${XFSTEST_CONFIG})
if [ -z ${SCRATCH_DEV} ]; then
	fail "SCRATCH_DEV is not set in ${XFSTEST_CONFIG}"
fi

SCRATCH_MNT=$(awk -F '=' '/^SCRATCH_MNT=/ {print $2}' ${XFSTEST_CONFIG})
if [ -z ${SCRATCH_MNT} ]; then
	fail "SCRATCH_MNT is not set in ${XFSTEST_CONFIG}"
fi

XFSTEST_CHECK_LOG="/root/xfstests_check.log"
if [ -f ${XFSTEST_CHECK_LOG} ]; then
	rm -rf ${XFSTEST_CHECK_LOG}
fi

rod rpm -qi libuuid-devel

rod rpm -qi xfsprogs-devel

rod rpm -qi libattr-devel

rod rpm -qi libacl-devel

rod rpm -qi libaio-devel

rod rpm -qi fio

rod rpm -qi yp-tools

rod rpm -qi dump

rod rpm -qi git

rod rpm -qi automake

rod rpm -qi gcc

rod rpm -qi libtool

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

GIT_URL="git://10.167.225.115/git2/oss.sgi.com/xfs/cmds/xfstests.git"
XFSTESTS_DIR="/root/xfstests"
if [ ! -d "${XFSTESTS_DIR}" ]; then
	rod git clone ${GIT_URL} ${XFSTESTS_DIR}
else
	cd "${XFSTESTS_DIR}"
	rod git pull
fi

if [ -d "/var/lib/xfstests/" ]; then
	rm -rf "/var/lib/xfstests/"
fi

if [ -f "${XFSTESTS_DIR}.log" ]; then
	rm -rf "${XFSTESTS_DIR}.log"
fi

rod make clean

rod make

rod make install

if [ -f /root/filter.sh ]; then
	/root/filter.sh
fi

cp ${XFSTEST_CONFIG} "/var/lib/xfstests/"

cd /var/lib/xfstests/

MOUNTS="/proc/self/mounts"

grep -q ${TEST_DEV} ${MOUNTS}
if [ $? -eq 0 ]; then
	if ! umount ${TEST_DEV}; then
		fail "umount ERROR! Please check ${TEST_DEV} in ${MOUNTS}"
	fi
fi

if [ ${fstype} == "ext4" ]; then
	rod mkfs -t ${fstype} ${TEST_DEV}
elif [ ${fstype} == "xfs" ]; then
	rod mkfs -t ${fstype} -f ${TEST_DEV}
fi

grep -q ${SCRATCH_DEV} ${MOUNTS}
if [ $? -eq 0 ]; then
	if ! umount ${SCRATCH_DEV}; then
		fail "umount ERROR! Please check ${SCRATCH_DEV} in ${MOUNTS}"
	fi
fi

if [ ${fstype} == "ext4" ]; then
	rod mkfs -t ${fstype} ${SCRATCH_DEV}
elif [ ${fstype} == "xfs" ]; then
	rod mkfs -t ${fstype} -f ${SCRATCH_DEV}
fi

grep -q "/mnt" ${MOUNTS}
if [ $? -eq 0 ]; then
	if ! umount "/mnt"; then
		fail "umount ERROR! Please check /mnt in ${MOUNTS}"
	fi
fi

if [ ! -d "${TEST_DIR}" ]; then
	mkdir -p "${TEST_DIR}"
fi

if [ ! -d "${SCRATCH_MNT}" ]; then
	mkdir -p "${SCRATCH_MNT}"
fi

xfstests_log="$(date +"%Y%m%d")_${fstype}_$(hostname).log"
echo "/var/lib/xfstests/check &> /home/yl/${xfstests_log}" > /root/at.log

at now +1 minutes -f /root/at.log

rm -rf /root/at.log
