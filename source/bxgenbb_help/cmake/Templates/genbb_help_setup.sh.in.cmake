# -*- mode: shell-script; -*-
# genbb_help_setup.sh

function do_genbb_help_setup ()
{
    echo "NOTICE: do_genbb_help_setup: Setup genbb_help ..." 1>&2

    if [ "x${GENBB_HELP_ROOT}" != "x" ]; then
	echo "WARNING: do_genbb_help_setup: genbb_help is already setup !" 1>&2
	return 2
    fi 

    which geomtools-config >/dev/null 2>&1
    if [ $? -ne 0 ]; then
    	echo "ERROR: do_genbb_help_setup: Cannot find geomtools setup ! Abort !" 1>&2
    	return 1
    fi
    
    export GENBB_HELP_ROOT="@CMAKE_INSTALL_PREFIX@"
    export GENBB_HELP_ROOT_DIR="@CMAKE_INSTALL_PREFIX@"
    export GENBB_HELP_NAME="@genbb_help_PACKAGE_NAME@"
    export GENBB_HELP_VERSION="@genbb_help_VERSION_STR@"
    export GENBB_HELP_BIN_DIR="@CMAKE_INSTALL_PREFIX@/bin"
    export GENBB_HELP_ETC_DIR="@CMAKE_INSTALL_PREFIX@/etc"
    export GENBB_HELP_LIB_DIR="@CMAKE_INSTALL_PREFIX@/lib"
    export GENBB_HELP_INCLUDE_DIR="@CMAKE_INSTALL_PREFIX@/include"
    export GENBB_HELP_SHARE_DIR="@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/"
    export GENBB_HELP_RESOURCES_DIR="@CMAKE_INSTALL_PREFIX@/share/@CMAKE_PROJECT_NAME@/resources"

    if [ -n "${PATH}" ]; then
	export PATH="${GENBB_HELP_BIN_DIR}:${PATH}"
    else
	export PATH="${GENBB_HELP_BIN_DIR}"
    fi 
   
    if [ "@CMAKE_SYSTEM_NAME@" = "Linux" ]; then 
	if [ -n "${LD_LIBRARY_PATH}" ]; then
	    export LD_LIBRARY_PATH="${GENBB_HELP_LIB_DIR}:${LD_LIBRARY_PATH}"
	else
	    export LD_LIBRARY_PATH="${GENBB_HELP_LIB_DIR}"
	fi 
    fi

    if [ "@CMAKE_SYSTEM_NAME@" = "Darwin" ]; then 
	if [ -n "${DYLD_LIBRARY_PATH}" ]; then
	    export DYLD_LIBRARY_PATH="${GENBB_HELP_LIB_DIR}:${DYLD_LIBRARY_PATH}"
	else
	    export DYLD_LIBRARY_PATH="${GENBB_HELP_LIB_DIR}"
	fi 
    fi

    if [ "x@GENBB_HELP_WITH_PKGCONFIG@" = "xON" ]; then
	if [ -n "${PKG_CONFIG_PATH}" ]; then 
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig:${PKG_CONFIG_PATH}"
	else
            export PKG_CONFIG_PATH="@CMAKE_INSTALL_PREFIX@/lib/pkgconfig"
	fi
    fi

    if [ "x@GENBB_HELP_WITH_CMAKE_FIND_MODULE@" = "xON" ]; then
	if [ -n "${CMAKE_MODULE_PATH}" ]; then 
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@:${CMAKE_MODULE_PATH}"
	else
            export CMAKE_MODULE_PATH="@CMAKE_INSTALL_PREFIX@/@_find_module_install_dir@"
	fi
    fi
   
    echo "NOTICE: do_genbb_help_setup: Setup genbb_help done." 1>&2
    return 0
}
typeset -fx do_genbb_help_setup
alias genbb_help_setup='do_genbb_help_setup'

echo "NOTICE: do_genbb_help_setup: Please setup 'genbb_help' with 'do_genbb_help_setup'." 1>&2

# end of genbb_help_setup.sh