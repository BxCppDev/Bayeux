#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# validationDriver.bash

APPNAME="emfield/validationDriver"

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

validation_tmp_dir=
validation_setup_bash=
validation_report="validation.report"
validation_log="validation.log"
validation_tmp_tagfile=".created"

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
	elif [ "${opt}" = "--setup" ]; then
	    shift 1
	    validation_setup_bash="$1"
	elif [ "${opt}" = "--tmp-dir" ]; then
	    shift 1
	    validation_tmp_dir="$1"
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
            elif [ "$arg" = "report" ]; then
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

the_action_options=$@

if [ "x${emfield_VALIDATION_TMP_DIR}" != "x" ]; then
    validation_tmp_dir="${emfield_VALIDATION_TMP_DIR}"  
fi

if [ "x${validation_tmp_dir}" = "x" ]; then
    validation_tmp_dir="/tmp/${USER}/emfield/validation"
fi

if [ ${debug} -ne 0 ]; then  
    echo "DEBUG: ${appname}: the_action_mode=${the_action_mode}" >&2
    echo "DEBUG: ${appname}: the_action_options=${the_action_options}" >&2
    echo "DEBUG: ${appname}: validation_tmp_dir=${validation_tmp_dir}" >&2
    echo "DEBUG: ${appname}: validation_setup_bash=${validation_setup_bash}" >&2
fi

if [ "x${EMFIELD_VERSION}" = "x" ]; then
    if [ "x${validation_setup_bash}" = "x" ]; then
	echo "ERROR: ${appname}: Missing emfield bash setup script !" >&2
	return 1
    else
        if [ ! -f ${validation_setup_bash} ]; then
	    echo "ERROR: ${appname}: '${validation_setup_bash}' does not exist !" >&2
	    return 1
	else
	    echo "NOTICE: ${appname}: Sourcing '${validation_setup_bash}'..." >&2
  	    source ${validation_setup_bash}
	    do_emfield_setup
	    if [ $? -ne 0 ]; then
		echo "ERROR: ${appname}: Could not setup 'emfield' !" >&2
		return 1
	    fi
	fi
    fi	
fi

validation_data_dir=${EMFIELD_DATA_DIR}/validation/data
if [ ! -d ${validation_data_dir} ]; then
    echo "NOTICE: ${appname}: Validation data directory is : '${validation_data_dir}'" >&2   
fi
 
##########################################################

function do_clean ()
{
    if [ -d ${validation_tmp_dir} ]; then
	if [ -f ${validation_tmp_dir}/${validation_tmp_tagfile} ]; then
	    echo "NOTICE: ${appname}: Removing the '${validation_tmp_dir}' validation directory..." >&2	    
	    ### 2012-02-28 FM : Possible bug here : comment the rm -fr
	    ### rm -fr ${validation_tmp_dir}
	    echo "NOTICE: ${appname}: The '${validation_tmp_dir}' validation directory was removed." >&2	    
	else
	    echo "NOTICE: ${appname}: Do not remove the '${validation_tmp_dir}' directory." >&2	    
	fi
    fi
    return 0
}

function do_report ()
{
    local log_file=${validation_tmp_dir}/${validation_log}
    local report_file=${validation_tmp_dir}/${validation_report}
    if [ ! -f ${report_file} ]; then
	echo "ERROR: ${appname}: Missing validation report file '${report_file}' !" >&2
	return 1
    fi
cat<<EOF

*********************************************************************

                   emfield ${EMFIELD_VERSION}

               Validation Protocol Report

*********************************************************************

EOF

    cat ${report_file}

    return 0
}

function do_run ()
{
    opwd=$(pwd)
    
    if [ "x${data_test_dir}" != "x" ]; then
	export EMFIELD_DATA_DIR=${data_test_dir}
    fi

    if [ "x${EMFIELD_DATA_DIR}" = "x" ]; then
	echo "ERROR: ${appname}: Missing EMFIELD_DATA_DIR environment variable !" >&2
	return 1
    fi
    if [ ! -d ${EMFIELD_DATA_DIR} ]; then
	echo "ERROR: ${appname}: Directory '${EMFIELD_DATA_DIR}' does not exists !" >&2
	return 1
    fi

    echo "NOTICE: ${appname}: First clean the validation temporary directory..." >&2
    if [ ! -d ${validation_tmp_dir} ]; then
	mkdir -p ${validation_tmp_dir}
	if [ $? -ne 0 ]; then
	    echo "ERROR: ${appname}: Cannot create validation temporary directory '${validation_tmp_dir}' !" >&2
	    return 1
	fi
	touch ${validation_tmp_dir}/${validation_tmp_tagfile}
    fi
    cd ${validation_tmp_dir}
 
    local log_file=${validation_tmp_dir}/${validation_log}
    local report_file=${validation_tmp_dir}/${validation_report}

    cat > ${log_file} <<EOF
*********************************************************************
emfield validation log file :  
Log file    : ${log_file}
Date        : $(date)
Hostname    : $(hostname)
Report file : ${report_file}
*********************************************************************
EOF

    cat > ${report_file} <<EOF
*********************************************************************
emfield validation report file :  
Report file : ${report_file}
Date        : $(date)
Hostname    : $(hostname)
Log file    : ${log_file}
*********************************************************************
EOF

    local list_of_validation_scripts=$(ls -1 ${EMFIELD_DATA_DIR}/validation/scripts/*.bash 2> /dev/null)
    echo "NOTICE: ${appname}: list_of_validation_scripts : ${list_of_validation_scripts}" >&2
    local total_number_of_scripts=0
    if [ "x${list_of_validation_scripts}" != "x" ]; then
	total_number_of_scripts=$( echo ${list_of_validation_scripts} | tr '[[:space:]]' ' ' | tr -s ' ' | tr ' ' '\n' | wc -l)
    fi
    echo "NOTICE: ${appname}: Total number of scripts : ${total_number_of_scripts}" >&2
    local number_of_failed_scripts=0 
   
    for script in ${list_of_validation_scripts}; do
	echo "NOTICE: ${appname}: Running validation script '${script}' ..." >&2
	cat >> ${log_file} <<EOF

*********************************************************************
Running emfield validation script : '${script}'
*********************************************************************
EOF
	cat >> ${report_file} <<EOF

*********************************************************************
Running emfield validation script : ${script}
*********************************************************************
EOF
	bash ${script} >> ${report_file} 2>> ${log_file}
	if [ $? -ne 0 ]; then
	    echo "ERROR: ${appname}: Validation script '${script}' failed !" >&2
	    let number_of_failed_scripts=number_of_failed_scripts+1
	fi
	echo "NOTICE: ${appname}: Validation script '${script}' is done." >&2
    done

    cat >> ${report_file} <<EOF
Number of validation scripts        : ${total_number_of_scripts}
Number of failed validation scripts : ${number_of_failed_scripts}
*********************************************************************
End of emfield validation report file.  
*********************************************************************
EOF

    cat >> ${log_file} <<EOF
*********************************************************************
End of emfield validation log file.  
*********************************************************************
EOF

    cd ${opwd}
    return 0
}

function main ()
{
    local action_mode=${the_action_mode}

    # Some checks...
    if [ -z "${action_mode}" ]; then
	echo "ERROR: ${appname}: Missing action !" >&2
	print_usage
	return 1
    fi

    # Perform action...    
    if [ "${action_mode}" = "run" ]; then
	do_run $@
    fi
     
    if [ "${action_mode}" = "report" ]; then
    	do_report $@
    	if [ $? -ne 0 ]; then
    	    echo "ERROR: ${appname}: Report validation failed !" >&2
    	    return 1
    	fi
    fi
   
    if [ "${action_mode}" = "clean" ]; then
    	do_clean $@
    	if [ $? -ne 0 ]; then
    	    echo "ERROR: ${appname}: Cleaning validation failed !" >&2
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

# end of validationDriver.bash
