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

which pandoc
test $? = 0 && pandoc -r rst -w html README.rst > README.html
# which firefox
# test $? = 0 && firefox file://$(pwd)/README.html &


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
materials_inspector_bin="bxmaterials_inspector"

which ${materials_inspector_bin} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: No '${materials_inspector_bin}' executable !" 1>&2
    my_exit 1
fi

${materials_inspector_bin} --manager-config "config/manager.conf" --with-decoration -M
if [ $? -ne 0 ]; then
    echo "ERROR: Materials inspector failed !" 1>&2
    my_exit 1
fi

./ex_manager
if [ $? -ne 0 ]; then
    echo "ERROR: Example program failed !" 1>&2
    my_exit 1
fi

rm -f ex_manager
rm -f README.html
rm -fr ${build_dir}
rm -f *~

my_exit 0

# end
