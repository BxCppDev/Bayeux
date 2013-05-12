#!/usr/bin/env bash

opwd=$(pwd)

which pandoc
test $? = 0 && pandoc -r rst -w html README.rst > README.html
which firefox
test $? = 0 && firefox file://$(pwd)/README.html &

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Dmaterials_DIR=$(materials-config --prefix) \
  ..
make
make install

cd ${opwd}

./ex_manager

rm -f ex_manager
rm -fr ${build_dir}
rm -f *~

exit 0

# end