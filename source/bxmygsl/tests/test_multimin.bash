#!/bin/bash

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

function do_clean()
{
    rm -f *~
    rm -f *.data
    rm -f *.hist
    test -f test_multimin.pars && rm -f test_multimin.pars
    test -f test_multimin.pars.200 && rm -f test_multimin.pars.200
    test -f test_multimin.hist && rm -f test_multimin.hist
    test -f test_multimin.sol && rm -f test_multimin.sol
    test -f ha.gpm && rm -f ha.gpm
    test -f hf.gpm && rm -f hf.gpm
    test -f hp2.gpm && rm -f hp2.gpm
    test -f af.gpm && rm -f  af.gpm
}

if [ "x${mode}" = "xclean" ]; then
    do_clean
    exit 0
fi

function run()
{
    local BIN=${MYGSL_ROOT}/Linux-i686/bin/test_multimin
    local OPTS=-l -d 
    #rm -f test_multimin.hist test_multimin.pars test_multimin.sol
    local nloops=100
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
    return 0
}


#run


function ana_a()
{

    cat test_multimin.pars.200 | cut -d' ' -f1 | gsl-histogram 2.8 3.2 40 > a.hist

    cat>ha.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [2.8:3.2]
set yrange [0:50]
#plot 'a.hist' using 1:3 with steps

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_a=3.0
a=1.0
u_sa=0.05
sa=1.0
u_n=100.0
n=1.0
abin=(3.2-2.8)/40
fa(x)=n*u_n*fgauss(x,a*u_a,sa*u_sa)
fa_fit(x)=fa(x+0.5*abin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit fa_fit(x) 'a.hist' using 1:3:(ferr(\$3)) via a,sa,n
set samples 10000
plot 'a.hist' using 1:3 with steps, fa(x) with lines

print "a  = ", a*u_a, " +/- " , a_err*u_a
print "sa = ", sa*u_sa, " +/- " , sa_err*u_sa
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF

    #(echo "set term postscript color"; cat ha.gpm) | gnuplot > ha.ps
    #gv ha.ps &
    return 0
}

ana_a

function ana_p2()
{

    cat test_multimin.pars.200 | cut -d' ' -f5| gsl-histogram 0.30 0.60 60 > p2.hist

    cat>hp2.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [0.30:0.60]
set yrange [0:50]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_p2=0.45
p2=1.0
u_sp2=0.005
sp2=1.0
u_n=100.0
n=1.0
abin=(0.6-0.3)/60
fp2(x)=n*u_n*fgauss(x,p2*u_p2,sp2*u_sp2)
fp2_fit(x)=fp2(x+0.5*abin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit fp2_fit(x) 'p2.hist' using 1:3:(ferr(\$3)) via p2,sp2,n
set samples 10000
plot 'p2.hist' using 1:3 with steps, fp2(x) with lines

print "p2  = ", p2*u_p2, " +/- " , p2_err*u_p2
print "sp2 = ", sp2*u_sp2, " +/- " , sp2_err*u_sp2
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF
    return 0
}

ana_p2

function ana_f()
{

    cat test_multimin.pars.200 | cut -d' ' -f8 | gsl-histogram 750 900 25 > f.hist

    cat>hf.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [750:900]
set yrange [0:50]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_f=800.0
f=1.0
u_sf=10.0
sf=1.0
u_n=60.0
n=1.0
fbin=(900-750)/25
ff(x)=n*u_n*fgauss(x,f*u_f,sf*u_sf)
ff_fit(x)=ff(x+0.5*fbin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit ff_fit(x) 'f.hist' using 1:3:(ferr(\$3)) via f,sf,n
set samples 10000
set xrange [750:900]
set yrange [0:50]
plot 'f.hist' using 1:3 with steps, ff(x) with lines

print "f  = ", f*u_f, " +/- " , f_err*u_f
print "sf = ", sf*u_sf, " +/- " , sf_err*u_sf
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF

    #(echo "set term postscript color"; cat ha.gpm) | gnuplot > ha.ps
    #gv ha.ps &
    return 0
}

ana_f

function ana_af()
{

    cat test_multimin.pars.200 | cut -d' ' -f1,8 > af.data
    cat>af.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [2.8:3.2]
set yrange [750:900]


plot "af.data" with points pt 6 ps 2
EOF
    return 0
}

ana_af


# end 