#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# genbb_help-config

script_name="genbb_help-config"

opwd=$(pwd)
help=0
debug=0
bio_support=1
python_wrapper_support=0

if [ ${python_wrapper_support} -eq 1 ]; then
    with_python_wrapper=0
    if [ "x@GENBB_HELP_WITH_PYTHON_WRAPPER@" == "x1" ]; then
	with_python_wrapper=1
    fi
fi

if [ ${bio_support} -eq 1 ]; then
    with_bio=0
    if [ "x@GENBB_HELP_WITH_BIO@" == "x1" ]; then
	with_bio=1
    fi
fi

function my_exit ()
{
    cd ${opwd}
    exit $?
}

genbb_help_system="@CMAKE_SYSTEM_NAME@-@CMAKE_SYSTEM_PROCESSOR@"

genbb_help_root=@CMAKE_INSTALL_PREFIX@

if [ -n "${GENBB_HELP_ROOT}" ]; then
    genbb_help_root="${GENBB_HELP_ROOT}"
else
    echo "ERROR: ${script_name}: 'GENBB_HELP_ROOT' environment variable is not defined ! Abort !" 1>&2
    my_exit 1
fi

genbb_help_include_dir="${genbb_help_root}/include"

genbb_help_lib_dir="${genbb_help_root}/lib"

genbb_help_bin_dir="${genbb_help_root}/bin"

genbb_help_resources_dir="@CMAKE_INSTALL_PREFIX@/@INSTALL_RESOURCES_DIR@"

genbb_help_version=@genbb_help_VERSION_STR@

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
  --prefix              print genbb_help installation prefix
  --incdir              print include directory
  --libdir              print library directory
  --bindir              print binary directory
  --resources-dir       print resources directory
  --include             print include path pre-processor flags without dependencies
  --cflags              print include path pre-processor flags with dependencies
  --libs    [option]    print library linking information, without dependencies
  --ldflags [option]    print library linking information, with dependencies
     Options: --with-bio     with linkage against the 'libgenbb_help_bio.so' DLL (default)
              --without-bio  without linkage against the 'libgenbb_help_bio.so' DLL
              --only-fortran only with the Fortran DLL (Decay0) 
  --with-python-wrapper print if Python wrapper module is used
  --with-bio            print if Boost/Serialization binary code is pre-built 
  --geomtools-version   print geomtools version
EOF
    my_exit 1
fi

if [ "x${option}" = "x--system" ]; then
    echo "${genbb_help_system}"
    my_exit 0
fi

if [ "x${option}" = "x--prefix" ]; then
    echo "${genbb_help_root}"
    my_exit 0
fi

if [ "x${option}" = "x--version" ]; then
    echo "${genbb_help_version}"
    my_exit 0
fi

if [ "x${option}" = "x--include-dir" -o "x${option}" = "x--incdir" ]; then
    echo "${genbb_help_include_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--resources-dir" ]; then
    echo "${genbb_help_resources_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--include" ]; then
    echo "-I${genbb_help_include_dir} "
    my_exit 0
fi

if [ "x${option}" = "x--cflags" ]; then
    only_fortran=0
    if [ "x${option2}" = "x--only-fortran" ]; then
    	only_fortran=1
    fi
    (
	echo -n "-fPIC " 
	echo -n "-I${genbb_help_include_dir} "  
	if [ ${only_fortran} -eq 1 ]; then
 	    echo -n "`geomtools-config --cflags` "  
	fi 
 	echo -n "`mygsl-config --cflags` "  
	echo ""
    ) | python @CMAKE_INSTALL_PREFIX@/@INSTALL_MISC_DIR@/pkgtools/mkuniqueflags.py 
    my_exit 0
fi

if [ "x${option}" = "x--libdir" ]; then
    echo "${genbb_help_lib_dir}"
    my_exit 0
fi

if [ "x${option}" = "x--bindir" ]; then
    echo "${genbb_help_bin_dir}"
    my_exit 0
fi

more_ldflags=
# compute default linkage :
if [ ${bio_support} -eq 1 ]; then
  bio_ldflags=
  if [ ${with_bio} -eq 1 ]; then
      bio_ldflags=-lgenbb_help_bio
  fi
fi
 
if [ "x${option}" = "x--libs" ]; then
    only_fortran=0
    if [ ${bio_support} -eq 1 ]; then
	if [ "x${option2}" = "x--without-bio" ]; then
    	    bio_ldflags=
	elif [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
    	    bio_ldflags=-lgenbb_help_bio
	elif [ "x${option2}" = "x--only-fortran" ]; then
    	    only_fortran=1
	fi
	more_ldflags="${more_ldflags} ${bio_ldflags} "
    fi
    (
	echo -n "-L${genbb_help_lib_dir} "
	if [ ${only_fortran} -eq 0 ]; then
	    echo -n "${more_ldflags} -lgenbb_help "
	fi
	echo -n "-lgenbb_help_f77 "
	echo ""
    )
    my_exit 0
fi

if [ "x${option}" = "x--ldflags" ]; then
    only_fortran=0
    if [ ${bio_support} -eq 1 ]; then
	if [ "x${option2}"   = "x--without-bio" ]; then
    	    bio_ldflags=
	elif [ "x${option2}" = "x--with-bio" -a ${with_bio} -eq 1 ]; then
    	    bio_ldflags=-lgenbb_help_bio
	elif [ "x${option2}" = "x--only-fortran" ]; then
    	    only_fortran=1
	fi
	more_ldflags="${more_ldflags} ${bio_ldflags} "
    fi
    (
	echo -n "-L${genbb_help_lib_dir} "
	if [ ${only_fortran} -eq 0 ]; then
	    echo -n "${more_ldflags} -lgenbb_help "
	    echo -n "`geomtools-config --ldflags` "
	fi
	echo -n "-lgenbb_help_f77 "
	echo -n "`mygsl-config --ldflags` "
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

# end of genbb_help-config
