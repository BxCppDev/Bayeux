#!/usr/bin/env bash

opwd=$(pwd)

function my_exit()
{
    cd ${opwd}
    exit $1
}

if [ ! -x ./bootstrap_standalone.bash ]; then
    echo "ERROR: This is not the devel directory !" >&2
    my_exit 1
fi

base_dir=$(pwd)
source_dir=$(pwd)/..
build_dir=$(pwd)/_build
install_dir=$(pwd)/_install

if [ ! -d ${build_dir} ]; then
    mkdir -p ${build_dir}
    if [ $? -ne 0 ]; then
	echo "ERROR: Cannot mkdir ' ${build_dir}' !" >&2
	my_exit 1
    fi
fi

cd ${build_dir}
cmake \
  -DCMAKE_INSTALL_PREFIX:PATH=${install_dir} \
  -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
  ${base_dir}
if [ $? -ne 0 ]; then
    echo "ERROR: Cannot configure !" >&2
    my_exit 1
fi

make
if [ $? -ne 0 ]; then
    echo "ERROR: Cannot compile !" >&2
    my_exit 1
fi

export ELECTRONICS_TESTING_DIR=${source_dir}/testing
echo "DEVEL: ELECTRONICS_TESTING_DIR=${ELECTRONICS_TESTING_DIR}" >&2
make test
if [ $? -ne 0 ]; then
    echo "ERROR: Some tests failed !" >&2
    my_exit 1
fi

my_exit 0

# end
