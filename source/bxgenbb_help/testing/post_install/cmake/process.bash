#!/usr/bin/env bash

opwd=$(pwd)

function my_exit ()
{
    cd ${opwd}
    exit $1
}

./clean.bash
mkdir __build
cd __build
echo "Configure..." 1>&2
cmake -DCMAKE_INSTALL_PREFIX:PATH=${opwd}/__install -DCMAKE_BUILD_TYPE:STRING="Release" ..
if [ $? -ne 0 ]; then
    echo "Configure failed ! Abort !" 1>&2
    my_exit 1
fi
echo "Build..." 1>&2
make
if [ $? -ne 0 ]; then
    echo "Build failed ! Abort !" 1>&2
    my_exit 1
fi
echo "Install..." 1>&2
make install
if [ $? -ne 0 ]; then
    echo "Install failed ! Abort !" 1>&2
    my_exit 1
fi

echo "Execute the 'test_genbb_help' executable..." 1>&2
${opwd}/__install/bin/test_genbb_help
if [ $? -ne 0 ]; then
    echo "Execute failed ! Abort !" 1>&2
    my_exit 1
fi

echo "Execute the 'test_genbb_mgr' executable..." 1>&2
${opwd}/__install/bin/test_genbb_mgr
if [ $? -ne 0 ]; then
    echo "Execute failed ! Abort !" 1>&2
    my_exit 1
fi

my_exit 0

# end
