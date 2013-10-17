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
    which datatools-config
    if [ $? -ne 0 ]; then
	echo "ERROR: Missing datatools-config !" >&2
	exit 0
    fi
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(datatools-config --prefix) \
	..
    ocd_manual_bin="ocd_manual"
else
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
	-DUSE_BAYEUX:BOOLEAN=1 \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(bayeux-config --prefix) \
	..
    ocd_manual_bin="bxocd_manual"
fi
make
make install

cd ${opwd}

ls -l ./lib/
ls -l ./ex_OCD

echo "Run the example program : " 1>&2
LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} ./ex_OCD

LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} \
    ${ocd_manual_bin} --load-dll "datatools_ex_OCD" \
    --class-id "foo"            \
    --action show > foo_ocd.rst

which pandoc > /dev/null 2>&1
if [ $? -eq 0 ]; then
    pandoc -r rst -w html foo_ocd.rst > foo_ocd.html
fi

### firefox file://$(pwd)/foo_ocd.html &

rm -f foo_ocd.html
rm -f foo_ocd.rst
rm -f ex_OCD.foo.doc.rst
rm -f ex_OCD.foo.skeleton.conf
rm -f ex_OCD.service_manager.doc.rst
rm -f ex_OCD.service_manager.skeleton.conf
rm -f ex_OCD
rm -fr ./lib
rm -fr ${build_dir}

exit 0

# end
