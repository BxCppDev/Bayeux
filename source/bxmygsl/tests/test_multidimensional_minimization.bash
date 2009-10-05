#!/bin/bash
# -*- mode: shell-script; -*-
#

mode="run"

while [ -n "$1" ]; do 
    arg="$1"
    if [ "x${arg:0:1}" = "x-" ]; then
	if [ "x${arg}" = "x--clean" ]; then
	    mode="clean" 
	elif [ "x${arg}" = "x--run" ]; then
	    mode="run" 
	else
	    echo "Ignoring option '$arg'!" >&2
	fi
    else
	echo "Ignoring argument '$arg'!" >&2
    fi
    shift 1
done

BIN=${MYGSL_ROOT}/$(uname -s)-$(uname -m | sed 's/ //g')/bin/test_multidimensional_minimization

function do_clean ()
{
    find . -name "*~" -exec rm -f \{\} \;
    rm -f test_multimin_f_*.data test_multimin.his test_multimin.ini test_multimin.ps 
}

function do_run ()
{

    rm -f test_multimin.his test_multimin.ini test_multimin.ps test_multimin.lop 
    local OPTS="--chisquare --plot-f --debug --nmsimplex --50000 --seed 1"    
    OPTS="--chisquare --plot-f --nmsimplex --50000 --seed 1"    
    OPTS="--likelihood --plot-f --nmsimplex --50000 --seed 1"    
    ${BIN} ${OPTS}
    gnuplot ${MYGSL_ROOT}/tests/test_multidimensional_minimization.gpl

    (echo "set term postscript color solid"; cat ${MYGSL_ROOT}/tests/test_multidimensional_minimization.gpl) | gnuplot > test_multimin.ps

    echo ""
    test -f test_multimin.lop && cat test_multimin.lop
    echo ""
    test -f test_multimin.ps && (gv -scale=-2 test_multimin.ps &)
}

function do_multi_run ()
{
    local OPTS="-l -d --5000"
    rm -f test_multimin_f_*.data test_multimin.his test_multimin.ini test_multimin.ps 
    local nloops=100
    #nloops=5
    local iloop=0
    while [ ${iloop} -lt ${nloops} ]; do
	echo "DEBUG: Loop=${iloop}" >&2
	${BIN} ${OPTS}
	if [ $? -ne 0 ]; then
	    echo "ERROR: Failed!" >&2
	    return 1
	fi
	let iloop=iloop+1
    done
    cat test_multimin.pars
    #mv test_mm.pars test_mm.pars.200
    return 0
}

if [ "x${mode}" = "xrun" ]; then
    do_run
fi

if [ "x${mode}" = "xclean" ]; then
    do_clean
fi

exit 0

# end 