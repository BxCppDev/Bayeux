#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# testDriver.bash

APPNAME="mctools/testDriver"

opwd=$(pwd)

function my_exit ()
{
    cd ${opwd}
    exit $1
}

appname=${APPNAME}
appversion=0.1
the_base_dir=$(pwd)
debug=0

function print_usage ()
{
    cat <<EOF

  ${appname} -- Description

  Usage:

    ${appname} [Options...] ACTION [Options...]

  Options:

    -h
    --help    : print this help then exit

    --version  : print version then exit

EOF
    return 0
}

tmp_test_dir=/tmp/${USER}/mctools/test
prefix_test_dir=
data_test_dir=
exe_test=
geomtools_data_test_dir=
materials_data_test_dir=
genvtx_data_test_dir=
genbb_help_data_test_dir=
emfield_data_test_dir=

#######################################################

the_action_mode=
the_action_options=
parse_switch=1
while [ -n "$1" ]; do
    token="$1"
    if [ "${token:0:1}" = "-" ]; then
	opt=${token}
	if [ ${parse_switch} -eq 0 ]; then
	    break
	fi
	if [ "${opt}" = "-h" -o "${opt}" = "--help" ]; then
	    print_usage
	    my_exit 0
	elif [ "${opt}" = "-d" -o "${opt}" = "--debug" ]; then
	    debug=1
	elif [ "${opt}" = "--version" ]; then
	    echo ${appversion}
	    my_exit 0
	elif [ "${opt}" = "--prefix" ]; then
	    shift 1
	    prefix_test_dir="$1"
	elif [ "${opt}" = "--data-dir" ]; then
	    shift 1
	    data_test_dir="$1"
	elif [ "${opt}" = "--tmp-dir" ]; then
	    shift 1
	    tmp_test_dir="$1"
	elif [ "${opt}" = "--exe" ]; then
	    shift 1
	    exe_test="$1"
        elif [ "${opt}" = "--materials-data-dir" ]; then
            shift 1
            materials_data_test_dir="$1"
        elif [ "${opt}" = "--geomtools-data-dir" ]; then
            shift 1
            geomtools_data_test_dir="$1"
        elif [ "${opt}" = "--genvtx-data-dir" ]; then
            shift 1
            genvtx_data_test_dir="$1"
        elif [ "${opt}" = "--genbb_help-data-dir" ]; then
            shift 1
            genbb_help_data_test_dir="$1"
        elif [ "${opt}" = "--emfield-data-dir" ]; then
            shift 1
            emfield_data_test_dir="$1"
	else
	    echo "ERROR: ${appname}: Invalid option '${opt}' !" >&2
	    my_exit 1
	fi
    else
	arg=${token}
	parse_switch=0
	if [ "x${the_action_mode}" = "x" ]; then
	    if [ "$arg" = "run" ]; then
		the_action_mode="${arg}"
            elif [ "$arg" = "clean" ]; then
	        the_action_mode="${arg}"
	    else
		echo "ERROR: ${appname}: Invalid argument '${arg}' !" >&2
		my_exit 1
	    fi
	else
	    break
	fi
    fi
    shift 1
done

if [ ${debug} -ne 0 ]; then
    echo "DEBUG: ${appname}: the_action_mode=${the_action_mode}" >&2
    echo "DEBUG: ${appname}: tmp_test_dir=${tmp_test_dir}" >&2
    echo "DEBUG: ${appname}: exe_test=${exe_test}" >&2
    echo "DEBUG: ${appname}: prefix_test_dir=${prefix_test_dir}" >&2
    echo "DEBUG: ${appname}: data_test_dir=${data_test_dir}" >&2
    echo "DEBUG: ${appname}: materials_data_test_dir=${materials_data_test_dir}" >&2
    echo "DEBUG: ${appname}: geomtools_data_test_dir=${geomtools_data_test_dir}" >&2
    echo "DEBUG: ${appname}: genvtx_data_test_dir=${genvtx_data_test_dir}" >&2
    echo "DEBUG: ${appname}: genbb_help_data_test_dir=${genbb_help_data_test_dir}" >&2
    echo "DEBUG: ${appname}: emfield_data_test_dir=${emfield_data_test_dir}" >&2
fi

##########################################################
test_exe="\
test_mctools \
"

function do_clean ()
{
    if [ -d ${tmp_test_dir} ]; then
	rm -fr ${tmp_test_dir}
    fi
    return 0
}

function do_run ()
{
    opwd=$(pwd)

    if [ "x${data_test_dir}" != "x" ]; then
	export MCTOOLS_DATA_DIR=${data_test_dir}
    fi

    if [ "x${MCTOOLS_DATA_DIR}" = "x" ]; then
	echo "ERROR: ${appname}: Missing MCTOOLS_DATA_DIR environment variable !"
	return 1
    fi
    if [ ! -d ${MCTOOLS_DATA_DIR} ]; then
	echo "ERROR: ${appname}: Directory '${MCTOOLS_DATA_DIR}' does not exists !"
	return 1
    fi

    export CONFIG_DIR=${MCTOOLS_DATA_DIR}/testing/config/g4/test-2.0

    ###############################
    # depends on materials data dir:
    if [ "x${materials_data_test_dir}" != "x" ]; then
        export MATERIALS_DATA_DIR=${materials_data_test_dir}
    fi
    if [ "x${MATERIALS_DATA_DIR}" = "x" ]; then
        echo "ERROR: ${appname}: Missing MATERIALS_DATA_DIR environment variable !" >&2
        return 1
    fi
    if [ ! -d ${MATERIALS_DATA_DIR} ]; then
        echo "ERROR: ${appname}: Directory '${MATERIALS_DATA_DIR}' does not exists !" >&2
        return 1
    fi
    echo "NOTICE: ${appname}: Directory MATERIALS_DATA_DIR='${MATERIALS_DATA_DIR}'"  >&2

    # depends on geomtools data dir:
    if [ "x${geomtools_data_test_dir}" != "x" ]; then
        export GEOMTOOLS_DATA_DIR=${geomtools_data_test_dir}
    fi
    if [ "x${GEOMTOOLS_DATA_DIR}" = "x" ]; then
        echo "ERROR: ${appname}: Missing GEOMTOOLS_DATA_DIR environment variable !" >&2
        return 1
    fi
    if [ ! -d ${GEOMTOOLS_DATA_DIR} ]; then
        echo "ERROR: ${appname}: Directory '${GEOMTOOLS_DATA_DIR}' does not exists !" >&2
        return 1
    fi
    echo "NOTICE: ${appname}: Directory GEOMTOOLS_DATA_DIR='${GEOMTOOLS_DATA_DIR}'"  >&2

    # depends on genvtx data dir:
    if [ "x${genvtx_data_test_dir}" != "x" ]; then
        export GENVTX_DATA_DIR=${genvtx_data_test_dir}
    fi
    if [ "x${GENVTX_DATA_DIR}" = "x" ]; then
        echo "ERROR: ${appname}: Missing GENVTX_DATA_DIR environment variable !" >&2
        return 1
    fi
    if [ ! -d ${GENVTX_DATA_DIR} ]; then
        echo "ERROR: ${appname}: Directory '${GENVTX_DATA_DIR}' does not exists !" >&2
        return 1
    fi
    echo "NOTICE: ${appname}: Directory GENVTX_DATA_DIR='${GENVTX_DATA_DIR}'"  >&2

    # depends on genbb_help data dir:
    if [ "x${genbb_help_data_test_dir}" != "x" ]; then
        export GENBB_HELP_DATA_DIR=${genbb_help_data_test_dir}
    fi
    if [ "x${GENBB_HELP_DATA_DIR}" = "x" ]; then
        echo "ERROR: ${appname}: Missing GENBB_HELP_DATA_DIR environment variable !" >&2
        return 1
    fi
    if [ ! -d ${GENBB_HELP_DATA_DIR} ]; then
        echo "ERROR: ${appname}: Directory '${GENBB_HELP_DATA_DIR}' does not exists !" >&2
        return 1
    fi
    echo "NOTICE: ${appname}: Directory GENBB_HELP_DATA_DIR='${GENBB_HELP_DATA_DIR}'"  >&2

    # depends on emfield data dir:
    if [ "x${emfield_data_test_dir}" != "x" ]; then
        export EMFIELD_DATA_DIR=${emfield_data_test_dir}
    fi
    if [ "x${EMFIELD_DATA_DIR}" = "x" ]; then
        echo "ERROR: ${appname}: Missing EMFIELD_DATA_DIR environment variable !" >&2
        return 1
    fi
    if [ ! -d ${EMFIELD_DATA_DIR} ]; then
        echo "ERROR: ${appname}: Directory '${EMFIELD_DATA_DIR}' does not exists !" >&2
        return 1
    fi
    echo "NOTICE: ${appname}: Directory EMFIELD_DATA_DIR='${EMFIELD_DATA_DIR}'"  >&2


    #############################
    echo "NOTICE: ${appname}: First clean the test temporary directory..." >&2
    if [ ! -d ${tmp_test_dir} ]; then
	mkdir -p ${tmp_test_dir}
    fi
    cd ${tmp_test_dir}

    cat >> ${tmp_test_dir}/tests.log<<EOF

****************************************************
mctools test log file :
'${exe_test}'
****************************************************
EOF
    bin=${exe_test}
    if [ ! -x ${bin} ]; then
	echo "ERROR: ${appname}: No '${bin}' exectuable available ! "
	return 1
    fi
    exe=$(basename ${exe_test})
    if [ "${exe}" = "test_XXX" ]; then
	${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    else
	${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    fi

    cd ${opwd}
    return 0
}

function main ()
{
    local action_mode=${the_action_mode}
    local action_options=${the_action_options}

    # Some checks...
    if [ -z "${action_mode}" ]; then
	echo "ERROR: ${appname}: Missing action !" >&2
	print_usage
	return 1
    fi

    # Perform action...
    if [ "${action_mode}" = "run" ]; then
	if [ "x${exe_test}" = "x" ]; then
	    echo "ERROR: ${appname}: Missing executable name !" >&2
	    return 1
	fi
	do_run $@
	if [ $? -ne 0 ]; then
	    echo "ERROR: ${appname}: Running failed !" >&2
	    return 1
	fi
    fi

    if [ "${action_mode}" = "clean" ]; then
    	do_clean $@
    	if [ $? -ne 0 ]; then
    	    echo "ERROR: ${appname}: Cleaning failed !" >&2
    	    return 1
    	fi
    fi
    return 0
}

##########################################################

main
if [ $? -ne 0 ]; then
    echo "ERROR: ${appname}: Failure !" >&2
    my_exit 1
fi
my_exit 0

# end of testDriver.bash
