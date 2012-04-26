#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# testDriver.bash

APPNAME="emfield/testDriver"

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

tmp_test_dir=/tmp/${USER}/emfield/test
prefix_test_dir=
data_test_dir=
bin_test_dir=
lib_test_dir=
etc_test_dir=
inc_test_dir=
exe_test=
###yyy_data_test_dir=

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
	# elif [ "${opt}" = "--yyy-data-dir" ]; then
	#     shift 1
	#     yyy_data_test_dir="$1"
	else
	    echo "WARNING: ${appname}: Ignoring invalid option '${opt}' !" >&2
	    ### my_exit 1
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
		echo "WARNING: ${appname}: Ignoring invalid argument '${arg}' !" >&2
	        ### my_exit 1
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
    ###echo "DEBUG: ${appname}: yyy_data_test_dir=${yyy_data_test_dir}" >&2
fi

##########################################################

function do_clean ()
{
    if [ -d ${tmp_test_dir} ]; then
	echo "WARNING: Here we should remove the '${tmp_test_dir}' temporary directory used for testing !" >&2
        ### Uncomment the following line if you know what you are doing :
	### rm -fr ${tmp_test_dir}
    fi
    return 0
}

function do_run ()
{
    opwd=$(pwd)

    if [ "x${inc_test_dir}" != "x" ]; then
	export EMFIELD_INCLUDE_DIR=${inc_test_dir}
    fi

    if [ "x${etc_test_dir}" != "x" ]; then
	export EMFIELD_ETC_DIR=${etc_test_dir}
    fi

    if [ "x${bin_test_dir}" != "x" ]; then
	export EMFIELD_BIN_DIR=${bin_test_dir}
    fi

    if [ "x${lib_test_dir}" != "x" ]; then
	export EMFIELD_LIB_DIR=${lib_test_dir}
    fi

    if [ "x${data_test_dir}" != "x" ]; then
	export EMFIELD_DATA_DIR=${data_test_dir}
    fi

    if [ "x${EMFIELD_DATA_DIR}" = "x" ]; then
	echo "ERROR: ${appname}: Missing EMFIELD_DATA_DIR environment variable !"
	return 1
    fi
    if [ ! -d ${EMFIELD_DATA_DIR} ]; then
	echo "ERROR: ${appname}: Directory '${EMFIELD_DATA_DIR}' does not exists !"
	return 1
    fi

    # if [ "x${yyy_data_test_dir}" != "x" ]; then
    # 	export YYY_DATA_DIR=${yyy_data_test_dir}
    # fi
    # if [ "x${YYY_DATA_DIR}" = "x" ]; then
    # 	echo "ERROR: ${appname}: Missing YYY_DATA_DIR environment variable !"
    # 	return 1
    # fi
    # if [ ! -d ${YYY_DATA_DIR} ]; then
    # 	echo "ERROR: ${appname}: Directory '${YYY_DATA_DIR}' does not exists !"
    # 	return 1
    # fi

    echo "NOTICE: ${appname}: First clean the test temporary directory..." >&2
    if [ ! -d ${tmp_test_dir} ]; then
	mkdir -p ${tmp_test_dir}
    fi
    cd ${tmp_test_dir}
 
    cat >> ${tmp_test_dir}/tests.log<<EOF

****************************************************
emfield test log file :
'${exe_test}' 
****************************************************
EOF
    bin=${exe_test}
    if [ ! -x ${bin} ]; then
	echo "ERROR: ${appname}: No '${bin}' exectuable available ! "
	return 1
    fi
    exe=$(basename ${exe_test})
    if [ "${exe}" = "test_emfield" ]; then
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
