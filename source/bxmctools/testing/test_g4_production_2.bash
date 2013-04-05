#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# test_sng4_production_2.bash

APPNAME="test_sng4_production_2.bash"

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
if [ "x${SNG4_ROOT}" = "x" ]; then
    echo " ERROR: ${appname}: SNG4_ROOT is not defined ! Abort !" >&2  
    my_exit 1
else
    if [ ! -d ${SNG4_ROOT} ]; then
	echo " ERROR: ${appname}: SNG4_ROOT directory does not exist ! Abort !" >&2  
	my_exit 1
    fi
    the_base_dir=${SNG4_ROOT}
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
    echo "ERROR: ${appname}: Cannot find any 'libpkgtools.sh' setup file ! Abort !" >&2  
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

function print_usage ()
{
    cat <<EOF

  ${appname} -- Description

  Usage:

    ${appname} [Options...] [Args...]

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

  Examples:

    shell> ${appname} --help
 
EOF
    return 0
}

#######################################################

the_first_arg=
#the_second_arg=
the_remaining_options=
clean_only=0

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
	elif [ "${opt}" = "--clean-only" ]; then
	    clean_only=1
	else
	    pkgtools__msg_error "Invalid option '${opt}' !" 
	    my_exit 1
	    # pkgtools__msg_warning "Ignoring option '${opt}' !" 
	fi
    else
	arg=${token}
	parse_switch=0
	if [ "x${the_first_arg}" = "x" ]; then
	    the_first_arg="${arg}"	
	# elif [ "x${the_second_arg}" = "x" ]; then
	#     the_second_arg="${arg}"	
	else
	    pkgtools__msg_error "Invalid argument '${arg}' !"
	    my_exit 1
	    # pkgtools__msg_warning "Ignoring argument '${arg}' !" 
	    # break
	fi
    fi
    shift 1
done

the_remaining_options="$@"
#pkgtools__msg_devel "the_first_arg=${the_first_arg}"
#pkgtools__msg_devel "remaining_options=${the_remaining_options}"

# Batch mode:
pkgtools__ui_is_interactive
if [ $? -ne 0 ]; then
    pkgtools__ui_not_using_gui
fi

##########################################################

function clean ()
{
    __pkgtools__at_function_enter clean

    # rm -f *~ 
    # rm -f test_sng4_production_*.log 
    # rm -f test_sng4_production_*.seeds 
    # rm -f test_sng4_production_*.states 
    # rm -f test_sng4_production_*.txt.gz 

    __pkgtools__at_function_exit
    return 0
}

function main ()
{
    __pkgtools__at_function_enter main

    clean 
    if [ ${clean_only} -eq 1 ]; then
	__pkgtools__at_function_exit
	return 0
    fi

    NOEVTS=10000
    VGNAME="source_pads_bulk"
    EGNAME="bb0nu_Se82.wrapper"

    if [ ! -f test_sng4_production_2.txt.gz ]; then
	pkgtools__msg_notice "Simulation..."
	${SNG4_BIN_DIR}/sng4_production \
	    --batch \
	    --vertex-generator-name ${VGNAME} \
	    --vertex-generator-seed 0 \
	    --event-generator-name ${EGNAME} \
	    --event-generator-seed 0 \
	    --g4-manager-seed 0 \
	    --shpf-seed 0 \
	    --output-prng-seeds-file test_sng4_production.seeds \
	    --output-prng-states-file test_sng4_production.states \
	    --number-of-events ${NOEVTS} \
            --number-of-events-modulo 1000 \
            --prng-state-save-modulo 100 \
            --using-time-statistics \
            --forbid-private-hits \
	    --config ${SNG4_ROOT}/resources/snemo/config_1/manager.conf
	
	mv -f sng4_0.txt.gz test_sng4_production_2.txt.gz 
	mv -f test_sng4_production.seeds test_sng4_production_2.seeds 
	mv -f test_sng4_production.states test_sng4_production_2.states 
	pkgtools__msg_notice "Seeds are : "
	cat test_sng4_production_2.seeds 
    fi

    ${SNG4_BIN_DIR}/test_sng4_output_1 test_sng4_production_2.txt.gz 


    #rm -f test_sng4_production_2.txt.gz
    #rm -f test_sng4_production_2.log
    rm -f test_sng4_production_2.seeds
    rm -f test_sng4_production_2.states

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

# end of tests/test_sng4_production_2.bash
