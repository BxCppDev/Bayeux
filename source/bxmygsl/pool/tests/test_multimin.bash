#!/bin/bash
# -*- mode: shell-script; -*-
#
#    params.a  = 3.0;
#    params.b  = 50.0;
#    params.r  = 0.15;
#    params.p1 = 0.30;
#    params.p2 = 0.45;
#    params.p3 = 0.10;
#    params.p4 = 0.15;
#

mode="run"
mode=""

while [ -n "$1" ]; do 
    arg="$1"
    if [ "x${arg:0:1}" = "x-" ]; then
	if [ "x${arg}" = "x--clean" ]; then
	    mode="clean" 
	elif [ "x${arg}" = "x--run" ]; then
	    mode="run" 
	elif [ "x${arg}" = "x--a" ]; then
	    mode="ana_a" 
	elif [ "x${arg}" = "x--b" ]; then
	    mode="ana_b" 
	elif [ "x${arg}" = "x--r" ]; then
	    mode="ana_r" 
	elif [ "x${arg}" = "x--p1" ]; then
	    mode="ana_p1" 
	elif [ "x${arg}" = "x--p2" ]; then
	    mode="ana_p2" 
	elif [ "x${arg}" = "x--p3" ]; then
	    mode="ana_p3" 
	elif [ "x${arg}" = "x--p4" ]; then
	    mode="ana_p4" 
	elif [ "x${arg}" = "x--f" ]; then
	    mode="ana_f" 
	elif [ "x${arg}" = "x--af" ]; then
	    mode="ana_af" 
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
    find . -name "*~" -exec rm -f \{\} \;
    rm -f *~
    rm -f *.ps
    rm -f fit.log
    rm -f *.data
    rm -f *.hist
    rm -f h*.gpm
    test -f test_multimin.pars && rm -f test_multimin.pars
    test -f test_multimin.pars.200 && rm -f test_multimin.pars.200
    test -f test_multimin.hist && rm -f test_multimin.hist
    test -f test_multimin.sol && rm -f test_multimin.sol
    test -f af.gpm && rm -f  af.gpm
}

function do_run()
{
    local BIN=${MYGSL_ROOT}/$(uname -s)-$(uname -m | sed 's/ //g')/bin/test_multimin
    local OPTS="-l -d --5000"
    rm -f test_multimin.hist test_multimin.pars test_multimin.sol 
    #rm -f test_multimin.hist test_multimin.pars test_multimin.sol
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
    mv test_multimin.pars test_multimin.pars.200
    return 0
}

function do_ana_a()
{
    cat test_multimin.pars.200 | cut -d' ' -f1 | gsl-histogram 2.9 3.1 20 > a.hist
    cat>ha.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [2.9:3.1]
set yrange [0:]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_a=3.0
a=1.0
u_sa=0.05
sa=1.0
u_n=100.0
n=1.0
abin=(3.1-2.9)/20
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

    (echo "set term postscript color"; cat ha.gpm) | gnuplot > ha.ps
    gv ha.ps &
    return 0
}

function do_ana_b()
{
    cat test_multimin.pars.200 | cut -d' ' -f2 | gsl-histogram 40.0 60.0 20 > b.hist
    cat>hb.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [40.0:60.0]
set yrange [0:]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_b=50.0
b=1.0
u_sb=1.0
sb=1.0
u_n=100.0
n=1.0
abin=(60.-40.)/20
fb(x)=n*u_n*fgauss(x,b*u_b,sb*u_sb)
fb_fit(x)=fb(x+0.5*abin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit fb_fit(x) 'b.hist' using 1:3:(ferr(\$3)) via b,sb,n
set samples 10000
plot 'b.hist' using 1:3 with steps, fb(x) with lines

print "b  = ", b*u_b, " +/- " , b_err*u_b
print "sb = ", sb*u_sb, " +/- " , sb_err*u_sb
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF

    (echo "set term postscript color"; cat hb.gpm) | gnuplot > hb.ps
    gv hb.ps &
    return 0
}

function do_ana_r()
{
    cat test_multimin.pars.200 | cut -d' ' -f3 | gsl-histogram 0.14 0.16 20 > r.hist
    cat>hr.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [0.14:0.16]
set yrange [0:]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_r=0.15
r=1.0
u_sr=0.001
sr=1.0
u_n=100.0
n=1.0
abin=(0.16-0.14)/20
fr(x)=n*u_n*fgauss(x,r*u_r,sr*u_sr)
fr_fit(x)=fr(x+0.5*abin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit fr_fit(x) 'r.hist' using 1:3:(ferr(\$3)) via r,sr,n
set samples 10000
plot 'r.hist' using 1:3 with steps, fr(x) with lines

print "r  = ", r*u_r, " +/- " , r_err*u_r
print "sr = ", sr*u_sr, " +/- " , sr_err*u_sr
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF

    (echo "set term postscript color"; cat hr.gpm) | gnuplot > hr.ps
    gv hr.ps &
    return 0
}

function do_ana_p1()
{
    cat test_multimin.pars.200 | cut -d' ' -f4| gsl-histogram 0.25 0.35 20 > p1.hist
    cat>hp1.gpm<<EOF
ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [0.25:0.35]
set yrange [0:]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_p1=0.30
p1=1.0
u_sp1=0.005
sp1=1.0
u_n=100.0
n=1.0
abin=(0.35-0.25)/20
fp1(x)=n*u_n*fgauss(x,p1*u_p1,sp1*u_sp1)
fp1_fit(x)=fp1(x+0.5*abin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit fp1_fit(x) 'p1.hist' using 1:3:(ferr(\$3)) via p1,sp1,n
set samples 10000
plot 'p1.hist' using 1:3 with steps, fp1(x) with lines

print "p1  = ", p1*u_p1, " +/- " , p1_err*u_p1
print "sp1 = ", sp1*u_sp1, " +/- " , sp1_err*u_sp1
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF
    (echo "set term postscript color"; cat hp1.gpm) | gnuplot > hp1.ps
    gv hp1.ps &
    return 0
}


function do_ana_p2()
{
    cat test_multimin.pars.200 | cut -d' ' -f5| gsl-histogram 0.40 0.50 40 > p2.hist
    cat>hp2.gpm<<EOF
ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [0.40:0.50]
set yrange [0:]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_p2=0.45
p2=1.0
u_sp2=0.005
sp2=1.0
u_n=100.0
n=1.0
abin=(0.5-0.4)/40
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
    (echo "set term postscript color"; cat hp2.gpm) | gnuplot > hp2.ps
    gv hp2.ps &
    return 0
}

function do_ana_p3()
{
    cat test_multimin.pars.200 | cut -d' ' -f6| gsl-histogram 0.08 0.12 10 > p3.hist
    cat>hp3.gpm<<EOF
ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [0.12:0.08]
set yrange [0:]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_p3=0.10
p3=1.0
u_sp3=0.005
sp3=1.0
u_n=100.0
n=1.0
abin=(0.12-0.08)/10
fp3(x)=n*u_n*fgauss(x,p3*u_p3,sp3*u_sp3)
fp3_fit(x)=fp3(x+0.5*abin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit fp3_fit(x) 'p3.hist' using 1:3:(ferr(\$3)) via p3,sp3,n
set samples 10000
plot 'p3.hist' using 1:3 with steps, fp3(x) with lines

print "p3  = ", p3*u_p3, " +/- " , p3_err*u_p3
print "sp3 = ", sp3*u_sp3, " +/- " , sp3_err*u_sp3
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF
    (echo "set term postscript color"; cat hp3.gpm) | gnuplot > hp3.ps
    gv hp3.ps &
    return 0
}

function do_ana_p4()
{
    cat test_multimin.pars.200 | cut -d' ' -f7| gsl-histogram 0.12 0.18 20 > p4.hist
    cat>hp4.gpm<<EOF
ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [0.12:0.18]
set yrange [0:]

fgauss(x,y,z)=exp(-0.5*((x-y)/z)**2)/(z*sqrt(2.*pi))
u_p4=0.15
p4=1.0
u_sp4=0.005
sp4=1.0
u_n=100.0
n=1.0
abin=(0.18-0.12)/20
fp4(x)=n*u_n*fgauss(x,p4*u_p4,sp4*u_sp4)
fp4_fit(x)=fp4(x+0.5*abin)

FIT_LIMIT = 1e-6
set fit errorvariables
fit fp4_fit(x) 'p4.hist' using 1:3:(ferr(\$3)) via p4,sp4,n
set samples 10000
plot 'p4.hist' using 1:3 with steps, fp4(x) with lines

print "p4  = ", p4*u_p4, " +/- " , p4_err*u_p4
print "sp4 = ", sp4*u_sp4, " +/- " , sp4_err*u_sp4
print "n  = ", n*u_n, " +/- " , n_err*u_n
EOF
    (echo "set term postscript color"; cat hp4.gpm) | gnuplot > hp4.ps
    gv hp4.ps &
    return 0
}

function do_ana_f()
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

    (echo "set term postscript color"; cat hf.gpm) | gnuplot > hf.ps
    gv hf.ps &
    return 0
}


function do_ana_af()
{

    cat test_multimin.pars.200 | cut -d' ' -f1,8 > af.data
    cat>af.gpm<<EOF

ferr(x)=x<1?1:sqrt(x)
set grid
set xrange [2.8:3.2]
set yrange [750:900]

plot "af.data" with points pt 6 ps 2
EOF
    (echo "set term postscript color"; cat af.gpm) | gnuplot > af.ps
    gv af.ps &
    return 0
}

if [ "x${mode}" = "xclean" ]; then
    do_clean
    exit 0
fi

if [ "x${mode}" = "xrun" ]; then
    do_run
    exit 0
fi

if [ "x${mode}" = "xana_a" ]; then
    do_ana_a
    exit 0
fi

if [ "x${mode}" = "xana_b" ]; then
    do_ana_b
    exit 0
fi

if [ "x${mode}" = "xana_r" ]; then
    do_ana_r
    exit 0
fi

if [ "x${mode}" = "xana_p1" ]; then
    do_ana_p1
    exit 0
fi

if [ "x${mode}" = "xana_p2" ]; then
    do_ana_p2
    exit 0
fi

if [ "x${mode}" = "xana_p3" ]; then
    do_ana_p3
    exit 0
fi

if [ "x${mode}" = "xana_p4" ]; then
    do_ana_p4
    exit 0
fi

if [ "x${mode}" = "xana_f" ]; then
    do_ana_f
    exit 0
fi

if [ "x${mode}" = "xana_af" ]; then
    do_ana_af
    exit 0
fi

exit 1

# end 