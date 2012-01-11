#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# genvtx-config
##

script_name="genvtx-config"

opwd=$(pwd)
help=0
debug=0
bio_support=0
python_wrapper_support=0

if [ ${python_wrapper_support} -eq 1 ]; then
    with_python_wrapper=0
    if [ "x@GENVTX_WITH_PYTHON_WRAPPER@" == "xON" ]; then
	with_python_wrapper=1
    fi
fi

if [ ${bio_support} -eq 1 ]; then
    with_bio=0
    if [ "x@GENVTX_WITH_BIO@" == "xON" ]; then
	with_bio=1
    fi
fi

function my_exit ()
{
    cd ${opwd}
    exit $?
}

genvtx_system="@CMAKE_SYSTEM_NAME@-@CMAKE_SYSTEM_PROCESSOR@"

genvtx_root=@CMAKE_INSTALL_PREFIX@

if [ -n "${GENVTX_ROOT}" ]; then
    genvtx_root="${GENVTX_ROOT}"
else
    echo "ERROR: ${script_name}: 'GENVTX_ROOT' environment variable is not defined ! Abort !" 1>&2
    my_exit 1
fi

genvtx_include_dir="${genvtx_root}/include"

genvtx_lib_dir="${genvtx_root}/lib"

genvtx_bin_dir="${genvtx_root}/bin"

genvtx_resources_dir="@CMAKE_INSTALL_PREFIX@/@INSTALL_RESOURCES_DIR@"

genvtx_version=@genvtx_VERSION_STR@

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
  --prefix              print genvtx installation prefix
  --incdir              print include directory
  --libdir              print library directory
  --bindir              print binary directory
  --resources-dir       print resources directory
  --include             print include path pre-processor flags without dependencies
  --cflags              print include path pre-processor flags with dependencies
  --libs                print library linking information, without dependencies
  --ldflags             print library linking information, with dependencies
  --geomtools-version   print geomtools version
EOF
    my_exit 1
fi

if [ "x${option}" = "x--system" ]; then
    echo "${genvtx_system}"
    my_exit 0
fi

if [ "x${option}" = "x--prefix" ]; then
    echo "${genvtx_root}"
    my_exit 0
fi

if [ "x${option}" = "x--version" ]; then
    echo "${genvtx_version}"
    my_exit 0
fi

if [ "x${option}" = "x--include-dir" -o "x${option}" = "x--incdir" ]; then
    echo "${genvtx_include_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--resources-dir" ]; then
    echo "${genvtx_resources_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--include" ]; then
    echo "-I${genvtx_include_dir} "
    my_exit 0
fi

if [ "x${option}" = "x--cflags" ]; then
    (
	echo -n "-fPIC " 
	echo -n "-I${genvtx_include_dir} "  
	echo -n "`geomtools-config --cflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py 
    my_exit 0
fi

if [ "x${option}" = "x--libdir" ]; then
    echo "${genvtx_lib_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--bindir" ]; then
    echo "${genvtx_bin_dir}"
    my_exit 0
fi

more_ldflags=
# compute default linkage :
if [ ${bio_support} -eq 1 ]; then
  bio_ldflags=
  if [ ${with_bio} -eq 1 ]; then
      bio_ldflags=-lgenvtx_bio
  fi
fi
 
if [ "x${option}" = "x--libs" ]; then
    if [ ${bio_support} -eq 1 ]; then
      if [ "x${option2}" = "x--without-bio" ]; then
     	bio_ldflags=
      fi
      if [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
     	bio_ldflags=-lgenvtx_bio
      fi
      more_ldflags="${more_ldflags} ${bio_ldflags} "
    fi
    echo -n "-L${genvtx_lib_dir} ${more_ldflags} -lgenvtx "
    echo
    my_exit 0
fi

if [ "x${option}" = "x--ldflags" ]; then
    if [ ${bio_support} -eq 1 ]; then
	if [ "x${option2}" = "x--without-bio" ]; then
    	    bio_ldflags=
	fi
	if [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
    	bio_ldflags=-lgeomtools_bio
	fi
	more_ldflags="${more_ldflags} ${bio_ldflags} "
    fi
    (
	echo -n "-L${genvtx_lib_dir} ${more_ldflags} -lgenvtx "
	echo -n "`geomtools-config --ldflags` "
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py -r
    my_exit 0
fi

if [ ${bio_support} -eq 1 ]; then
    if [ "x${option}" = "x--with-bio" ]; then
	echo "${with_bio}"
	my_exit 0
    fi
fi

if [ ${python_wrapper_support} -eq 1 ]; then
    if [ "x${option}" = "x--with-python-wrapper" ]; then
	echo "${with_python_wrapper}"
	my_exit 0
    fi
fi

if [ "x${option}" = "x--geomtools-version" ]; then
    echo @GEOMTOOLS_VERSION@
    my_exit 0
fi

echo "ERROR: ${script_name}: Unknown option !" 1>&2
$0 --help
my_exit 1

# end of genvtx-config
