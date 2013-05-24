#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Dgenvtx_DIR=$(genvtx-config --prefix) \
  ..
make
make install

cd ..
ls -l

echo "Check the  geometry setup : " 1>&2
geomtools_check_setup \
   --manager-config config/geometry/manager.conf \
   --with-gdml \
   --with-visu --visu-view-3d


echo "Run the example program : " 1>&2
./ex01 > ex01_vertices.data
gnuplot ./ex01.gp

echo "Run the genvtx_production program : " 1>&2
genvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --list

 genvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --shoot \
	       --number-of-vertices 10000 \
	       --prng-seed 314159 \
	       --vertex-generator "from_every_pcb_bulk.vg" \
               --output-file "genvtx_ex01_vertices.txt" \
	       --visu --tiny --tiny


rm -f ./ex01
rm -f ./ex01_vertices.data
rm -f ./genvtx_ex01_vertices.txt
rm -f ./genvtx-example-ex01.gdml
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

cd ${opwd}

exit 0

# end