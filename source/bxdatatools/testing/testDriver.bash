#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# testDriver.bash

APPNAME="datatools/testDriver"

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

tmp_test_dir=/tmp/${USER}/datatools/test
prefix_test_dir=
data_test_dir=
exe_test=

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
	elif [ "${opt}" = "--tmp-dir" ]; then
	    shift 1
	    tmp_test_dir="$1"
        elif [ "${opt}" = "--data-dir" ]; then
            shift 1
            data_test_dir="$1"
	elif [ "${opt}" = "--exe" ]; then
	    shift 1
	    exe_test="$1"
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
fi

##########################################################
test_exe="test_basic_event \
test_binary_serialization \
test_bio_1 \
test_cloneable \
test_cloneable_2 \
test_command \
test_data_serialization \
test_dummy_datatools \
test_event_id \
test_handle_1 \
test_handle_2 \
test_ioutils \
test_multi_properties \
test_multi_properties_0 \
test_named \
test_nans_ar \
test_predicate_1 \
test_properties \
test_properties_0 \
test_properties_2 \
test_properties_2b \
test_properties_3 \
test_properties_4 \
test_propinit \
test_ser_bitset \
test_serializable_1 \
test_serialization \
test_serialization_2 \
test_serialization_3 \
test_serialization_4 \
test_service_manager \
test_shared_ptr_0 \
test_shared_ptr_1 \
test_smart_ref \
test_temp_file \
test_things \
test_things_1 \
test_things_2 \
test_things_3 \
test_time_tools \
test_tmp \
test_tree_dump \
test_units \
test_utils \
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

    if [ "x${prefix_test_dir}" = "x" ]; then
	echo "ERROR: ${appname}: Missing prefix_test_dir !"
	return 1
    fi

    if [ "x${data_test_dir}" != "x" ]; then
        export DATATOOLS_DATA_DIR=${data_test_dir}
    fi

    if [ "x${DATATOOLS_DATA_DIR}" = "x" ]; then
        echo "ERROR: ${appname}: Missing DATATOOLS_DATA_DIR environment variable !" >&2
        return 1
    fi

    if [ ! -d ${DATATOOLS_DATA_DIR} ]; then
        echo "ERROR: ${appname}: Directory '${DATATOOLS_DATA_DIR}' does not exists !" >&2
        return 1
    fi
    
    ###export DATATOOLS_ROOT=${prefix_test_dir}
    echo "NOTICE: ${appname}: First clean the test temporary directory..." >&2
    if [ ! -d ${tmp_test_dir} ]; then
	mkdir -p ${tmp_test_dir}
    fi
    cd ${tmp_test_dir}
 
    cat >> ${tmp_test_dir}/tests.log<<EOF

****************************************************
datatool test log file :
'${exe_test}' 
****************************************************
EOF
    bin=${exe_test}
    if [ ! -x ${bin} ]; then
	echo "ERROR: ${appname}: No '${bin}' exectuable available ! "
	return 1
    fi
    exe=$(basename ${exe_test})
    if [ "${exe}" = "test_command" ]; then
	echo "exec name arg1 arg2" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi 
    elif [ "${exe}" = "test_event_id" ]; then
	echo "9_666" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi 
    elif [ "${exe}" = "test_ioutils" ]; then
	echo "3.14" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
	if [ $? -ne 0 ]; then
	    return 1
	fi 
    elif [ "${exe}" = "test_properties" ]; then
	echo \"my\" \"favorite\" \"color\" \"is\" \"blue\" | ${bin} >> ${tmp_test_dir}/tests.log 2>&1
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
