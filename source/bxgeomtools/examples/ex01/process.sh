#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Dgeomtools_DIR=$(geomtools-config --prefix) \
  ..
make
make install

cd ..
ls -l

./ex01

geomtools_check_setup \
   --manager-config config/manager.conf \
   --with-gdml \
   --with-visu --visu-view-3d

rm -f ./ex01
rm -f ./geomtools-ex01.gdml
rm -fr ${build_dir}

cd ${opwd}

exit 0

# end