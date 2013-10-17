#!/usr/bin/env bash

use_bayeux=1
which bayeux-config > /dev/null 2>&1
if [ $? -ne 0 ]; then
    use_bayeux=0
fi

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

if [ $use_bayeux -eq 0 ]; then
    which datatools-config
    if [ $? -ne 0 ]; then
	echo "ERROR: Missing datatools-config !" >&2
	exit 0
    fi
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(datatools-config --prefix) \
	..
else
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
	-DUSE_BAYEUX:BOOLEAN=1 \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(datatools-config --prefix) \
	..
fi
make
make install

cd ${opwd}

ls -l ./lib/

./ex_serializable_1 --debug

ls -l *.xml

rm -f raw_hit.xml
rm -f raw_hits.xml
rm -f raw_hits_h.xml
rm -f raw_hit_banks.xml
rm -f raw_data_banks.xml
rm -f ./ex_serializable_1
rm -fr ./lib
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

exit 0

# end
