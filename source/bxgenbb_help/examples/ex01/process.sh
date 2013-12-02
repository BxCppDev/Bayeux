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
ls -l

echo "Running ex01..." 1>&2
./ex01
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex01 failed !" 1>&2
    my_exit 1
fi
genbb_inspector_bin="bxgenbb_inspector"

which ${genbb_inspector_bin} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: No '${genbb_inspector_bin}' executable !" 1>&2
    my_exit 1
fi

echo "List available event generators..." 1>&2
${genbb_inspector_bin} \
    --configuration config/manager.conf \
    --action list
if [ $? -ne 0 ]; then
    echo "ERROR: List event generators failed !" 1>&2
    my_exit 1
fi

echo "Shoot events..." 1>&2
${genbb_inspector_bin} \
    --configuration config/manager.conf \
    --action shoot \
    --generator "Co60" \
    --prng-seed 314159 \
    --number-of-events 55 \
    --output-file "histos_Co60.root"
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot events failed !" 1>&2
    my_exit 1
fi

rm -f ./ex01
rm -f ./histos_Co60.root
rm -fr ${build_dir}

cd ${opwd}

my_exit 0

# end
