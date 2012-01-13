#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# datatools-config
##

script_name="datatools-config"

opwd=$(pwd)
help=0
debug=0

function my_exit ()
{
    cd ${opwd}
    exit $?
}

datatools_system="@CMAKE_SYSTEM_NAME@-@CMAKE_SYSTEM_PROCESSOR@"

datatools_root=@CMAKE_INSTALL_PREFIX@

if [ -n "${DATATOOLS_ROOT}" ]; then
    datatools_root="${DATATOOLS_ROOT}"
else
    echo "ERROR: ${script_name}: 'DATATOOLS_ROOT' environment variable is not defined ! Abort !" 1>&2
    my_exit 1
fi

datatools_include_dir="${datatools_root}/include"

datatools_lib_dir="${datatools_root}/lib"

datatools_bin_dir="${datatools_root}/bin"

datatools_resources_dir="@CMAKE_INSTALL_PREFIX@/@INSTALL_RESOURCES_DIR@"

datatools_version=@datatools_VERSION_STR@

with_python_wrapper=0
if [ "x@DATATOOLS_WITH_PYTHON_WRAPPER@" == "x1" ]; then
    with_python_wrapper=1
fi

with_bio=0
if [ "x@DATATOOLS_WITH_BIO@" == "x1" ]; then
    with_bio=1
fi

with_embedded_kwsys=0
if [ "x@DATATOOLS_WITH_EMBEDDED_KWSYS@" == "x1" ]; then
    with_embedded_kwsys=1
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
  --prefix              print datatools installation prefix
  --incdir              print include directory
  --libdir              print library directory
  --bindir              print binary directory
  --resources-dir       print resources directory
  --include             print include path pre-processor flags without dependencies
  --cflags              print include path pre-processor flags with dependencies
  --libs    [option]    print library linking information, without dependencies
  --ldflags [option]
      Options: --with-bio     with linkage against the 'libdatatools_bio.so' DLL (default)
               --without-bio  without linkage against the 'libdatatools_bio.so' DLL
  --with-python-wrapper print if Python wrapper module is used
  --with-bio            print if Boost/Serialization binary code is pre-built 
                        in an auxiliary I/O shared library
  --with-embedded-kwsys print if Kwsys is embedded in datatools
  --boost-version       print Boost version
  --clhep-version       print CLHEP version

EOF
    my_exit 1
fi

if [ "x${option}" = "x--system" ]; then
    echo "${datatools_system}"
    my_exit 0
fi

if [ "x${option}" = "x--prefix" ]; then
    echo "${datatools_root}"
    my_exit 0
fi

if [ "x${option}" = "x--version" ]; then
    echo "${datatools_version}"
    my_exit 0
fi

if [ "x${option}" = "x--include-dir" -o "x${option}" = "x--incdir" ]; then
    echo "${datatools_include_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--resources-dir" ]; then
    echo "${datatools_resources_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--include" ]; then
    echo "-I${datatools_include_dir} "
    my_exit 0
fi

if [ "x${option}" = "x--cflags" ]; then
    (
	echo -n "-fPIC " 
	echo -n "-I${datatools_include_dir} "  
	#echo -n "`boost-config --cflags date_time serialization iostreams filesystem system` "
	echo -n "`boost-config --cflags` "
        echo -n "`clhep-config --include` "
	if [ ${with_embedded_kwsys} -eq 0 ]; then
	    echo -n "`kwsys-config --cflags` "
	fi
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py 

    ###) | sed -e "s/-Wstrict-prototypes//g" -e "s/-Wall//g" | python ${datatools_root}/pkgtools.d/mkuniqueflags.py
    my_exit 0
fi

if [ "x${option}" = "x--libdir" ]; then
    echo "${datatools_lib_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--bindir" ]; then
    echo "${datatools_bin_dir}"
    my_exit 0
fi

# compute default linkage :
bio_ldflags=
if [ ${with_bio} -eq 1 ]; then
    bio_ldflags=-ldatatools_bio
fi

if [ "x${option}" = "x--libs" ]; then
    if [ "x${option2}" = "x--without-bio" ]; then
	bio_ldflags=
    fi
    if [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
	bio_ldflags=-ldatatools_bio
    fi
    echo -n "-L${datatools_lib_dir} ${bio_ldflags} -ldatatools "
    if [ ${with_embedded_kwsys} -eq 1 ]; then
	echo -n "-lkwsys -lkwsys_c -ldl "
    fi
    echo
    my_exit 0
fi

if [ "x${option}" = "x--ldflags" ]; then
    if [ "x${option2}" = "x--without-bio" ]; then
	bio_ldflags=
    fi
    if [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
	bio_ldflags=-ldatatools_bio
    fi
    (
	echo -n "-L${datatools_lib_dir} ${bio_ldflags} -ldatatools "
	#echo -n "`boost-config --ldflags serialization iostreams filesystem system` "
	echo -n "`boost-config --ldflags` "
	echo -n "`clhep-config --libs` -lm "
	if [ ${with_embedded_kwsys} -eq 0 ]; then
	    echo -n "`kwsys-config --ldflags` -ldl "
	else
	    echo -n "-lkwsys -lkwsys_c -ldl "
	fi
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py -r
    my_exit 0
fi

if [ "x${option}" = "x--with-bio" ]; then
    echo "${with_bio}"
    my_exit 0
fi

if [ "x${option}" = "x--with-embedded-kwsys" ]; then
    echo "${with_embedded_kwsys}"
    my_exit 0
fi

if [ "x${option}" = "x--with-python-wrapper" ]; then
    echo "${with_python_wrapper}"
    my_exit 0
fi

if [ "x${option}" = "x--boost-version" ]; then
    echo @Boost_VERSION@
    my_exit 0
fi

if [ "x${option}" = "x--clhep-version" ]; then
    echo @CLHEP_VERSION@
    my_exit 0
fi

echo "ERROR: ${script_name}: Unknown option !" 1>&2
$0 --help
my_exit 1

# end of datatools-config
