#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Ddatatools_DIR=$(datatools-config --prefix) \
  ..
make
make install

cd ${opwd}

./ex_things_1 --debug

rm -f ex_things_1
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

exit 0

# end