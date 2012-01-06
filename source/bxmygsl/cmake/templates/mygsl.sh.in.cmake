# -*- mode : shell-script; -*-
# mygsl_setup.sh

function do_mygsl_setup ()
{
    echo "NOTICE: do_mygsl_setup: Setup mygsl..." 1>&2

    if [ "x${MYGSL_ROOT}" != "x" ]; then
	echo "WARNING: do_mygsl_setup: mygsl is already setup !" 1>&2
	return 2
    fi 
 
    which datatools-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_mygsl_setup: Cannot find datatools setup ! Abort !" 1>&2
	return 1
    fi

    which gsl-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_mygsl_setup: Cannot find GSL setup ! Abort !" 1>&2
	return 1
    fi
    
    export MYGSL_ROOT=@CMAKE_INSTALL_PREFIX@
    export MYGSL_ROOT_DIR=@CMAKE_INSTALL_PREFIX@
    export MYGSL_NAME=@mygsl_PACKAGE_NAME@
    export MYGSL_VERSION=@mygsl_VERSION@
    export MYGSL_BIN_DIR=@CMAKE_BIN_DIR@
    export MYGSL_ETC_DIR=@CMAKE_INSTALL_PREFIX@/etc
    export MYGSL_LIB_DIR=@CMAKE_LIB_DIR@
    export MYGSL_INCLUDE_DIR=@CMAKE_INSTALL_PREFIX@/include
    export MYGSL_RESOURCES_DIR=@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources

    if [ -n "${PATH}" ]; then
	export PATH="${MYGSL_BIN_DIR}:${PATH}"
    else
	export PATH="${MYGSL_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${MYGSL_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${MYGSL_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${MYGSL_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${MYGSL_LIB_DIR}"
	fi 
    fi

    if [ "x@MYGSL_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@MYGSL_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@_find_module_install_dir@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@_find_module_install_dir@"
	fi
    fi

    echo "NOTICE: do_mygsl_setup: Setup mygsl done." 1>&2
    return 0
}
typeset -fx do_mygsl_setup
alias mygsl_setup='do_mygsl_setup'

# end of mygsl_setup.sh