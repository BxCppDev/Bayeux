# -*- mode: shell-script; -*-
# cuts_setup.sh

function do_cuts_setup ()
{
    echo "NOTICE: do_cuts_setup: Setup cuts ..." 1>&2

    if [ "x${CUTS_ROOT}" != "x" ]; then
	echo "WARNING: do_cuts_setup: cuts is already setup !" 1>&2
	return 2
    fi 
 
    which datatools-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
    	echo "ERROR: do_cuts_setup: Cannot find datatools setup ! Abort !" 1>&2
    	return 1
    fi
    
    export CUTS_ROOT="@CMAKE_INSTALL_PREFIX@"
    export CUTS_ROOT_DIR="@CMAKE_INSTALL_PREFIX@"
    export CUTS_NAME="@cuts_PACKAGE_NAME@"
    export CUTS_VERSION="@cuts_VERSION_STR@"
    export CUTS_BIN_DIR="@CMAKE_INSTALL_PREFIX@/bin"
    export CUTS_ETC_DIR="@CMAKE_INSTALL_PREFIX@/etc"
    export CUTS_LIB_DIR="@CMAKE_INSTALL_PREFIX@/lib"
    export CUTS_INCLUDE_DIR="@CMAKE_INSTALL_PREFIX@/include"
    export CUTS_RESOURCES_DIR="@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources"

    if [ -n "${PATH}" ]; then
	export PATH="${CUTS_BIN_DIR}:${PATH}"
    else
	export PATH="${CUTS_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${CUTS_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${CUTS_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${CUTS_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${CUTS_LIB_DIR}"
	fi 
    fi

    if [ "x@CUTS_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@CUTS_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@"
	fi
    fi
   
    echo "NOTICE: do_cuts_setup: Setup cuts done." 1>&2
    return 0
}
typeset -fx do_cuts_setup
alias cuts_setup='do_cuts_setup'

echo "NOTICE: do_cuts_setup: Please setup 'cuts' with 'do_cuts_setup'." 1>&2

# end of cuts_setup.sh