#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

install_dir=$(pwd)/__install
test -d ${install_dir} && rm -fr ${install_dir}

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=${install_dir} \
  -Ddatatools_DIR=$(datatools-config --prefix) \
  ..
make
make install

cd ${opwd}

exit 0

# end