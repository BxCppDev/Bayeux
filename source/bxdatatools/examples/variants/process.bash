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

build_dir=$(pwd)/_build.d
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

echo "INFO: The example program ex_variants without variants..." 1>&2
./ex_variants
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants without variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants without variants is ok !" 1>&2
    # my_exit 0
fi

echo "INFO: The example program ex_variants with variants (1)..." 1>&2
./ex_variants --with-variants --variants-test 1
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants with variants (1) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants with variants (1) is ok !" 1>&2
    # my_exit 0
fi

echo "INFO: The example program ex_variants with variants (2)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variance.conf),
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - launch a GUI dialog box to enable the (re)configuration of some of the variant parameters,
#
./ex_variants --with-variants --variants-test 1 \
    --datatools::logging="fatal" \
    --datatools::variant-config="${FOO_CONFIG_DIR}/variants/foo_variance.conf" \
    --datatools::variant-set="core:gui=true" \
    --datatools::variant-set="core:operator=John Doe" \
    --datatools::variant-set="core:logging=warning" \
    --datatools::variant-set="algo:accuracy=4" \
    --datatools::variant-set="algo:max_time=3 s" \
    --datatools::variant-set="algo:nevents=5" \
    --datatools::variant-set="gui:antialiasing=false" \
    --datatools::variant-qt-gui
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants (2) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants (2) is ok !" 1>&2
    # my_exit 0
fi

echo "INFO: The example program ex_variants with variants (3)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variance.conf),
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - store the variant settings in a file
#
./ex_variants --with-variants --variants-test 1 \
    --datatools::logging="trace" \
    --datatools::variant-config="${FOO_CONFIG_DIR}/variants/foo_variance.conf" \
    --datatools::variant-set="core:gui=true" \
    --datatools::variant-set="core:operator=John Doe" \
    --datatools::variant-set="core:help=FullHelp" \
    --datatools::variant-set="core:help/if_full_help/url=https://foo.org/help2/" \
    --datatools::variant-set="core:plugin=true" \
    --datatools::variant-set="algo:accuracy=4" \
    --datatools::variant-set="algo:max_time=3 s" \
    --datatools::variant-set="algo:nevents=5" \
    --datatools::variant-set="gui:antialiasing=false" \
    --datatools::variant-set="gui:color=true" \
    --datatools::variant-set="gui:color/is_colored/palette=5" \
    --datatools::variant-set="gui:logging=warning" \
    --datatools::variant-store="my_profile.rep"
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants (3) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants (3) is ok !" 1>&2
    # my_exit 0
fi

echo "INFO: The example program ex_variants with variants (4)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variance.conf),
# - load variant parameters default values from a file (default.rep)
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - launch a GUI dialog box to enable the (re)configuration of some of the variant parameters,
# - save the variant parameters values in a file (my_profile.rep)
#
./ex_variants --with-variants --variants-test 1 \
    --datatools::logging="fatal" \
    --datatools::variant-config="${FOO_CONFIG_DIR}/variants/foo_variance.conf" \
    --datatools::variant-load="${FOO_CONFIG_DIR}/variants/profiles/default.rep" \
    --datatools::variant-set="core:gui=true" \
    --datatools::variant-set="core:operator=John Doe" \
    --datatools::variant-set="core:logging=warning" \
    --datatools::variant-set="algo:accuracy=4" \
    --datatools::variant-set="algo:max_time=3 s" \
    --datatools::variant-set="algo:nevents=5" \
    --datatools::variant-set="gui:antialiasing=false" \
    --datatools::variant-qt-gui \
    --datatools::variant-store="my_profile.rep"
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants (4) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants (4) is ok !" 1>&2
fi

rm -f my_profile.rep
rm -f ex_variants
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

my_exit 0

# end
