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

rootcint brio_dict.cc \
 -c -I${SOURCE_PATH}/include -I${BINARY_PATH}/include \
 ${SOURCE_PATH}/include/brio/detail/TArrayCMod.h \
 ${SOURCE_PATH}/include/brio/detail/brio_record.h > /dev/null 2>&1
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
