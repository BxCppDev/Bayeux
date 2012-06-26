#!/usr/bin/env bash

export tmp_dir=/tmp/${USER}
genbb_root=${GENBB_HELP_SRC_ROOT}
if [ "x${genbb_root}" = "x" ]; then
    genbb_root=$(pwd)
fi 

sys=$(uname -s)-$(uname -m)
export PATH=${genbb_root}/__build-${sys}/programs:${PATH}

if [ ! -f ${tmp_dir}/se82_0nubb_mn.genbb ]; then
    ${genbb_root}/scripts/genbb ${genbb_root}/testing/config/se82_0nubb_mn.conf ${RANDOM} 2000 ${tmp_dir}/se82_0nubb_mn.genbb
fi
if [ ! -f ${tmp_dir}/se82_2nubb.genbb ]; then
    ${genbb_root}/scripts/genbb ${genbb_root}/testing/config/se82_2nubb.conf ${RANDOM} 40000 ${tmp_dir}/se82_2nubb.genbb
fi

E_RESOL=0.08
E_MIN=200
E_SUM_MIN=0
E_SUM_MAX=3400

#E_RESOL=0.0
#E_MIN=0.
#E_SUM_MIN=0
#E_SUM_MAX=10000

echo "DEVEL: Processing 'se82_0nubb_mn.genbb'..." >&2
${genbb_root}/__build-${sys}/testing/test_bb0nu_channel \
  -s ${RANDOM} \
  -r ${E_RESOL} \
  -m ${E_MIN} \
  -sm ${E_SUM_MIN} \
  -sM ${E_SUM_MAX} \
  ${tmp_dir}/se82_0nubb_mn.genbb \
   > ${tmp_dir}/se82_bb0nu.data

cat ${tmp_dir}/se82_bb0nu.data \
    | cut -d ' ' -f 3 \
    | gsl-histogram 0 4000 200 \
    > ${tmp_dir}/se82_bb0nu.his

###E_SUM_MIN=2000

echo "DEVEL: Processing 'se82_2nubb.genbb'..." >&2
${genbb_root}/__build-${sys}/testing/test_bb0nu_channel \
  -s ${RANDOM} \
  -r ${E_RESOL} \
  -m ${E_MIN} \
  -sm ${E_SUM_MIN} \
  -sM ${E_SUM_MAX} \
  ${tmp_dir}/se82_2nubb.genbb \
    > ${tmp_dir}/se82_bb2nu.data

cat ${tmp_dir}/se82_bb2nu.data \
    | cut -d ' ' -f 3 \
    | gsl-histogram 0 4000 200 \
    > ${tmp_dir}/se82_bb2nu.his

cat> ${tmp_dir}/test_bb0nu_channel.gpl <<EOF

set grid
set xlabel "E1+E2 (keV)"
set ylabel "dN/d(E1+E2)"
plot [0:4000][0:*]\
  "${tmp_dir}/se82_bb0nu.his" u 1:3 notitle with histeps,\
  "${tmp_dir}/se82_bb2nu.his" u 1:3 notitle with histeps
pause -1 "Hit return..."
EOF

gnuplot ${tmp_dir}/test_bb0nu_channel.gpl 

rm -f ${tmp_dir}/test_bb0nu_channel.gpl 
rm -f ${tmp_dir}/se82_0nubb_mn.genbb
rm -f ${tmp_dir}/se82_bb0nu.data 
rm -f ${tmp_dir}/se82_bb2nu.data 
rm -f ${tmp_dir}/se82_bb0nu.his 
rm -f ${tmp_dir}/se82_bb2nu.his 

exit 0

# end
