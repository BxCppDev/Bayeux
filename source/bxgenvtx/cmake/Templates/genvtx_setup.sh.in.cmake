# -*- mode: shell-script; -*-
# genvtx_setup.sh

function do_genvtx_setup ()
{
    echo "NOTICE: do_genvtx_setup: Setup genvtx ..." 1>&2

    if [ "x${GENVTX_ROOT}" != "x" ]; then
	echo "WARNING: do_genvtx_setup: genvtx is already setup !" 1>&2
	return 2
    fi 
 
    # which root-config >/dev/null 2>&1
    # if [ $? -ne 0 ]; then
    # 	echo "ERROR: do_genvtx_setup: Cannot find ROOT setup ! Abort !" 1>&2
    # 	return 1
    # fi

    # which datatools-config >/dev/null 2>&1
    # if [ $? -ne 0 ]; then
    # 	echo "ERROR: do_genvtx_setup: Cannot find datatools setup ! Abort !" 1>&2
    # 	return 1
    # fi
    
    export GENVTX_ROOT="@CMAKE_INSTALL_PREFIX@"
    export GENVTX_ROOT_DIR="@CMAKE_INSTALL_PREFIX@"
    export GENVTX_NAME="@genvtx_PACKAGE_NAME@"
    export GENVTX_VERSION="@genvtx_VERSION_STR@"
    export GENVTX_BIN_DIR="@CMAKE_INSTALL_PREFIX@/bin"
    export GENVTX_ETC_DIR="@CMAKE_INSTALL_PREFIX@/etc"
    export GENVTX_LIB_DIR="@CMAKE_INSTALL_PREFIX@/lib"
    export GENVTX_INCLUDE_DIR="@CMAKE_INSTALL_PREFIX@/include"
    export GENVTX_RESOURCES_DIR="@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources"

    if [ -n "${PATH}" ]; then
	export PATH="${GENVTX_BIN_DIR}:${PATH}"
    else
	export PATH="${GENVTX_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${GENVTX_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${GENVTX_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${GENVTX_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${GENVTX_LIB_DIR}"
	fi 
    fi

    if [ "x@GENVTX_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@GENVTX_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@"
	fi
    fi
   
    echo "NOTICE: do_genvtx_setup: Setup genvtx done." 1>&2
    return 0
}
typeset -fx do_genvtx_setup
alias genvtx_setup='do_genvtx_setup'

echo "NOTICE: do_genvtx_setup: Please setup 'genvtx' with 'do_genvtx_setup'." 1>&2

# end of genvtx_setup.sh