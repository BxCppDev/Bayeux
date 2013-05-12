#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

install_dir=$(pwd)

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=${install_dir} \
  -Dcuts_DIR=$(cuts-config --prefix) \
  ..
make
make install

cd ${opwd}

./ex_manager
gnuplot ex_manager.gp

rm -f ex_manager.orig.data
rm -f ex_manager.selected.data
rm -f ex_manager
rm -fr ${build_dir}
rm -f *~

exit 0

# end