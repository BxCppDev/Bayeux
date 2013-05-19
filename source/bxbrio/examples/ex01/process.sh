#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Dbrio_DIR=$(brio-config --prefix) \
  ..
make
make install

cd ..
ls -l

./ex01

rm -f ./ex01
rm -f ./ex01_data.brio
rm -fr ${build_dir}

cd ${opwd}

exit 0

# end