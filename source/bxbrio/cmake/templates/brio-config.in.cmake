#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# brio-config
##

script_name="brio-config"

opwd=$(pwd)
help=0
debug=0

function my_exit ()
{
    cd ${opwd}
    exit $?
}

brio_system="@CMAKE_SYSTEM_NAME@-@CMAKE_SYSTEM_PROCESSOR@"

brio_root=@CMAKE_INSTALL_PREFIX@

if [ -n "${BRIO_ROOT}" ]; then
    brio_root="${BRIO_ROOT}"
else
    echo "ERROR: ${script_name}: 'BRIO_ROOT' environment variable is not defined ! Abort !" 1>&2
    my_exit 1
fi

brio_include_dir="${brio_root}/include"

brio_lib_dir="${brio_root}/lib"

brio_bin_dir="${brio_root}/bin"

brio_resources_dir="@CMAKE_INSTALL_PREFIX@/@INSTALL_RESOURCES_DIR@"

brio_version=@brio_VERSION_STR@

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
  --prefix              print brio installation prefix
  --incdir              print include directory
  --libdir              print library directory
  --bindir              print binary directory
  --resources-dir       print resources directory
  --include             print include path pre-processor flags without dependencies
  --cflags              print include path pre-processor flags with dependencies
  --libs                print library linking information, without dependencies
  --ldflags 
  --root-version        print ROOT version
  --datatools-version   print datatools version

EOF
    my_exit 1
fi

if [ "x${option}" = "x--system" ]; then
    echo "${brio_system}"
    my_exit 0
fi

if [ "x${option}" = "x--prefix" ]; then
    echo "${brio_root}"
    my_exit 0
fi

if [ "x${option}" = "x--version" ]; then
    echo "${brio_version}"
    my_exit 0
fi

if [ "x${option}" = "x--include-dir" -o "x${option}" = "x--incdir" ]; then
    echo "${brio_include_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--resources-dir" ]; then
    echo "${brio_resources_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--include" ]; then
    echo "-I${brio_include_dir} "
    my_exit 0
fi

if [ "x${option}" = "x--cflags" ]; then
    (
	echo -n "-fPIC " 
	echo -n "-I${brio_include_dir} "  
	echo -n "`datatools-config --cflags` "
        echo -n "`root-config --cflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py 
    my_exit 0
fi

if [ "x${option}" = "x--libdir" ]; then
    echo "${brio_lib_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--bindir" ]; then
    echo "${brio_bin_dir}"
    my_exit 0
fi

# compute default linkage :
bio_ldflags=

if [ "x${option}" = "x--libs" ]; then
    echo -n "-L${brio_lib_dir} ${bio_ldflags} -lbrio "
    echo
    my_exit 0
fi

if [ "x${option}" = "x--ldflags" ]; then
    (
	echo -n "-L${brio_lib_dir} ${bio_ldflags} -lbrio "
	echo -n "`root-config --libs` "
	echo -n "`datatools-config --ldflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py -r
    my_exit 0
fi

if [ "x${option}" = "x--datatools-version" ]; then
    echo @DATATOOLS_VERSION@
    my_exit 0
fi

if [ "x${option}" = "x--root-version" ]; then
    echo @ROOTVERSION@
    my_exit 0
fi

echo "ERROR: ${script_name}: Unknown option !" 1>&2
$0 --help
my_exit 1

# end of brio-config
