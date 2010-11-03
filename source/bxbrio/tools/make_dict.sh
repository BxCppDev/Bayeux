#!/usr/bin/env bash

cat>&2 <<EOF

Prepare class dictionnary and streamer stuff for classes:
 - TArrayCMod
 - brio_record

EOF
test -f brio_dict.h && rm -f brio_dict.h
test -f brio_dict.cc && rm -f brio_dict.cc

rootcint brio_dict.cc \
 -c -I./include \
 include/brio/TArrayCMod.h \
 include/brio/brio_record.h 

cat brio_dict.cc | sed \
  -e 's@#include "brio_dict.h"@#include <brio/brio_dict.h>@g' \
  -e 's@void TArrayCMod::Streamer@void TArrayCMod::__Streamer@g' \
  > src/brio_dict.cc

cat brio_dict.h | sed \
  -e 's@#include "include/brio/brio_record.h"@#include <brio/brio_record.h>@g' \
  -e 's@#include "include/brio/TArrayCMod.h"@#include <brio/TArrayCMod.h>@g' \
  > include/brio/brio_dict.h


rm -f brio_dict.cc
rm -f brio_dict.h 

ls -l include/brio/brio_dict.h
ls -l src/brio_dict.cc

exit 0

# end
