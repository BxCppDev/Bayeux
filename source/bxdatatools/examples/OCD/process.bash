#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

install_dir=$(pwd)

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Ddatatools_DIR=$(datatools-config --prefix) \
  ..
make
make install

cd ${opwd}

ls -l ./lib/
ls -l ./ex_OCD

echo "Run the example program : " 1>&2
LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} ./ex_OCD

LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} \
             ocd_manual --load-dll "datatools_ex_OCD" \
                          --class-id "foo"            \
                          --action show > foo_ocd.rst

pandoc -r rst -w html foo_ocd.rst > foo_ocd.html

### firefox file://$(pwd)/foo_ocd.html &

rm -f foo_ocd.html
rm -f foo_ocd.rst
rm -f ex_OCD.foo.doc.rst
rm -f ex_OCD.foo.skeleton.conf
rm -f ex_OCD.service_manager.doc.rst
rm -f ex_OCD.service_manager.skeleton.conf
rm -f ex_OCD
rm -fr ./lib
rm -fr ${build_dir}

exit 0

# end