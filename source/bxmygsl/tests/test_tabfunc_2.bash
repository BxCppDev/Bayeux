#!/bin/bash
# -*- mode: shell-script; -*-

test_tabfunc_2 > tf2.data

cat>tf2.gpl<<EOF

set grid
set size ratio -1
set title "mygsl: test of the 'tabulated_function' class"
set xrange [-0.25:4.5]
set yrange [-0.25:3.5]
set xlabel "x"
set ylabel "f(x)"

plot \
 'tf2.data' index 0 using 1:2 title "F(x) with interpolation" with lines, \
 ''        index 1 using 1:2 title "F(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 2 using 1:2 title "F2(x) interpolation" with lines, \
 ''        index 3 using 1:2 title "F2(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 4 using 1:2 title "F3(x) interpolation" with lines, \
 ''        index 5 using 1:2 title "F3(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 6 using 1:2 title "F3bis(x) interpolation" with lines, \
 ''        index 7 using 1:2 title "F3bis(x) Sampled points" with points pt 7 ps 0.75
pause -1

EOF

gnuplot tf2.gpl

(echo "set terminal postscript solid color; load 'tf2.gpl'" ) | gnuplot > tf2.ps 
gv tf2.ps &
rm -f tf2.data tf2.gpl 

# end
