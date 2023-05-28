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

export FOO_CONFIG_DIR=./config

echo 1>&2
echo 1>&2
echo 1>&2
echo 1>&2
echo "INFO: The example program ex_variants without variants..." 1>&2
${install_dir}/ex_variants
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants without variants failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants without variants is ok !" 1>&2
fi

echo 1>&2
echo 1>&2
echo 1>&2
echo 1>&2
echo "INFO: The example program ex_variants with variants (1)..." 1>&2
${install_dir}/ex_variants --with-variants --variants-test 1
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants with variants (1) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants with variants (1) is ok !" 1>&2
fi

echo 1>&2
echo 1>&2
echo 1>&2
echo 1>&2
echo "INFO: The example program ex_variants with variants (2)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variants.conf),
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - launch a GUI dialog box to enable the (re)configuration of some of the variant parameters,
${install_dir}/ex_variants --with-variants --variants-test 1 \
    --variant-set="core:gui=true" \
    --variant-set="core:operator=John Doe" \
    --variant-set="core:logging=warning" \
    --variant-set="algo:accuracy=4" \
    --variant-set="algo:max_time=3 s" \
    --variant-set="algo:nevents=5" \
    --variant-set="gui:antialiasing=false" \
    --variant-gui
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants (2) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants (2) is ok !" 1>&2
fi

echo 1>&2
echo 1>&2
echo 1>&2
echo 1>&2
echo "INFO: The example program ex_variants with variants (3)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variants.conf),
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - store the variant settings in a file
${install_dir}/ex_variants --with-variants --variants-test 1 \
    --variant-set="core:gui=true" \
    --variant-set="core:operator=John Doe" \
    --variant-set="core:help=FullHelp" \
    --variant-set="core:help/if_full_help/url=https://foo.org/help2/" \
    --variant-set="core:plugin=true" \
    --variant-set="algo:accuracy=4" \
    --variant-set="algo:max_time=3 s" \
    --variant-set="algo:nevents=5" \
    --variant-set="gui:antialiasing=false" \
    --variant-set="gui:color=true" \
    --variant-set="gui:color/is_colored/palette=5" \
    --variant-set="gui:logging=warning"
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants (3) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants (3) is ok !" 1>&2
fi

echo 1>&2
echo 1>&2
echo 1>&2
echo 1>&2
echo "INFO: The example program ex_variants with variants (4)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variants.conf),
# - load variant parameters default values from a file (default.rep)
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - launch a GUI dialog box to enable the (re)configuration of some of the variant parameters,
# - save the variant parameters values in a file (my_profile.rep)
${install_dir}/ex_variants --with-variants --variants-test 1 \
    --variant-load="${FOO_CONFIG_DIR}/variants/profiles/default.rep" \
    --variant-set="core:gui=true" \
    --variant-set="core:operator=John Doe" \
    --variant-set="core:logging=warning" \
    --variant-set="algo:accuracy=4" \
    --variant-set="algo:max_time=3 s" \
    --variant-set="algo:nevents=5" \
    --variant-set="gui:antialiasing=false" \
    --variant-gui \
    --variant-store="my_profile.rep"
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants (4) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants (4) is ok !" 1>&2
fi

echo 1>&2
echo 1>&2
echo 1>&2
echo 1>&2
echo "INFO: The example program ex_variants with variants (4)..." 1>&2
# Ask the datatools kernel to :
# - setup its system repository from a variant repository
#   configuration file (foo_variants.conf),
# - load variant parameters default values from a file (default.rep)
# - set some of the variant parameters from the command line ("algo:accuracy=4"...)
# - launch a GUI dialog box to enable the (re)configuration of some of the variant parameters,
# - save the variant parameters values in a file (my_profile.rep)
${install_dir}/ex_variants --with-variants --variants-test 2 \
    --variant-load="${FOO_CONFIG_DIR}/variants/profiles/default.rep" \
    --variant-set="core:gui=true" \
    --variant-set="core:operator=John Doe" \
    --variant-set="core:logging=warning" \
    --variant-set="algo:accuracy=4" \
    --variant-set="algo:max_time=3 s" \
    --variant-set="algo:nevents=5" \
    --variant-set="gui:antialiasing=false" \
    --variant-gui \
    --variant-store="my_profile.rep"
if [ $? -ne 0 ]; then
    echo "ERROR: The example program ex_variants (5) failed !" 1>&2
    my_exit 1
else
    echo "INFO: The example program ex_variants (5) is ok !" 1>&2
fi

rm -f my_profile.rep
rm -fr ${install_dir}
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

my_exit 0

# end
