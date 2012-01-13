#!/usr/bin/env bash

tmp_dir=/tmp/${USER}
tmp_dir=.

#test -d ${tmp_dir} || mkdir -p ${tmp_dir} 

if [ ! -f ${tmp_dir}/se82_0nubb_mn.genbb ]; then
    genbb ${GENBB_HELP_ROOT}/testing/config/se82_0nubb_mn.conf ${RANDOM} 3000 ${tmp_dir}/se82_0nubb_mn.genbb
fi
if [ ! -f ${tmp_dir}/se82_2nubb.genbb ]; then
    genbb ${GENBB_HELP_ROOT}/testing/config/se82_2nubb.conf ${RANDOM} 20000 ${tmp_dir}/se82_2nubb.genbb
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
test_bb0nu_channel \
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

echo "DEVEL: Processing 'se82_2nubb.genbb'..." >&2
test_bb0nu_channel \
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
