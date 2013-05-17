#!/usr/bin/env bash

opwd=$(pwd)
cd ..
export CUTS_DATA_DIR=$(pwd)

cd ${opwd}
if [ "x${CUTS_SRC_ROOT}" = "x" ]; then
    cd ..
    export CUTS_SRC_ROOT=$(pwd)
fi
cd ${opwd}

arch=$(uname -s)-$(uname -m)
if [ ! -x  ${CUTS_SRC_ROOT}/__build-${arch}/testing/test_manager ]; then
    echo "ERROR: Test executable is not built ! Abort !" >&2
    exit 1
fi

CUTS_TESTING_CONFIG_DIR=${CUTS_DATA_DIR}/testing/config
if [ ! -d ${CUTS_TESTING_CONFIG_DIR} ]; then
    export CUTS_TESTING_CONFIG_DIR=${CUTS_SRC_ROOT}/testing/config
fi

${CUTS_SRC_ROOT}/__build-${arch}/testing/test_manager -o -m -m -m -m -m -m ${CUTS_TESTING_CONFIG_DIR}/test_cut_manager.conf # "x1|y2"
if [ $? -ne 0 ]; then
    echo "ERROR: Test executable failed ! Abort !" >&2
    exit 1
fi
tmp_dir=/tmp/${USER}/cuts_testing.d
if [ ! -d ${tmp_dir} ]; then
    mkdir -p  ${tmp_dir}
fi

mv test_manager.orig.data test_manager.selected.data ${tmp_dir}/
cat>${tmp_dir}/tmp.gp<<EOF

set grid
set xlabel "x"
set ylabel "y"
set zlabel "z"
set xrange [-1:+1]
set yrange [-1:+1]
set zrange [-1:+1]
set title "original data points"
splot \
  "${tmp_dir}/test_manager.orig.data" index 0 title "" with dots lt -1, \
  "${tmp_dir}/test_manager.orig.data" index 1 title "" with dots lt 3, \
  "${tmp_dir}/test_manager.orig.data" index 2 title "" with dots lt 2, \
  "${tmp_dir}/test_manager.orig.data" index 3 title "" with dots lt 1
pause -1 'Hit [Return]...'

set title "selected data points"
splot \
  "${tmp_dir}/test_manager.selected.data" index 0 title "" with dots lt -1, \
  "${tmp_dir}/test_manager.selected.data" index 1 title "" with dots lt 3, \
  "${tmp_dir}/test_manager.selected.data" index 2 title "" with dots lt 2, \
  "${tmp_dir}/test_manager.selected.data" index 3 title "" with dots lt 1
pause -1 'Hit [Return]...'
EOF

gnuplot ${tmp_dir}/tmp.gp

rm -f ${tmp_dir}/tmp.gp
rm -f ${tmp_dir}/test_manager.orig.data ${tmp_dir}/test_manager.selected.data

exit 0

# end
