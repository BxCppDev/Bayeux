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

echo "INFO: Example the program ex_variants without variants..." 1>&2
./ex_variants
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_variants without variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: Example program ex_variants without variants is ok !" 1>&2
    # my_exit 0
fi

echo "INFO: Example the program ex_variants with variants (1)..." 1>&2
./ex_variants --with-variants --variants-test 1
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_variants with variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: Example program ex_variants with variants (1) is ok !" 1>&2
    # my_exit 0
fi

echo "INFO: Example the program ex_variants with variants (2)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variance.conf),
# - load variant parameters default values from a file (default.rep)
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - launch a GUI dialog box to enable the (re)configuration of some of the variant parameters,
# - save the variant parameters values in a file (my_profile.rep)
#
./ex_variants --with-variants --variants-test 2 \
    --datatools::logging="fatal" \
    --datatools::variant-config="${FOO_CONFIG_DIR}/variants/foo_variance.conf" \
    --datatools::variant-load="${FOO_CONFIG_DIR}/variants/profiles/default.rep" \
    --datatools::variant-set="algo:accuracy=4" \
    --datatools::variant-set="algo:max_time=3 s" \
    --datatools::variant-set="algo:nevents=5" \
    --datatools::variant-set="core:gui=true" \
    --datatools::variant-set="core:operator=John Doe" \
    --datatools::variant-set="core:logging=warning" \
    --datatools::variant-set="gui:antialiasing=false" \
    --datatools::variant-gui \
    --datatools::variant-store="${FOO_CONFIG_DIR}/variants/profiles/my_profile.rep"
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex_variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: Example program ex_variants (2) is ok !" 1>&2
    # my_exit 0
fi

rm -f ex_variants
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

my_exit 0

# end
