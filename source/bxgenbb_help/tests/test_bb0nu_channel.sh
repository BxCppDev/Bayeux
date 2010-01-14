#!/bin/bash

tmp_dir=/tmp/${USER}

test -d ${tmp_dir} || mkdir -p ${tmp_dir} 

genbb ${GENBB_HELP_ROOT}/resources/se82_0nubb_mn.conf ${RANDOM} 100 ${tmp_dir}/se82_0nubb_mn.genbb

genbb ${GENBB_HELP_ROOT}/resources/se82_2nubb.conf ${RANDOM} 100000 ${tmp_dir}/se82_2nubb.genbb

E_RESOL=0.15
E_MIN=200
E_SUM_MIN=2600
E_SUM_MAX=3400

#E_RESOL=0.0
#E_MIN=0.
#E_SUM_MIN=0
#E_SUM_MAX=10000

bb0nu_channel \
  -s ${RANDOM} \
  -r ${E_RESOL} \
  -m ${E_MIN} \
  -sm ${E_SUM_MIN} \
  -sM ${E_SUM_MAX} \
  ${tmp_dir}/se82_0nubb_mn.genbb \
    | cut -d ' ' -f 3 \
    | gsl-histogram 0 4000 200 \
    > ${tmp_dir}/se82_bb0nu.data

bb0nu_channel \
  -s ${RANDOM} \
  -r ${E_RESOL} \
  -m ${E_MIN} \
  -sm ${E_SUM_MIN} \
  -sM ${E_SUM_MAX} \
  ${tmp_dir}/se82_2nubb.genbb \
    | cut -d ' ' -f 3 \
    | gsl-histogram 0 4000 200 \
    > ${tmp_dir}/se82_bb2nu.data

cat> ${tmp_dir}/test_bb0nu_channel.gpl <<EOF

set grid
set xlabel "E1+E2 (keV)"
set ylabel "dN/d(E1+E2)"
plot \
  "${tmp_dir}/se82_bb0nu.data" u 1:3 notitle with histeps,\
  "${tmp_dir}/se82_bb2nu.data" u 1:3 notitle with histeps
pause -1 "Hit return..."
EOF

gnuplot ${tmp_dir}/test_bb0nu_channel.gpl 

rm -f ${tmp_dir}/test_bb0nu_channel.gpl 
rm -f ${tmp_dir}/se82_0nubb_mn.genbb
rm -f ${tmp_dir}/se82_bb0nu.data 
rm -f ${tmp_dir}/se82_bb2nu.data 

exit 0

# end
