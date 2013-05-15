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

./ex_properties --debug

cat foo.conf

rm -f foo.conf
rm -f ex_properties
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

exit 0

# end