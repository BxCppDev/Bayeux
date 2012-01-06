#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# mygsl-config
##

script_name="mygsl-config"

opwd=$(pwd)
help=0
debug=0

function my_exit ()
{
    cd ${opwd}
    exit $?
}

mygsl_system="@CMAKE_SYSTEM_NAME@-@CMAKE_SYSTEM_PROCESSOR@"

mygsl_root=@CMAKE_INSTALL_PREFIX@

if [ -n "${MYGSL_ROOT}" ]; then
    mygsl_root="${MYGSL_ROOT}"
else
    echo "ERROR: ${script_name}: 'MYGSL_ROOT' environment variable is not defined ! Abort !" 1>&2
    my_exit 1
fi

mygsl_include_dir="${mygsl_root}/include"

mygsl_lib_dir="${mygsl_root}/lib"

mygsl_bin_dir="${mygsl_root}/bin"

mygsl_resources_dir="@CMAKE_INSTALL_PREFIX@/@INSTALL_RESOURCES_DIR@"

mygsl_version=@mygsl_VERSION_STR@

option="$1"
shift 1
option2="$1"
shift 1

if [ "x${option}" = "x" ]; then
    echo "ERROR: ${script_name}: Missing option ! Abort !" 1>&2
    help=1
fi

if [ "x${option}" = "x--help" -o "x${option}" = "x-h" -o "x${option}" = "x-?" ]; then
    help=1
fi

if [ ${help} -eq 1 ]; then
    cat<<EOF
Usage: ${script_name} [OPTIONS]

Known values for OPTION are:

  --help                print this help then exit
  --version             print version information
  --system              print system
  --prefix              print mygsl installation prefix
  --incdir              print include directory
  --libdir              print library directory
  --bindir              print binary directory
  --resources-dir       print resources directory
  --include             print include path pre-processor flags without dependencies
  --cflags              print include path pre-processor flags with dependencies
  --libs                print library linking information, without dependencies
  --ldflags 
  --gsl-version         print GSL version
  --datatools-version   print datatools version

EOF
    my_exit 1
fi

if [ "x${option}" = "x--system" ]; then
    echo "${mygsl_system}"
    my_exit 0
fi

if [ "x${option}" = "x--prefix" ]; then
    echo "${mygsl_root}"
    my_exit 0
fi

if [ "x${option}" = "x--version" ]; then
    echo "${mygsl_version}"
    my_exit 0
fi

if [ "x${option}" = "x--include-dir" -o "x${option}" = "x--incdir" ]; then
    echo "${mygsl_include_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--resources-dir" ]; then
    echo "${mygsl_resources_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--include" ]; then
    echo "-I${mygsl_include_dir} "
    my_exit 0
fi

if [ "x${option}" = "x--cflags" ]; then
    (
	echo -n "-fPIC " 
	echo -n "-I${mygsl_include_dir} "  
	echo -n "`datatools-config --cflags` "
        echo -n "`gsl-config --cflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py 
    my_exit 0
fi

if [ "x${option}" = "x--libdir" ]; then
    echo "${mygsl_lib_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--bindir" ]; then
    echo "${mygsl_bin_dir}"
    my_exit 0
fi

# compute default linkage :
bio_ldflags=

if [ "x${option}" = "x--libs" ]; then
    echo -n "-L${mygsl_lib_dir} ${bio_ldflags} -lmygsl "
    echo
    my_exit 0
fi

if [ "x${option}" = "x--ldflags" ]; then
    (
	echo -n "-L${mygsl_lib_dir} ${bio_ldflags} -lmygsl "
	echo -n "`gsl-config --libs` "
	echo -n "`datatools-config --ldflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py -r
    my_exit 0
fi

if [ "x${option}" = "x--datatools-version" ]; then
    echo @DATATOOLS_VERSION@
    my_exit 0
fi

if [ "x${option}" = "x--gsl-version" ]; then
    echo `gsl-config --version` ###@GSL_VERSION@
    my_exit 0
fi

echo "ERROR: ${script_name}: Unknown option !" 1>&2
$0 --help
my_exit 1

# end of mygsl-config
