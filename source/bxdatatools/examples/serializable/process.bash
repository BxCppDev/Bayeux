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

install_dir=$(pwd)/_install.d
test -d ${install_dir} && rm -fr ${install_dir}
build_dir=$(pwd)/_build.d
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=${install_dir} \
    -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
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

${install_dir}/ex_serializable_1 --debug
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
rm -fr ${build_dir}
rm -fr ${install_dir}
find . -name "*~" -exec rm -f \{\} \;

my_exit 0

# end
