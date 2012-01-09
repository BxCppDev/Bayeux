# -*- mode: shell-script; -*-
# materials_setup.sh

function do_materials_setup ()
{
    echo "NOTICE: do_materials_setup: Setup materials ..." 1>&2

    if [ "x${MATERIALS_ROOT}" != "x" ]; then
	echo "WARNING: do_materials_setup: materials is already setup !" 1>&2
	return 2
    fi 
 
    # which root-config >/dev/null 2>&1
    # if [ $? -ne 0 ]; then
    # 	echo "ERROR: do_materials_setup: Cannot find ROOT setup ! Abort !" 1>&2
    # 	return 1
    # fi

    # which datatools-config >/dev/null 2>&1
    # if [ $? -ne 0 ]; then
    # 	echo "ERROR: do_materials_setup: Cannot find datatools setup ! Abort !" 1>&2
    # 	return 1
    # fi
    
    export MATERIALS_ROOT="@CMAKE_INSTALL_PREFIX@"
    export MATERIALS_ROOT_DIR="@CMAKE_INSTALL_PREFIX@"
    export MATERIALS_NAME="@materials_PACKAGE_NAME@"
    export MATERIALS_VERSION="@materials_VERSION_STR@"
    export MATERIALS_BIN_DIR="@CMAKE_INSTALL_PREFIX@/bin"
    export MATERIALS_ETC_DIR="@CMAKE_INSTALL_PREFIX@/etc"
    export MATERIALS_LIB_DIR="@CMAKE_INSTALL_PREFIX@/lib"
    export MATERIALS_INCLUDE_DIR="@CMAKE_INSTALL_PREFIX@/include"
    export MATERIALS_RESOURCES_DIR="@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources"

    if [ -n "${PATH}" ]; then
	export PATH="${MATERIALS_BIN_DIR}:${PATH}"
    else
	export PATH="${MATERIALS_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${MATERIALS_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${MATERIALS_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${MATERIALS_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${MATERIALS_LIB_DIR}"
	fi 
    fi

    if [ "x@MATERIALS_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@MATERIALS_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@MATERIALS_CMAKE_MODULE_INSTALL_DIR@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@MATERIALS_CMAKE_MODULE_INSTALL_DIR@"
	fi
    fi
   
    echo "NOTICE: do_materials_setup: Setup materials done." 1>&2
    return 0
}
typeset -fx do_materials_setup
alias materials_setup='do_materials_setup'

echo "NOTICE: do_materials_setup: Please setup 'materials' with 'do_materials_setup'." 1>&2

# end of materials_setup.sh