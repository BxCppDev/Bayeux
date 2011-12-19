#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# tests/run_tests.bash

APPNAME="tests/run_tests.bash"

opwd=$(pwd)
	
function my_exit ()
{
    cd ${opwd}
    exit $1
}

appname=${APPNAME} 
appversion=0.1 

the_base_dir=$(pwd)

# uncomment for usage related to a pkgtools managed package:
if [ "x${BRIO_ROOT}" = "x" ]; then
    echo " ERROR: ${appname}: BRIO_ROOT is not defined ! Abort !" >&2  
    my_exit 1
else
    if [ ! -d ${BRIO_ROOT} ]; then
	echo " ERROR: ${appname}: BRIO_ROOT directory does not exist ! Abort !" >&2  
	my_exit 1
    fi
    the_base_dir=${BRIO_ROOT}
fi

libpkgtools_sourced=0

pkgtools_root=${PKGTOOLS_ROOT}
if [ ${libpkgtools_sourced} -eq 0 ]; then
    if [ "x${pkgtools_root}" != "x" ]; then
	if [ -f ${pkgtools_root}/lib/libpkgtools.sh ]; then
	    source ${pkgtools_root}/lib/libpkgtools.sh
	    libpkgtools_sourced=1
	fi
    fi
fi

if [ ${libpkgtools_sourced} -eq 0 ]; then
    libpkgtools_sh=${the_base_dir}/pkgtools.d/libpkgtools.sh 
    if [ -f ${libpkgtools_sh} ]; then
	source ${libpkgtools_sh}    
	libpkgtools_sourced=1
    fi
fi

if [ ${libpkgtools_sourced} -eq 0 ]; then
    echo " ERROR: ${appname}: Cannot find any 'libpkgtools.sh' setup file ! Abort !" >&2  
    my_exit 1
fi

pkgtools__msg_not_using_quiet
pkgtools__msg_not_using_verbose
pkgtools__msg_not_using_debug
pkgtools__msg_not_using_devel
pkgtools__msg_not_using_date
pkgtools__msg_using_color
pkgtools__ui_interactive
pkgtools__ui_not_using_gui

tmp_test_dir=${BRIO_ROOT}/tests/tmp

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

    -d 
    --debug   : print debug mode informations

    -D 
    --devel   : print devel mode informations

    -v 
    --verbose : verbose print

    -i      
    --interactive  : run in interactive mode (this is the default)

    -b      
    --batch        : run in batch mode (no user interaction, disable GUI features)

    --no-color     : use un-colored message output

    --gui          : use GUI user interface (interactive mode only)

  Actions:

    run : Run the test programs

    clean : Clean the test programs' output

  Examples:

    shell> ${appname} --help
 
EOF
    return 0
}

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
	elif [ "${opt}" = "--version" ]; then
	    echo ${appversion}
	    my_exit 0
	elif [ "${opt}" = "-d" -o "${opt}" = "--debug" ]; then
	    pkgtools__msg_using_debug
	elif [ "${opt}" = "-D" -o "${opt}" = "--devel" ]; then
	    pkgtools__msg_using_devel
	elif [ "${opt}" = "-v" -o "${opt}" = "--verbose" ]; then
	    pkgtools__msg_using_verbose
	elif [ "${opt}" = "-i" -o "${opt}" = "--interactive" ]; then
	    pkgtools__ui_interactive
	elif [ "${opt}" = "-b" -o "${opt}" = "--batch" ]; then
	    pkgtools__ui_batch
	elif [ "${opt}" = "--no-color" ]; then
	    pkgtools__msg_not_using_color
	elif [ "${opt}" = "-G" -o "${opt}" = "--gui" ]; then
	    pkgtools__ui_using_gui
	    pkgtools__msg_devel "Using GUI" 	
	else
	    pkgtools__msg_error "Invalid option '${opt}' !" 
	    my_exit 1
	    # pkgtools__msg_warning "Ignoring option '${opt}' !" 
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
		pkgtools__msg_error "Invalid argument !"
		my_exit 1
	    fi
	else
	    # remaining arguments from command line are kept in "$@"
	    pkgtools__msg_devel "BREAK !"
	    break
	fi
    fi
    shift 1
done

the_action_options="$@"
pkgtools__msg_devel "the_action_mode=${the_action_mode}"
pkgtools__msg_devel "the_action_options=${the_action_options}"

# Batch mode:
pkgtools__ui_is_interactive
if [ $? -ne 0 ]; then
    pkgtools__ui_not_using_gui
fi

##########################################################
test_exe="test_dummy_brio \
test_data_io \
test_brio_writer_1 \
test_brio_reader_1 \
test_brio_writer_2 \
test_brio_reader_2 \
test_writer \
test_reader \
test_writer_2 \
test_reader_2 \
test_reader_3 \
"

function do_clean ()
{
    __pkgtools__at_function_enter do_clean

    if [ -d ${tmp_test_dir} ]; then
	rm -fr ${tmp_test_dir}
    fi

    __pkgtools__at_function_exit
}

function do_run ()
{
    __pkgtools__at_function_enter do_run

    opwd=$(pwd)
    pkgtools__msg_notice "First clean the test temporary directory..."
    do_clean
    if [ ! -d ${tmp_test_dir} ]; then
	mkdir ${tmp_test_dir}
    fi
    cd ${tmp_test_dir}
    count=0
    error_count=0
    for exe in ${test_exe} ; do
	let count=count+1
	pkgtools__msg_notice "Running ${exe}..."
	bin=${BRIO_BIN_DIR}/${exe}
	if [ "${exe}" = "test_command" ]; then
	    echo "exec name arg1 arg2" | ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	elif [ "${exe}" = "test_event_id" ]; then
	    echo "9_666" | ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	elif [ "${exe}" = "test_ioutils" ]; then
	    echo "3.14" | ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	elif [ "${exe}" = "test_properties" ]; then
	    echo \"my\" \"favorite\" \"color\" \"is\" \"blue\" | ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	else
	    ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	fi
    done
 
    pkgtools__msg_notice "Log file is '$(pwd)/tests.log'"
    pkgtools__msg_notice "Total Number of executable  = ${count}"
    pkgtools__msg_notice "Number of failed executable = ${error_count}"
   
    cd ${opwd}

    __pkgtools__at_function_exit
    return 0
}

# function do_something_else ()
# {
#     __pkgtools__at_function_enter do_something_else
#     pkgtools__msg_warning "Nothing special !"
#     __pkgtools__at_function_exit
#     return 0
# }

function main ()
{
    __pkgtools__at_function_enter main

    local action_mode=${the_action_mode}
    local action_options=${the_action_options}

    # Some checks...
    if [ -z "${action_mode}" ]; then
	pkgtools__msg_error "Missing action !"
	print_usage
	__pkgtools__at_function_exit
	return 1
    fi

    # Perform action...    
    if [ "${action_mode}" = "run" ]; then
	do_run $@
	if [ $? -ne 0 ]; then
	    pkgtools__msg_error "Running failed !"
	    __pkgtools__at_function_exit
	    return 1
	fi
    fi
    
    if [ "${action_mode}" = "clean" ]; then
    	do_clean $@
    	if [ $? -ne 0 ]; then
    	    pkgtools__msg_error "Cleaning failed !"
    	    __pkgtools__at_function_exit
    	    return 1
    	fi
    fi
  
    __pkgtools__at_function_exit
    return 0
}

##########################################################

main 
if [ $? -ne 0 ]; then
    pkgtools__msg_error "Failure !"
    my_exit 1
fi
my_exit 0

# end of tests/run_tests.bash
