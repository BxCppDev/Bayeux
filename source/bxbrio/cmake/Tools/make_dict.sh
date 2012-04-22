#!/usr/bin/env bash

cat>&2 <<EOF
--------------------------------------------------------------
Prepare class dictionnary and streamer stuff for classes:
 - TArrayCMod
 - brio_record

Working directory is:
$(pwd)
--------------------------------------------------------------
EOF

test -f brio_dict.h  && rm -f brio_dict.h
test -f brio_dict.cc && rm -f brio_dict.cc

rootcint_exe=""

while [ -n "$1" ]; do
    token="$1"
    if [ "x${token}" = "x--rootcint" ]; then
	shift 1
	rootcint_exe="$1"
    fi
    shift 1
done

if [ "x${rootcint_exe}" = "x" ]; then
    rootcint_exe=rootcint
fi

which ${rootcint_exe}
if [ $? -ne 0 ]; then
    echo "ERROR: make_dict.sh: Cannot find '${rootcint_exe}' ! Abort !"
    exit 1
fi

tmp_test_dir=/tmp/${USER}/brio
if [ ! -d ${tmp_test_dir} ]; then
    mkdir -p ${tmp_test_dir}
fi
tmp_log_file=${tmp_test_dir}/root_dictionnary.log

${rootcint_exe} brio_dict.cc \
 -c -I${SOURCE_PATH}/include \
 ${SOURCE_PATH}/include/brio/detail/TArrayCMod.h \
 ${SOURCE_PATH}/include/brio/detail/brio_record.h > ${tmp_log_file} 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: make_dict.sh: ROOT dictionary generation failed ! Abort !"
    exit 1
fi

# cat brio_dict.cc | sed \
#   -e 's@#include "brio_dict.h"@#include <brio/detail/brio_dict.h>@g' \
#   -e 's@void TArrayCMod::Streamer@void TArrayCMod::__Streamer@g' \
#     > ${BINARY_PATH}/src/brio_dict.cc
###cat brio_dict.cc > ${BINARY_PATH}/src/brio_dict.cc
cat brio_dict.cc | sed \
  -e 's@void TArrayCMod::Streamer@void TArrayCMod::__Streamer@g' \
    > ${BINARY_PATH}/src/brio_dict.cc

# cat brio_dict.h | sed \
#   -e 's@#include "include/brio/detail/brio_record.h"@#include <brio/detail/brio_record.h>@g' \
#   -e 's@#include "include/brio/detail/TArrayCMod.h"@#include <brio/detail/TArrayCMod.h>@g' \
#   > ${BINARY_PATH}/src/brio_dict.h
cat brio_dict.h > ${BINARY_PATH}/src/brio_dict.h

test -f brio_dict.h && rm -f brio_dict.cc
test -f brio_dict.h && rm -f brio_dict.h

ls -l ${BINARY_PATH}/src/brio_dict.h
ls -l ${BINARY_PATH}/src/brio_dict.cc
cat>&2 <<EOF
--------------------------------------------------------------
EOF
exit 0

# end
