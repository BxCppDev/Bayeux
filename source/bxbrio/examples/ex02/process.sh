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

./ex02

rm -f ./ex02
rm -f ./ex02_data.brio
rm -fr ${build_dir}

cd ${opwd}

exit 0

# end