#!/usr/bin/env bash

export tmp_dir=/tmp/${USER}
genbb_root=${GENBB_HELP_SRC_ROOT}
if [ "x${genbb_root}" = "x" ]; then
    genbb_root=$(pwd)
fi 

sys=$(uname -s)-$(uname -m)
export PATH=${genbb_root}/__build-${sys}/programs:${PATH}

NEVENTS=200000
test -d ${tmp_dir} || mkdir -p ${tmp_dir} 

if [ ! -f ${tmp_dir}/se82_0nubb_mn.genbb ]; then
    ${genbb_root}/scripts/genbb ${genbb_root}/testing/config/se82_0nubb_mn.conf ${RANDOM} ${NEVENTS} ${tmp_dir}/se82_0nubb_mn.genbb
fi

# E_RESOL=0.08
# E_MIN=200
# E_SUM_MIN=0
# E_SUM_MAX=3400
PI="`echo print pi | gnuplot 2>&1 `"
TWOPI="`echo print '2*pi' | gnuplot 2>&1 `"
E_RESOL=0.0
E_MIN=0.
E_SUM_MIN=2000
E_SUM_MAX=10000
echo "DEVEL: PI=${PI}" >&2
echo "DEVEL: TWOPI=${TWOPI}" >&2
#exit 1

echo "DEVEL: Processing 'se82_0nubb_mn.genbb'..." >&2
${genbb_root}/__build-${sys}/testing/test_bb0nu_channel_2 \
  -s ${RANDOM} \
  -r ${E_RESOL} \
  -m ${E_MIN} \
  -sm ${E_SUM_MIN} \
  -sM ${E_SUM_MAX} \
  ${tmp_dir}/se82_0nubb_mn.genbb \
   > ${tmp_dir}/se82_bb0nu.data

cat ${tmp_dir}/se82_bb0nu.data \
    | cut -d ' ' -f 9 \
    | gsl-histogram 0 4000 200 \
    > ${tmp_dir}/se82_bb0nu_esum.his

cat ${tmp_dir}/se82_bb0nu.data \
    | cut -d ' ' -f 10 \
    | gsl-histogram -1 +1 50 \
    > ${tmp_dir}/se82_bb0nu_cost.his

${genbb_root}/testing/test_bb0nu_channel_2.py p1cost \
  | gsl-histogram -1 +1 50 \
    > ${tmp_dir}/se82_bb0nu_p1cost.his

${genbb_root}/testing/test_bb0nu_channel_2.py p1phy \
  | gsl-histogram -${PI} ${PI} 50 \
    > ${tmp_dir}/se82_bb0nu_p1phy.his

${genbb_root}/testing/test_bb0nu_channel_2.py p2cost \
  | gsl-histogram -1 +1 50 \
    > ${tmp_dir}/se82_bb0nu_p2cost.his

${genbb_root}/testing/test_bb0nu_channel_2.py p2phy \
  | gsl-histogram -${PI}0 ${PI} 50 \
    > ${tmp_dir}/se82_bb0nu_p2phy.his

cat> ${tmp_dir}/test_bb0nu_channel.gpl <<EOF

set grid

set xlabel "E1+E2 (keV)"
set ylabel "dN/d(E1+E2)"
plot [0:4000][0:*]\
  "${tmp_dir}/se82_bb0nu_esum.his" u 1:3 notitle with histeps
pause -1 "Hit return..."

set xlabel "cost"
set ylabel "dN/d(cost)"
plot [-1:+1][0:*]\
  "${tmp_dir}/se82_bb0nu_cost.his" u 1:3 notitle with histeps
pause -1 "Hit return..."

set xlabel "p1cost"
set ylabel "dN/d(p1cost)"
plot [-1:+1][0:*]\
  "${tmp_dir}/se82_bb0nu_p1cost.his" u 1:3 notitle with histeps
pause -1 "Hit return..."

set xlabel "p1phy"
set ylabel "dN/d(p1phy)"
plot [-pi:+pi][0:*]\
  "${tmp_dir}/se82_bb0nu_p1phy.his" u 1:3 notitle with histeps
pause -1 "Hit return..."

set xlabel "p2cost"
set ylabel "dN/d(p2cost)"
plot [-1:+1][0:*]\
  "${tmp_dir}/se82_bb0nu_p2cost.his" u 1:3 notitle with histeps
pause -1 "Hit return..."

set xlabel "p2phy"
set ylabel "dN/d(p2phy)"
plot [-pi:+pi][0:*]\
  "${tmp_dir}/se82_bb0nu_p2phy.his" u 1:3 notitle with histeps
pause -1 "Hit return..."

EOF

gnuplot ${tmp_dir}/test_bb0nu_channel.gpl 

# rm -f ${tmp_dir}/test_bb0nu_channel.gpl 
# rm -f ${tmp_dir}/se82_0nubb_mn.genbb
# rm -f ${tmp_dir}/se82_bb0nu.data 
# rm -f ${tmp_dir}/se82_bb0nu_esum.his 
# rm -f ${tmp_dir}/se82_bb0nu_cost.his 

exit 0

# end
