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
    -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix 2> /dev/null) \
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

export FOO_CONFIG_DIR=./config
export FOO_VAR_DIR=./var

test -d ${FOO_VAR_DIR} && rm -fr ${FOO_VAR_DIR}
mkdir ${FOO_VAR_DIR}

echo "INFO: Example the program ex_variants without variants..." 1>&2
./ex_variants
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_variants without variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: Example program ex_variants without variants is ok !" 1>&2
    # my_exit 0
fi

echo "INFO: Example the program ex_variants with variants..." 1>&2
./ex_variants --with-variants
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_variants with variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: Example program ex_variants with variants is ok !" 1>&2
    my_exit 0
fi

echo "INFO: Example the program ex_variants with variants..." 1>&2
# Ask the datatools kernel to setup the variant repository in its
# system repository, then launch the GUI to (re)configure some of
# the variant parameters, then save the variants in a file:
./ex_variants \
    --datatools::logging="trace" \
    --datatools::variant-config="${FOO_CONFIG_DIR}/foo_variance.conf" \
    --datatools::variant-gui \
    --datatools::variant-store="${FOO_VAR_DIR}/foo_variants.rep"
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: Example program ex_variants is ok !" 1>&2
    # my_exit 0
fi

    # --datatools::variant-registry-config="${FOO_CONFIG_DIR}/foo_core_registry.conf" \
    #--datatools::variant-registry-config="${FOO_CONFIG_DIR}/foo_gui_registry.conf" \ #

if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_variants failed !" 1>&2
    my_exit 1
fi

#rm -f ex_variants
rm -fr ${FOO_VAR_DIR}
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

my_exit 0

# end
