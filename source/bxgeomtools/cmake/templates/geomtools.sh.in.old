# -*- mode : shell-script; -*-
# geomtools_setup.sh

function do_geomtools_setup ()
{
    echo "NOTICE: do_geomtools_setup: Setup geomtools..." 1>&2

    if [ "x${GEOMTOOLS_ROOT}" != "x" ]; then
	echo "WARNING: do_geomtools_setup: geomtools is already setup !" 1>&2
	return 2
    fi 
 
    which datatools-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_geomtools_setup: Cannot find datatools setup ! Abort !" 1>&2
	return 1
    fi

    which mygsl-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "ERROR: do_geomtools_setup: Cannot find mygsl setup ! Abort !" 1>&2
	return 1
    fi
    
    export GEOMTOOLS_ROOT=@CMAKE_INSTALL_PREFIX@
    export GEOMTOOLS_ROOT_DIR=@CMAKE_INSTALL_PREFIX@
    export GEOMTOOLS_NAME=@geomtools_PACKAGE_NAME@
    export GEOMTOOLS_VERSION=@geomtools_VERSION@
    export GEOMTOOLS_BIN_DIR=@CMAKE_BIN_DIR@
    export GEOMTOOLS_ETC_DIR=@CMAKE_INSTALL_PREFIX@/etc
    export GEOMTOOLS_LIB_DIR=@CMAKE_LIB_DIR@
    export GEOMTOOLS_INCLUDE_DIR=@CMAKE_INSTALL_PREFIX@/include
    export GEOMTOOLS_RESOURCES_DIR=@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources

    if [ -n "${PATH}" ]; then
	export PATH="${GEOMTOOLS_BIN_DIR}:${PATH}"
    else
	export PATH="${GEOMTOOLS_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${GEOMTOOLS_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${GEOMTOOLS_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${GEOMTOOLS_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${GEOMTOOLS_LIB_DIR}"
	fi 
    fi

    if [ "x@GEOMTOOLS_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@GEOMTOOLS_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@_find_module_install_dir@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@_find_module_install_dir@"
	fi
    fi

    echo "NOTICE: do_geomtools_setup: Setup geomtools done." 1>&2
    return 0
}
typeset -fx do_geomtools_setup
alias geomtools_setup='do_geomtools_setup'

# end of geomtools_setup.sh