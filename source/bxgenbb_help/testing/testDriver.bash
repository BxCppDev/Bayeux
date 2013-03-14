#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# testDriver.bash

APPNAME="genbb_help/testDriver"

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

tmp_test_dir=/tmp/${USER}/genbb_help/test
prefix_test_dir=
data_test_dir=
source_dir=
binary_dir=
bin_test_dir=
lib_test_dir=
etc_test_dir=
inc_test_dir=
exe_test=
# depends on materials data dir:
materials_data_test_dir=
# depends on geomtools data dir:
geomtools_data_test_dir=

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
	elif [ "${opt}" = "--source-dir" ]; then
	    shift 1
	    source_dir="$1"
	elif [ "${opt}" = "--binary-dir" ]; then
	    shift 1
	    binary_dir="$1"
	elif [ "${opt}" = "--bin-dir" ]; then
	    shift 1
	    bin_test_dir="$1"
	elif [ "${opt}" = "--lib-dir" ]; then
	    shift 1
	    lib_test_dir="$1"
	elif [ "${opt}" = "--etc-dir" ]; then
	    shift 1
	    etc_test_dir="$1"
	elif [ "${opt}" = "--include-dir" ]; then
	    shift 1
	    inc_test_dir="$1"
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
    (
	echo "DEBUG: ${appname}: the_action_mode=${the_action_mode}"
	echo "DEBUG: ${appname}: tmp_test_dir=${tmp_test_dir}" 
	echo "DEBUG: ${appname}: exe_test=${exe_test}" 
	echo "DEBUG: ${appname}: prefix_test_dir=${prefix_test_dir}" 
	echo "DEBUG: ${appname}: data_test_dir=${data_test_dir}"
	echo "DEBUG: ${appname}: materials_data_test_dir=${materials_data_test_dir}"
	echo "DEBUG: ${appname}: geomtools_data_test_dir=${geomtools_data_test_dir}"
    ) >> ${tmp_test_dir}/tests.log 2>&1
fi

##########################################################

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

    if [ "x${inc_test_dir}" != "x" ]; then
	export GENBB_HELP_INCLUDE_DIR=${inc_test_dir}
    fi

    if [ "x${etc_test_dir}" != "x" ]; then
	export GENBB_HELP_ETC_DIR=${etc_test_dir}
    fi

    if [ "x${bin_test_dir}" != "x" ]; then
	export GENBB_HELP_BIN_DIR=${bin_test_dir}
    fi

    if [ "x${lib_test_dir}" != "x" ]; then
	export GENBB_HELP_LIB_DIR=${lib_test_dir}
    fi

    if [ "x${data_test_dir}" != "x" ]; then
	export GENBB_HELP_DATA_DIR=${data_test_dir}
    fi

    if [ "x${GENBB_HELP_DATA_DIR}" = "x" ]; then
	echo "ERROR: ${appname}: Missing GENBB_HELP_DATA_DIR environment variable !"
	return 1
    fi
    if [ ! -d ${GENBB_HELP_DATA_DIR} ]; then
	echo "ERROR: ${appname}: Directory '${GENBB_HELP_DATA_DIR}' does not exists !"
	return 1
    fi
    
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

    
    ###############################
    
    echo "NOTICE: ${appname}: First clean the test temporary directory..." >&2
    if [ ! -d ${tmp_test_dir} ]; then
	mkdir -p ${tmp_test_dir}
    fi
    cd ${tmp_test_dir}
    
    cat >> ${tmp_test_dir}/tests.log<<EOF

****************************************************
genbb_help test log file :
'${exe_test}' 
****************************************************
EOF
    bin=${exe_test}
    if [ ! -x ${bin} ]; then
	echo "ERROR: ${appname}: No '${bin}' executable available ! " 
	return 1
    fi
    exe=$(basename ${exe_test})
    if [ "${exe}" = "test_genbb" ]; then
	if [ "x${source_dir}" != "x" ]; then
	    export PATH=${source_dir}/scripts:${PATH}
	fi
	if [ "x${binary_dir}" != "x" ]; then
	    export PATH=${binary_dir}/programs:${PATH} 
	fi
	if [ "x${binary_dir}" != "x" ]; then
	    export PATH=${binary_dir}/programs:${PATH} 
	fi
	which genbb 2>&1  # /dev/null 2>&1 
	if [ ! -d /tmp/${USER}/genbb_work.d ]; then
	    mkdir -p /tmp/${USER}/genbb_work.d
	    if [ $? -ne 0 ]; then
		echo "ERROR: ${appname}: Cannot create temporary directory '/tmp/${USER}/genbb_work.d' ! " 
		return 1
	    fi
	fi
	${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi 
    elif [ "${exe}" = "test_beta" ]; then
	echo -e "${RANDOM}\n10\n" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi 
    elif [ "${exe}" = "test_rnd2" ]; then
	echo -e "${RANDOM}\n10\n" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi 
    elif [ "${exe}" = "test_rnd" ]; then
	echo -e "${RANDOM}\n10\n" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi 
    elif [ "${exe}" = "test_genbb_writer" ]; then
	${bin} -d -N 10 --type e- \
            --energy-min 0. --energy-max 5. \
            --phi-min 0. --phi-max 90. \
            --seed 314159 \
            --outfile ${tmp_test_dir}/test_genbb_writer.data \
	    >> ${tmp_test_dir}/tests.log 2>&1
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
