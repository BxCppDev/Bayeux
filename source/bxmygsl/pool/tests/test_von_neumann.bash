#!/bin/bash
# -*- mode: shell-script; -*-

test_von_neumann  > vnm.data

cat vnm.data | gsl-histogram -1 4 100 > vnm.his

cat>vnm.gpl<<EOF

set grid
set xrange [-1:4]
set yrange [0:*]
set xlabel "x"
set ylabel "Counts"

plot 'vnm.his' using 1:3 with histeps
pause -1

EOF

gnuplot vnm.gpl
rm -f vnm.data vnm.his vnm.gpl

# end
