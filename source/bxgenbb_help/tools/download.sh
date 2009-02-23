#!/bin/bash
# 

cvs_host="https://cvs.in2p3.fr/nemo/"
cvs_pack=N3Nemos
cvs_path=simul/prog
files="genbb.cra genbb.com genbb.car"

opwd=$(pwd)
download=0
replace_fermi=1

function my_exit ()
{
    cd ${opwd}
    exit $1
}

if [ "x${GENBB_HELP_ROOT}" = "x" ]; then
    echo "ERROR: GENBB_HELP_ROOT is not defined! Abort!" >&2
    my_exit 1
fi


while [ -n "$1" ]; do
    arg="$1"

    if [ "x${arg:0:1}" = "x-" ]; then
	echo "opt=$arg"
	if [ "x${arg}" = "x--no-replace-fermi" ]; then
	    replace_fermi=0
	elif [ "x${arg}" = "x--no-download" ]; then
	    download=0
	elif [ "x${arg}" = "x--download" ]; then
	    download=1
	fi
    else
	echo "arg=$arg" >&2
    fi
    shift 1
done

###exit 0

#echo "TEST 1"


# download N3Nemos package:

tmpdir=/tmp/${USER}/genbb_work
test -d ${tmpdir} || mkdir -p ${tmpdir}
cd ${tmpdir}
pwd
#echo "TEST 2"

if [ ${download} -ne 0 ]; then
    #echo "TEST 2a"
    
    export CVSROOT=anoncvs@anoncvs.in2p3.fr:/cvs/nemo
    export CVS_RSH=ssh
    cvs co ${cvs_pack} 
    for file in ${files}; do
	cp ${tmpdir}/${cvs_pack}/${cvs_path}/${file} ${tmpdir}/
    done
else
    #echo "TEST 2b"
    for file in ${files}; do
	cp ${GENBB_HELP_ROOT}/data/${file} ${tmpdir}/
    done
fi

#echo "TEST 3"
which nypatchy
if [ $? -ne 0 ]; then
    echo "ERROR: Missing nypatchy! Abort!" >&2
    my_exit 1
fi
nypatchy <<EOF
zebrac.car100 genbb genbb.cra tty - - dummy.dat .go
EOF

#echo "TEST 4"
ls -l genbb.f

#echo "TEST 5"
if [ ${replace_fermi} -ne 0 ]; then
    #echo "TEST 5a"
    cat genbb.f | \
	sed -e 's/chfile\*40/chfile\*256/g' \
	-e 's/print 29/\n        include "trick_nevents.f.inc"\n/g' \
	-e 's/print 33/\n        include "trick_write.f.inc"\n/g' \
	-e 's/function fermi/function fermi_bad/g' \
	-e 's/fermi=p/fermi_bad=p/g' \
	-e 's/function fermi/function fermi_bad/g' \
	-e 's/fermi=p/fermi_bad=p/g' \
	-e 's/call GENBBsub/ier=0\n        ! call GENBBsub/g' \
	-e 's/rndm(d)/rnd1(d)/g' \
	> ${GENBB_HELP_ROOT}/src/genbb.f
    
else
    #echo "TEST 5b"
    echo "ERROR: 'replace_fermi==0' is not managed yet! Abort!" >&2
    my_exit 1    
fi

my_exit 0

# end