#!/usr/bin/env bash

use_bayeux=1
which bayeux-config > /dev/null 2>&1
if [ $? -ne 0 ]; then
    use_bayeux=0
fi

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

pandoc -r rst -w html README.rst -o geomtools-ex01_README.html

mkdir ${build_dir}
cd ${build_dir}

if [ $use_bayeux -eq 0 ]; then
    which geomtools-config
    if [ $? -ne 0 ]; then
	echo "ERROR: Missing geomtools-config !" >&2
	exit 0
    fi
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
    	-Dgeomtools_DIR=$(geomtools-config --prefix) \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(geomtools-config --prefix) \
	..
    #	-Dgeomtools_DIR=$(geomtools-config --prefix) \
    geomtools_inspector_bin="geomtools_inspector"
else
    echo "NOTICE: Using Bayeux..." >&2
    cmake \
	-DCMAKE_INSTALL_PREFIX=.. \
	-DUSE_BAYEUX:BOOLEAN=1 \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(bayeux-config --prefix) \
	..
    geomtools_inspector_bin="bxgeomtools_inspector"
fi
make
make install

cd ${opwd}
ls -l

echo "Run the 'ex01' program..." 1>&2
./ex01

echo "Browse the geometry setup via the '${geomtools_inspector_bin}' program..." 1>&2
${geomtools_inspector_bin} \
   --manager-config config/manager.conf \
   --with-visu --visu-view-3d

rm -f ./ex01
rm -f ./geomtools-ex01.C
rm -f ./geomtools-ex01.gdml
rm -f ./geomtools-ex01_README.html
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;


cd ${opwd}

exit 0

# end
