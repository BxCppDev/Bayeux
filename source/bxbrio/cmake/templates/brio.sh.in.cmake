# -*- mode : shell-script; -*-
# brio_setup.sh

function do_brio_setup ()
{
    echo "NOTICE: do_brio_setup: Setup brio..." 1>&2

    if [ "x${BRIO_ROOT}" != "x" ]; then
	echo "WARNING: do_brio_setup: brio is already setup !" 1>&2
	return 2
    fi 
 
    which datatools-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_brio_setup: Cannot find datatools setup ! Abort !" 1>&2
	return 1
    fi

    which root-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_brio_setup: Cannot find Root setup ! Abort !" 1>&2
	return 1
    fi
    
    export BRIO_ROOT=@CMAKE_INSTALL_PREFIX@
    export BRIO_ROOT_DIR=@CMAKE_INSTALL_PREFIX@
    export BRIO_NAME=@brio_PACKAGE_NAME@
    export BRIO_VERSION=@brio_VERSION@
    export BRIO_BIN_DIR=@CMAKE_BIN_DIR@
    export BRIO_ETC_DIR=@CMAKE_INSTALL_PREFIX@/etc
    export BRIO_LIB_DIR=@CMAKE_LIB_DIR@
    export BRIO_INCLUDE_DIR=@CMAKE_INSTALL_PREFIX@/include
    export BRIO_RESOURCES_DIR=@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources

    if [ -n "${PATH}" ]; then
	export PATH="${BRIO_BIN_DIR}:${PATH}"
    else
	export PATH="${BRIO_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${BRIO_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${BRIO_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${BRIO_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${BRIO_LIB_DIR}"
	fi 
    fi

    if [ "x@BRIO_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@BRIO_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@_find_module_install_dir@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@_find_module_install_dir@"
	fi
    fi

    echo "NOTICE: do_brio_setup: Setup brio done." 1>&2
    return 0
}
typeset -fx do_brio_setup
alias brio_setup='do_brio_setup'

# end of brio_setup.sh