#!/bin/bash
# -*- mode: shell-script; -*-
#
# ./test_ode.bash --run --rk8pd --debug
# ./test_ode.bash --run --bsimp --debug
# ./test_ode.bash --clean
#

mode="run"
ode_type="rk8pd"
debug=0
use_jacobian=0

while [ -n "$1" ]; do 
    arg="$1"
    if [ "x${arg:0:1}" = "x-" ]; then
	if [ "x${arg}" = "x--clean" ]; then
	    mode="clean" 
	elif [ "x${arg}" = "x--run" ]; then
	    mode="run" 
	elif [ "x${arg}" = "x--rk8pd" ]; then
	    ode_type="rk8pd" 
	elif [ "x${arg}" = "x--bsimp" ]; then
	    use_jacobian=1
	    ode_type="bsimp" 
	elif [ "x${arg}" = "x--rk2" ]; then
	    ode_type="rk2" 
	elif [ "x${arg}" = "x--rk4" ]; then
	    ode_type="rk4" 
	elif [ "x${arg}" = "x--rkf45" ]; then
	    ode_type="rkf45" 
	elif [ "x${arg}" = "x--rkck" ]; then
	    ode_type="rkck" 
	elif [ "x${arg}" = "x--rk2imp" ]; then
	    ode_type="rk2imp" 
	elif [ "x${arg}" = "x--rk4imp" ]; then
	    ode_type="rk4imp" 
	elif [ "x${arg}" = "x--gear1" ]; then
	    ode_type="gear1" 
	elif [ "x${arg}" = "x--gear2" ]; then
	    ode_type="gear2" 
	elif [ "x${arg}" = "x--debug" ]; then
	    debug=1 
	else
	    echo "Ignoring option '$arg'!" >&2
	fi
    else
	echo "Ignoring argument '$arg'!" >&2
    fi
    shift 1
done

prefix=__test_ode

function do_clean()
{
    rm -f *~
    rm -f ${prefix}.gnuplot
    rm -f ${prefix}.data
}

function do_run()
{

    BIN=${MYGSL_ROOT}/$(uname -s)-$(uname -m | sed 's/ //g')/bin/test_ode

    debug_opt=
    if [ $debug -ne 0 ]; then
	debug_opt=--debug
    fi
    use_jacobian_opt=
    if [ $use_jacobian -ne 0 ]; then
	use_jacobian_opt=--use-jacobian
    fi


    ${BIN} ${debug_opt} ${use_jacobian_opt} ${ode_type} > ${prefix}.data
    
    cat > ${prefix}.gnuplot <<EOF

set grid
set title "mygsl - test_ode (${ode_type})"

set xrange [0:]
set xlabel "Time"
set yrange [-2:+2]
set ylabel "Position"

plot '${prefix}.data' using 1:2 with lines
pause -1 "Hit return to quit."

EOF

    gnuplot ${prefix}.gnuplot
}


if [ "x${mode}" = "xclean" ]; then
    do_clean
    exit 0
fi

if [ "x${mode}" = "xrun" ]; then
    do_run
    exit 0
fi

exit 1

# end