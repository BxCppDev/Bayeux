#!/usr/bin/env bash

echo >&2 "[info] Running run.sh..."

src_dir=
work_dir=
serial_downgrade=0

while [ -n "$1" ]; do
    token="$1"
    if [ "x${token}" == "x--work-dir" ]; then
	shift 1
	work_dir=$1
    elif [ "x${token}" == "x--source-dir" ]; then
	shift 1
	src_dir=$1
    elif [ "x${token}" == "x--downgrade" ]; then
	serial_downgrade=1
    fi
    shift 1
done

if [ ! -d ${src_dir} ]; then
    echo >&2 "[error] Source dir '${src_dir}' does not exist! Abort!"
    exit 1
fi
echo >&2 "[info] Source dir       = '${src_dir}'"

if [ ! -d ${work_dir} ]; then
    echo >&2 "[error] Working dir '${work_dir}' does not exist! Abort!"
    exit 1
fi
echo >&2 "[info] Working dir      = '${work_dir}'"
echo >&2 "[info] Serial downgrade = ${serial_downgrade}"

# Boost Archive version
bav=$(${work_dir}/exbav)
echo >&2 "[info] Current Boost Archive version = '${bav}'"

if [ ! -d ${src_dir}/samples/bs${bav} ]; then
    echo "[error] Boost archive version ${bav} sample directory '${src_dir}/samples/bs${bav}' does not exist ! Skip this test!" 1>&2
    exit 0
fi

echo -e "\nLoad the sample MC hit data file (version 1)..." 1>&2
${work_dir}/exload -i ${src_dir}/samples/out-1.xml -o check-out-1.xml > test-1.txt
if [ $? -ne 0 ]; then
    echo "[error] exload failed !" 1>&2
    exit 1
fi
   
if [ ${serial_downgrade} -eq 0 ]; then
    echo -e "\nCompare sample MC hit data file with check file (version 2)..." 1>&2
    diff ${src_dir}/samples/bs${bav}/out-2.xml check-out-1.xml
    if [ $? -ne 0 ]; then
	echo "[error] not matching archives of 'mctools::base_step_hit'!" 1>&2	
	exit 1
    fi
    
    echo -e "\nLoad the sample MC hit data file (version 2)..." 1>&2
    ${work_dir}/exload -i ${src_dir}/samples/bs${bav}/out-2.xml -o check-out-2.xml > test-2.txt
    if [ $? -ne 0 ]; then
	echo "[error] exload failed !" 1>&2
	exit 1
    fi
   
    diff test-1.txt test-2.txt
    if [ $? -ne 0 ]; then
	echo "[error] not matching deserialized instances of 'mctools::base_step_hit'!" 1>&2	
	exit 1
    else
	echo >&2 "[info] Output text files match."
    fi
fi

exit 0

# end
