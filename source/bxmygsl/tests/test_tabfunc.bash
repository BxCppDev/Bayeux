#!/bin/bash
# -*- mode: shell-script; -*-

test_tabfunc > tf.data

cat>tf.gpl<<EOF

set grid
set title "mygsl: test of the 'tabulated_function' class"
set xrange [0:12]
set yrange [*:*]
set xlabel "x"
set ylabel "f(x)"

plot \
 'tf.data' index 0 using 1:2 title "Linear interpolation" with lines, \
 '' index 1 using 1:2 title "C-spline interpolation" with lines, \
 '' index 2 using 1:2 title "Sampled points" with points pt 7 ps 0.75, \
 '' index 3 using 1:2 title "Theroretical" with lines
pause -1

EOF

gnuplot tf.gpl
rm -f tf.data tf.gpl

# end
