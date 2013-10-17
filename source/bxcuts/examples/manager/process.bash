#!/usr/bin/env bash

use_bayeux=1
which bayeux-config > /dev/null 2>&1
if [ $? -ne 0 ]; then
    use_bayeux=0
fi

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

install_dir=$(pwd)

mkdir ${build_dir}
cd ${build_dir}

if [ $use_bayeux -eq 0 ]; then
    which cuts-config
    if [ $? -ne 0 ]; then
	echo "ERROR: Missing cuts-config !" >&2
	exit 0
    fi
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
    	-Dcuts_DIR=$(cuts-config --prefix) \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(cuts-config --prefix) \
	..
else
    echo "NOTICE: Using Bayeux..." >&2
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
	-DUSE_BAYEUX:BOOLEAN=1 \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(bayeux-config --prefix) \
	..
fi
make
make install

cd ${opwd}

echo "Run the 'ex_manager' program..." 1>&2
./ex_manager
gnuplot ex_manager.gp

rm -f ex_manager.orig.data
rm -f ex_manager.selected.data
rm -f ex_manager
rm -fr ${build_dir}
rm -f *~

exit 0

# end
