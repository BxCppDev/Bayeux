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

ls -l ./lib/

./ex_serializable_1 --debug

ls -l *.xml

rm -f raw_hit.xml
rm -f raw_hits.xml
rm -f raw_hits_h.xml
rm -f raw_hit_banks.xml
rm -f raw_data_banks.xml
rm -f ./ex_serializable_1
rm -fr ./lib
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

exit 0

# end