#!/usr/bin/env bash

clean=1

while [ -n "$1" ]; do
    token="$1"
    if [ "x${token}" = "x-C" ]; then
	clean=0
    fi
    shift 1
done

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

tree ${install_dir}

echo "Run the example program : " 1>&2
LD_LIBRARY_PATH=${install_dir}/lib:${LD_LIBRARY_PATH} ${install_dir}/ex_OCD -v -d
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_OCD failed !" 1>&2
    my_exit 1
else
    echo "INFO: Example program ex_OCD passed!" 1>&2
fi

# Special debug printing:
# export DATATOOLS_OCD_DEVEL_LOGGING="trace"

LD_LIBRARY_PATH=${install_dir}/lib:${LD_LIBRARY_PATH} \
    bxocd_manual --load-dll "datatools_ex_OCD" \
    --class-id "foo"            \
    --action show > foo_ocd.rst
if [ $? -ne 0 ]; then
    echo "ERROR: bxocd_manual failed !" 1>&2
    my_exit 1
else
    echo "INFO: bxocd_manual passed!" 1>&2
fi

which pandoc > /dev/null 2>&1
if [ $? -eq 0 ]; then
    pandoc -r rst -w html foo_ocd.rst > foo_ocd.html
    which xdg-open > /dev/null 2>&1
    if [ $? -eq 0 ]; then
	xdg-open file://$(pwd)/foo_ocd.html &
	sleep 3
    else
	echo "WARNING: Could not find xdg-open!" 1>&2
    fi
else
    echo "WARNING: Could not find pandoc!" 1>&2
fi

if [ ${clean} -eq 1 ]; then
    rm -f foo_ocd.html
    rm -f foo_ocd.rst
    rm -f ex_OCD.foo.doc.rst
    rm -f ex_OCD.foo.skeleton.conf
    rm -f ex_OCD.service_manager.doc.rst
    rm -f ex_OCD.service_manager.skeleton.conf
    rm -fr ${build_dir}
    rm -fr ${install_dir}
fi

my_exit 0

# end
