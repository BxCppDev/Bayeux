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

export CONFIG_DIR="$(pwd)/config"

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
ls -l

#pandoc -r rst -w html README.rst -o geomtools-ex01_README.html
geomtools_inspector_bin="bxgeomtools_inspector"

echo "Run the 'ex01' program..." 1>&2
${install_dir}/ex01
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex01 failed !" 1>&2
    my_exit 1
fi

echo "Browse the geometry setup via the '${geomtools_inspector_bin}' program..." 1>&2
${geomtools_inspector_bin} \
   --manager-config config/manager.conf \
   --with-visu --visu-view-3d

rm -f ./geomtools-ex01.C
rm -f ./geomtools-ex01.gdml
rm -f ./geomtools-ex01_README.html
rm -fr ${build_dir}
rm -fr ${install_dir}
find . -name "*~" -exec rm -f \{\} \;

cd ${opwd}

my_exit 0

# end
