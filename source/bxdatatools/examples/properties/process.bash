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
	-DCMAKE_FIND_ROOT_PATH:PATH=$(bayeux-config --prefix) \
	..
fi
make
make install

cd ${opwd}

./ex_properties --debug

cat foo.conf

rm -f foo.conf
rm -f ex_properties
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

exit 0

# end
