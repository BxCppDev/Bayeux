#!/usr/bin/env bash
# -*- mode: shell-script; -*-
# testDriver.bash

APPNAME="geomtools/testDriver"

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

tmp_test_dir=/tmp/${USER}/geomtools/testing
prefix_test_dir=
exe_test=
# depends on materials resource dir:
materials_resource_test_dir=


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
	elif [ "${opt}" = "--data-dir" ]; then
            shift 1
            data_test_dir="$1"
	elif [ "${opt}" = "--prefix" ]; then
	    shift 1
	    prefix_test_dir="$1"
	elif [ "${opt}" = "--tmp-dir" ]; then
	    shift 1
	    tmp_test_dir="$1"
	elif [ "${opt}" = "--exe" ]; then
	    shift 1
	    exe_test="$1"
        elif [ "${opt}" = "--materials-resource-dir" ]; then
            shift 1
            materials_resource_test_dir="$1"
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
    echo "DEBUG: ${appname}: materials_resource_test_dir=${materials_resource_test_dir}" >&2
fi

##########################################################
test_exe="\
test_geomtools \
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
    echo "NOTICE: ${appname}: do_run: Entering..." >&2
    opwd=$(pwd)

    if [ "x${prefix_test_dir}" != "x" ]; then
	export GEOMTOOLS_TESTING_DIR=${prefix_test_dir}/testing
    fi

    if [ "x${GEOMTOOLS_TESTING_DIR}" = "x" ]; then
	echo "ERROR: ${appname}: Missing GEOMTOOLS_TESTING_DIR environment variable !" >&2
	return 1
    fi
    if [ ! -d ${GEOMTOOLS_TESTING_DIR} ]; then
	echo "ERROR: ${appname}: Directory '${GEOMTOOLS_TESTING_DIR}' does not exists !" >&2
	return 1
    fi

    ###############################
    # depends on materials data dir:
    if [ "x${materials_resource_test_dir}" != "x" ]; then
        export MATERIALS_RESOURCE_DIR=${materials_resource_test_dir}
    fi
    if [ "x${MATERIALS_RESOURCE_DIR}" = "x" ]; then
        echo "ERROR: ${appname}: Missing MATERIALS_RESOURCE_DIR environment variable !" >&2
        return 1
    fi
    if [ ! -d ${MATERIALS_RESOURCE_DIR} ]; then
        echo "ERROR: ${appname}: Directory '${MATERIALS_RESOURCE_DIR}' does not exists !" >&2
        return 1
    fi
    echo "NOTICE: ${appname}: Directory MATERIALS_RESOURCE_DIR='${MATERIALS_RESOURCE_DIR}'"  >&2

    #############################

    echo "NOTICE: ${appname}: First clean the test temporary directory..." >&2
    if [ ! -d ${tmp_test_dir} ]; then
	mkdir -p ${tmp_test_dir}
    fi
    cd ${tmp_test_dir}

    export GEOMTOOLS_TMP_DIR=${tmp_test_dir}
    echo "NOTICE: ${appname}: GEOMTOOLS_TMP_DIR='${GEOMTOOLS_TMP_DIR}'" >&2

    cat >> ${tmp_test_dir}/tests.log<<EOF

****************************************************
geomtools test log file :
'${exe_test}'
****************************************************
EOF
    bin=${exe_test}
    if [ ! -x ${bin} ]; then
	echo "ERROR: ${appname}: No '${bin}' exectuable available ! " >&2
	return 1
    fi
    exe=$(basename ${exe_test})
    if [ "${exe}" = "test_address_set" ]; then
	echo -e "{1;4;5}\n" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_geom_id" ]; then
	echo "[12:1.45.3]" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_model_factory" ]; then
	echo -e "\n" | ${bin} --no-draw >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_multiple_placement" ]; then
	echo | ${bin} --no-draw >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_regular_grid_placement" ]; then
	echo | ${bin} --no-draw >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_regular_linear_placement" ]; then
	echo | ${bin} --no-draw >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_tube" ]; then
	echo "{tube 900 1000 1000}" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_union_3d" ]; then
	echo "" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_utils_2" ]; then
	echo -e "z\n90" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_serializable_3" ]; then
	${bin} --no-ab >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_stl_tools" ]; then
	${bin} -f ${GEOMTOOLS_TESTING_DIR}/data/test_stl_import.stl >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi
    elif [ "${exe}" = "test_manager" ]; then
	${bin} --plugins --gdml >> ${tmp_test_dir}/tests.log 2>&1
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
