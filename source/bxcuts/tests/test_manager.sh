#!/usr/bin/env bash

if [ "x${CUTS_ROOT}" = "x" ]; then
    echo "ERROR: Missing CUTS_ROOT variable ! No setup for 'cuts' ! Abort !" >&2
    exit 1
fi

./$(cuts-config --system)/bin/test_manager ${CUTS_ROOT}/tests/config/test_cut_manager.conf # "x1|y2"

cat>tmp.gp<<EOF

set grid
set xlabel "x"
set ylabel "y"
set zlabel "z"
set xrange [-1:+1]
set yrange [-1:+1]
set zrange [-1:+1]
set title "original data points"
splot \
  "test_manager.orig.data" index 0 title "" with dots lt -1, \
  "test_manager.orig.data" index 1 title "" with dots lt 3, \
  "test_manager.orig.data" index 2 title "" with dots lt 2, \
  "test_manager.orig.data" index 3 title "" with dots lt 1
pause -1 'Hit [Return]...'

set title "selected data points"
splot \
  "test_manager.selected.data" index 0 title "" with dots lt -1, \
  "test_manager.selected.data" index 1 title "" with dots lt 3, \
  "test_manager.selected.data" index 2 title "" with dots lt 2, \
  "test_manager.selected.data" index 3 title "" with dots lt 1
pause -1 'Hit [Return]...'
EOF

gnuplot tmp.gp

rm -f tmp.gp
rm -f test_manager.orig.data test_manager.selected.data

exit 0

# end
