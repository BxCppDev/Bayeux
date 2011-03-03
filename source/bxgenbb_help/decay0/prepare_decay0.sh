#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# prepare_decay0.sh

APPNAME="prepare_decay0.sh"

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
if [ "x${GENBB_HELP_ROOT}" = "x" ]; then
    echo " ERROR: ${appname}: GENBB_HELP_ROOT is not defined ! Abort !" >&2  
    my_exit 1
else
    if [ ! -d ${GENBB_HELP_ROOT} ]; then
	echo " ERROR: ${appname}: GENBB_HELP_ROOT directory does not exist ! Abort !" >&2  
	my_exit 1
    fi
    the_base_dir=${GENBB_HELP_ROOT}
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

the_decay0_for_file=
check_prepare=0
extract_original=0

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
	elif [ "${opt}" = "-c" -o "${opt}" = "--check" ]; then
	    check_prepare=1
	    extract_original=0
	elif [ "${opt}" = "-o" -o "${opt}" = "--extract-original" ]; then
	    check_prepare=0
	    extract_original=1
	    pkgtools__msg_error "Unimplemented option '${opt}' !" 	    
	else
	    pkgtools__msg_error "Invalid option '${opt}' !" 
	    my_exit 1
	    # pkgtools__msg_warning "Ignoring option '${opt}' !" 
	fi
    else
	arg=${token}
	parse_switch=0
	if [ "x${the_decay0_for_file}" = "x" ]; then
	    the_decay0_for_file="${arg}"	
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
pkgtools__msg_devel "the_decay0_for_file=${the_decay0_for_file}"
#pkgtools__msg_devel "the_second_arg=${the_second_arg}"
pkgtools__msg_devel "remaining_options=${the_remaining_options}"

# Batch mode:
pkgtools__ui_is_interactive
if [ $? -ne 0 ]; then
    pkgtools__ui_not_using_gui
fi

if [ "x${the_decay0_for_file}" = "x" ]; then
    pkgtools__ui_is_interactive
    if [ $? -eq 0 ]; then
	pkgtools__ui_ask_user_string "Enter the Decay0 main program file to be processed" "${GENBB_HELP_ROOT}/data/decay0.for"
	the_decay0_for_file=$(pkgtools__ui_get_user_string)
	pkgtools__ui_reset_user_string
    fi
fi

if [ "x${the_decay0_for_file}" = "x" ]; then
    pkgtools__msg_error "Missing Decay0 main program file ! Abort !"
    my_exit 1
fi


##########################################################

function main ()
{
    __pkgtools__at_function_enter main
 
    if [ ! -f ${the_decay0_for_file} ]; then
	pkgtools__msg_error "Decay0 main program file '${the_decay0_for_file}' does not exist !"
	__pkgtools__at_function_exit
	return 1
    fi

    tmp_decay0_for="tmp.decay0.for"
    pkgtools__msg_notice "Apply basic modifications to the Decay0 main program file..."
    cat ${the_decay0_for_file} | \
	sed \
	-e 's/chfile\*40/chfile\*256/g' \
 	-e 's/rndm(/rnd1(/g' \
	-e 's/^d/c/g' \
	-e "s/print *,'wait/!print *,'wait/g" \
	-e "s/print *,'starting the generation/!print *,'starting the generation/g" \
	-e "s/print *,'final random integer/!print *,'final random integer/g" \
	-e "s/print *,'toallevents/!print *,'toallevents/g" \
	> ${tmp_decay0_for}

    if [ ! -d ${GENBB_HELP_ROOT}/decay0/src ]; then
	pkgtools__msg_notice "Creating '\${GENBB_HELP_ROOT}/decay0/src' directory..."
	mkdir -p ${GENBB_HELP_ROOT}/decay0/src
    fi
    
    pkgtools__msg_notice "Extracting subroutines and function from the Decay0 main program file..."
    python process_decay0.py ${tmp_decay0_for}

    pkgtools__msg_notice "Removing original '\${GENBB_HELP_ROOT}/decay0/src/fermi.f' file..."
    mv ${GENBB_HELP_ROOT}/decay0/src/fermi.f ${GENBB_HELP_ROOT}/decay0/src/fermi.f.replaced
    pkgtools__msg_notice "Removing original '\${GENBB_HELP_ROOT}/decay0/src/rnd1.f' file..."
    mv ${GENBB_HELP_ROOT}/decay0/src/rnd1.f ${GENBB_HELP_ROOT}/decay0/src/rnd1.f.replaced
    if [ ${check_prepare} -eq 1 ]; then
	pkgtools__msg_notice "Installing alternative code in place of CERNLIB stuff..."
	cp ${GENBB_HELP_ROOT}/src/genbb_gauss.f ${GENBB_HELP_ROOT}/decay0/src/
	cp ${GENBB_HELP_ROOT}/src/genbb_divdif.f ${GENBB_HELP_ROOT}/decay0/src/
	cp ${GENBB_HELP_ROOT}/src/genbb_fermi.f ${GENBB_HELP_ROOT}/decay0/src/
	cp ${GENBB_HELP_ROOT}/src/genbb_random.f ${GENBB_HELP_ROOT}/decay0/src/
	#cp ${GENBB_HELP_ROOT}/src/trick_nevents.f.inc ${GENBB_HELP_ROOT}/decay0/src/
	#cp ${GENBB_HELP_ROOT}/src/trick_write.f.inc ${GENBB_HELP_ROOT}/decay0/src/
	test ! -d ${GENBB_HELP_ROOT}/decay0/src/genbb_help/ && mkdir ${GENBB_HELP_ROOT}/decay0/src/genbb_help/
	cp ${GENBB_HELP_ROOT}/include/genbb_help/fermi.h ${GENBB_HELP_ROOT}/decay0/src/genbb_help/
	cp ${GENBB_HELP_ROOT}/include/genbb_help/rng.h ${GENBB_HELP_ROOT}/decay0/src/genbb_help/
	cp ${GENBB_HELP_ROOT}/src/fermi.cc ${GENBB_HELP_ROOT}/decay0/src/
	cp ${GENBB_HELP_ROOT}/src/rng.cc ${GENBB_HELP_ROOT}/decay0/src/
	cp ${GENBB_HELP_ROOT}/decay0/GNUmakefile.sample ${GENBB_HELP_ROOT}/decay0/src/GNUmakefile
    fi

    pkgtools__msg_notice "Adapting the '\${GENBB_HELP_ROOT}/decay0/src/GENBBsub.f' file..."
    sub_file="${GENBB_HELP_ROOT}/decay0/src/GENBBsub.f"
    mv ${sub_file} ${sub_file}.orig
    cat ${sub_file}.orig | \
	sed \
	-e 's/call rdmin/idummy=0 ! call rdmin/g' \
	-e 's/call rdmout/idummy=0 ! call rdmout/g' \
	-e 's/call datime/! call datime/g' \
	-e 's/aaaaa/ccccc/g' \
	> ${sub_file}.tmp
    mv -f ${sub_file}.tmp ${sub_file}

    pkgtools__msg_notice "Adapting the '\${GENBB_HELP_ROOT}/decay0/src/GENBBdia.f' file..."
    # flist=$(ls -1 ${GENBB_HELP_ROOT}/decay0/src/*.f)
    # for fortran_file in $flist; do
    # 	#ffile=$(basename ${fortran_file})
    # 	emacs -batch ${fortran_file} \
    # 	    --eval '(indent-region (point-min) (point-max) nil)' \
    # 	    --eval '(untabify (point-min) (point-max))' \
    # 	    -f save-buffer \
    # 	    -kill > /dev/null 2>&1
    # done

    dia_file="${GENBB_HELP_ROOT}/decay0/src/GENBBdia.f"
    mv ${dia_file} ${dia_file}.orig
    cat ${dia_file}.orig | \
	sed \
	-e 's/call GENBBsub/ier=0\n        ! call GENBBsub/g' \
	-e 's/\t+/     +/g' \
	> ${dia_file}.tmp

    # n29=$(grep -n "print 29" ${dia_file}.tmp  | cut -d: -f1)
    # n29end=$(egrep -n '^31' ${dia_file}.tmp  | cut -d: -f1)
    # let n29begin=n29-1
    # let n29end=n29end+0
    # n33=$(grep -n "print 33" ${dia_file}.tmp  | cut -d: -f1)
    # let n33begin=n33+0
    # let n33end=n33+10
    # pkgtools__msg_notice "To be commented : from line ${n29begin} to ${n29end}"
    # pkgtools__msg_notice "To be commented : from line ${n33begin} to ${n33end}"
    # awk -f f77_comment.awk -v fl="${n29begin}" -v ll="${n29end}" -v hl="${n29}" ${dia_file}.tmp | \
    # 	sed -e 's/c@HOOK/        include "trick_nevents.f.inc"/g' > ${dia_file}.tmp1
    # awk -f f77_comment.awk -v fl="${n33begin}" -v ll="${n33end}" -v hl="${n33}" ${dia_file}.tmp1 | \
    # 	sed -e 's/c@HOOK/        include "trick_write.f.inc"/g' > ${dia_file}.tmp2
    # mv -f ${dia_file}.tmp2 ${dia_file}.tm
    mv -f ${dia_file}.tmp ${dia_file}

    rm -f tmp.decay0.for
    rm -f routines.lis

    if [ ${check_prepare} -eq 0 ]; then
	
	pkgtools__ui_ask_user_yesno "Do you want to install the Decay0 source files in the genbb_help source tree" \
 "no"
	pkgtools__ui_user_yesno_is_yes
	if [ $? -eq 0 ]; then
	    pkgtools__msg_notice "Installing fortran source files from Decay0..."
	    cp -f ${GENBB_HELP_ROOT}/decay0/src/*.f  ${GENBB_HELP_ROOT}/src/
	    pkgtools__msg_notice "Installing fortran program file from Decay0..."
	    cp -f ${GENBB_HELP_ROOT}/decay0/src/*.F  ${GENBB_HELP_ROOT}/programs/
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

# end of prepare_decay0.sh
