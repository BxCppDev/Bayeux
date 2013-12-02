#!/usr/bin/env bash


which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Missing Bayeux's bxquery !" >&2
    exit 1
fi

opwd=$(pwd)

function my_exit()
{
    cd ${opwd}
    exit $1
}


build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=.. \
    -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
    ..
if [ $? -ne 0 ]; then
    echo "ERROR: Configuration failed !" 1>&2
    my_exit 1
fi
make
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed !" 1>&2
    my_exit 1
fi
make install
if [ $? -ne 0 ]; then
    echo "ERROR: Installation failed !" 1>&2
    my_exit 1
fi

cd ${opwd}

ls -l ./lib/

./ex_serializable_1 --debug
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ./ex_serializable_1 failed !" 1>&2
    my_exit 1
fi

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

my_exit 0

# end
