#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# geomtools-config
##

script_name="geomtools-config"

opwd=$(pwd)
help=0
debug=0

function my_exit ()
{
    cd ${opwd}
    exit $?
}

geomtools_system="@CMAKE_SYSTEM_NAME@-@CMAKE_SYSTEM_PROCESSOR@"

geomtools_root=@CMAKE_INSTALL_PREFIX@

if [ -n "${GEOMTOOLS_ROOT}" ]; then
    geomtools_root="${GEOMTOOLS_ROOT}"
else
    echo "ERROR: ${script_name}: 'GEOMTOOLS_ROOT' environment variable is not defined ! Abort !" 1>&2
    my_exit 1
fi

geomtools_include_dir="${geomtools_root}/include"

geomtools_lib_dir="${geomtools_root}/lib"

geomtools_bin_dir="${geomtools_root}/bin"

geomtools_resources_dir="@CMAKE_INSTALL_PREFIX@/@INSTALL_RESOURCES_DIR@"

geomtools_version=@geomtools_VERSION_STR@

with_python_wrapper=0
if [ "x@GEOMTOOLS_WITH_PYTHON_WRAPPER@" == "xON" ]; then
    with_python_wrapper=1
fi

with_bio=0
if [ "x@GEOMTOOLS_WITH_BIO@" == "xON" ]; then
    with_bio=1
fi

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
  --prefix              print geomtools installation prefix
  --incdir              print include directory
  --libdir              print library directory
  --bindir              print binary directory
  --resources-dir       print resources directory
  --include             print include path pre-processor flags without dependencies
  --cflags              print include path pre-processor flags with dependencies
  --libs    [option]    print library linking information, without dependencies
  --ldflags [option]
      Options: --with-bio     with linkage against the 'libgeomtools_bio.so' DLL (default)
               --without-bio  without linkage against the 'libgeomtools_bio.so' DLL
  --with-python-wrapper print if Python wrapper module is used
  --with-bio            print if Boost/Serialization binary code is pre-built 
                        in an auxiliary I/O shared library
  --mygsl-version       print mygsl version
  --datatools-version   print datatools version

EOF
    my_exit 1
fi

if [ "x${option}" = "x--system" ]; then
    echo "${geomtools_system}"
    my_exit 0
fi

if [ "x${option}" = "x--prefix" ]; then
    echo "${geomtools_root}"
    my_exit 0
fi

if [ "x${option}" = "x--version" ]; then
    echo "${geomtools_version}"
    my_exit 0
fi

if [ "x${option}" = "x--include-dir" -o "x${option}" = "x--incdir" ]; then
    echo "${geomtools_include_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--resources-dir" ]; then
    echo "${geomtools_resources_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--include" ]; then
    echo "-I${geomtools_include_dir} "
    my_exit 0
fi

if [ "x${option}" = "x--cflags" ]; then
    (
	echo -n "-fPIC " 
	echo -n "-I${geomtools_include_dir} "  
        echo -n "`mygsl-config --cflags` "
	echo -n "`datatools-config --cflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py 
    my_exit 0
fi

if [ "x${option}" = "x--libdir" ]; then
    echo "${geomtools_lib_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--bindir" ]; then
    echo "${geomtools_bin_dir}"
    my_exit 0
fi

# compute default linkage :
bio_ldflags=
if [ ${with_bio} -eq 1 ]; then
    bio_ldflags=-lgeomtools_bio
fi

if [ "x${option}" = "x--libs" ]; then
    if [ "x${option2}" = "x--without-bio" ]; then
	bio_ldflags=
    fi
    if [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
	bio_ldflags=-lgeomtools_bio
    fi
    echo -n "-L${geomtools_lib_dir} ${bio_ldflags} -lgeomtools "
    echo
    my_exit 0
fi

if [ "x${option}" = "x--ldflags" ]; then
    if [ "x${option2}" = "x--without-bio" ]; then
	bio_ldflags=
    fi
    if [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
	bio_ldflags=-lgeomtools_bio
    fi
    (
	echo -n "-L${geomtools_lib_dir} ${bio_ldflags} -lgeomtools "
	echo -n "`mygsl-config --ldflags` "
	echo -n "`datatools-config --ldflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py -r
    my_exit 0
fi

if [ "x${option}" = "x--with-bio" ]; then
    echo "${with_bio}"
    my_exit 0
fi

if [ "x${option}" = "x--with-python-wrapper" ]; then
    echo "${with_python_wrapper}"
    my_exit 0
fi

if [ "x${option}" = "x--datatools-version" ]; then
    echo @DATATOOLS_VERSION@
    my_exit 0
fi

if [ "x${option}" = "x--mygsl-version" ]; then
    echo @MYGSL_VERSION@
    my_exit 0
fi

echo "ERROR: ${script_name}: Unknown option !" 1>&2
$0 --help
my_exit 1

# end of geomtools-config
