#!/usr/bin/env bash

opwd=$(pwd)

which pandoc
test $? -eq 0 && pandoc -r rst -w html README.rst > README.html
which firefox
test $? -eq 0 && firefox file://$(pwd)/README.html &

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}
test -d ${build_dir} || mkdir ${build_dir}

cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Ddpp_DIR=$(dpp-config --prefix) \
  ..
if [ $? -ne 0 ]; then
    cd ${opwd}
    exit 1
fi
make
if [ $? -ne 0 ]; then
    cd ${opwd}
    exit 1
fi
make install
if [ $? -ne 0 ]; then
    cd ${opwd}
    exit 1
fi

cd ${opwd}

echo "" 1>&2
echo "dpp_ex01 DLL linkage is :" 1>&2
find ./lib -name "libdpp_ex01.*" -exec ldd \{\} \; 1>&2
echo "" 1>&2
echo "dpp_ex01_bio DLL linkage is :" 1>&2
find ./lib -name "libdpp_ex01_bio.*" -exec ldd \{\} \; 1>&2


echo "" 1>&2
echo "Run the ./test_dpp_ex01 executable :" 1>&2
./test_dpp_ex01
ls -l ./test_dpp_ex01.xml

echo "" 1>&2
echo "Run the dpp_processing executable :" 1>&2

export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}
dpp_processing \
    --load-dll "dpp_ex01_bio" \
    --modulo 25 \
    --max-records 1000 \
    --module-manager-config "config/module_manager.conf" \
    --module "pipeline" \
    --max-records-per-output-file 250 \
    --output-file dpp_ex01_01.xml \
    --output-file dpp_ex01_02.xml \
    --output-file dpp_ex01_03.xml \
    --output-file dpp_ex01_04.xml
if [ $? -ne 0 ]; then
    echo "ERROR: dpp_processing has failed !" 1>&2
    exit 1
fi
ls -l ./dpp_ex01_intermediate_*.brio
ls -l ./dpp_ex01_0?.xml

rm -f test_dpp_ex01.xml
rm -f test_dpp_ex01
rm -f dpp_ex01_0?.xml
rm -f dpp_ex01_intermediate_*.brio
rm -fr ./lib
rm -fr ${build_dir}
rm -f README.html
find . -name "*~" -exec rm -f \{\} \;

exit 0

# end