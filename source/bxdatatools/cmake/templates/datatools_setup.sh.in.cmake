# -*- mode : shell-script; -*-
# datatools_setup.sh

function do_datatools_setup ()
{
    echo "NOTICE: do_datatools_setup: Setup datatools..." 1>&2

    if [ "x${DATATOOLS_ROOT}" != "x" ]; then
	echo "WARNING: do_datatools_setup: datatools is already setup !" 1>&2
	return 2
    fi 
 
    which clhep-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_datatools_setup: Cannot find CLHEP setup ! Abort !" 1>&2
	return 1
    fi

    which boost-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_datatools_setup: Cannot find Boost setup ! Abort !" 1>&2
	return 1
    fi
    
    export DATATOOLS_ROOT="@CMAKE_INSTALL_PREFIX@"
    export DATATOOLS_ROOT_DIR="@CMAKE_INSTALL_PREFIX@"
    export DATATOOLS_NAME="@datatools_PACKAGE_NAME@"
    export DATATOOLS_VERSION="@datatools_VERSION_STR@"
    export DATATOOLS_BIN_DIR="@CMAKE_INSTALL_PREFIX@/bin"
    export DATATOOLS_ETC_DIR="@CMAKE_INSTALL_PREFIX@/etc"
    export DATATOOLS_LIB_DIR="@CMAKE_INSTALL_PREFIX@/lib"
    export DATATOOLS_INCLUDE_DIR="@CMAKE_INSTALL_PREFIX@/include"
    export DATATOOLS_RESOURCES_DIR="@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources"

    if [ -n "${PATH}" ]; then
	export PATH="${DATATOOLS_BIN_DIR}:${PATH}"
    else
	export PATH="${DATATOOLS_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${DATATOOLS_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${DATATOOLS_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${DATATOOLS_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${DATATOOLS_LIB_DIR}"
	fi 
    fi

    if [ "x@DATATOOLS_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@DATATOOLS_WITH_PYTHON_WRAPPER@" = "xON" ]; then
     	# update python_path...
	if [ -n "${PYTHONPATH}" ]; then 
            export PYTHONPATH="@CMAKE_INSTALL_PREFIX@/lib/datatools/python:${PYTHONPATH}"
	else
            export PYTHONPATH="@CMAKE_INSTALL_PREFIX@/lib/datatools/python"
	fi
    fi

    if [ "x@DATATOOLS_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@"
	fi
    fi
   
    echo "NOTICE: do_datatools_setup: Setup datatools done." 1>&2
    return 0
}
typeset -fx do_datatools_setup
alias datatools_setup='do_datatools_setup'

echo "NOTICE: do_datatools_setup: Please setup 'datatools' with 'do_datatools_setup'." 1>&2

# end of datatools_setup.sh