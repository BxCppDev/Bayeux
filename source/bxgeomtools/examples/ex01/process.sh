#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

pandoc -r rst -w html README.rst -o geomtools-ex01_README.html

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

echo "Run the 'ex01' program..." 1>&2
./ex01

echo "Browse the geometry setup via the 'geomtools_inspector' program..." 1>&2
geomtools_inspector \
   --manager-config config/manager.conf \
   --with-gdml \
   --with-visu --visu-view-3d

rm -f ./ex01
rm -f ./geomtools-ex01.C
rm -f ./geomtools-ex01.gdml
rm -f ./geomtools-ex01_README.html
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;


cd ${opwd}

exit 0

# end