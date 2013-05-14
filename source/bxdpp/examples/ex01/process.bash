#!/usr/bin/env bash

opwd=$(pwd)

#which pandoc
#test $? = 0 && pandoc -r rst -w html README.rst > README.html
#which firefox
#test $? = 0 && firefox file://$(pwd)/README.html &

build_dir=$(pwd)/__build
#test -d ${build_dir} && rm -fr ${build_dir}

mkdir ${build_dir}
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

#rm -fr ./lib
#rm -fr ${build_dir}
rm -f *~

exit 0

# end