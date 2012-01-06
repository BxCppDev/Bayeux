#!/bin/bash
# -*- mode: shell-script; -*-

test_tabfunc_3 > tf3.data

cat>tf3.gpl<<EOF

set grid
set size ratio -1
set title "mygsl: test of the 'tabulated_function' class"
set xrange [-0.25:3.5]
set yrange [-0.75:2.5]
set xlabel "x"
set ylabel "f(x)"

plot \
 'tf3.data' index 0 using 1:2 title "F(x) with interpolation" with lines, \
 ''        index 1 using 1:2 title "F(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 2 using 1:2 title "F2(x) interpolation" with lines, \
 ''        index 3 using 1:2 title "F2(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 4 using 1:2 title "F3(x) interpolation" with lines, \
 ''        index 5 using 1:2 title "F3(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 6 using 1:2 title "F3bis(x) interpolation" with lines, \
 ''        index 7 using 1:2 title "F3bis(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 8 using 1:2 title "F_outer(x) interpolation" with lines, \
 ''        index 9 using 1:2 title "F_inner(x) interpolation" with lines
pause -1

plot \
 'tf3.data' index 1 using 1:2 title "F(x) Sampled points" with points pt 7 ps 0.75, \
 ''        index 8 using 1:2 title "F_outer(x) interpolation" with lines, \
 ''        index 9 using 1:2 title "F_inner(x) interpolation" with lines
pause -1

EOF

gnuplot tf3.gpl

(echo "set terminal postscript solid color; load 'tf3.gpl'" ) | gnuplot > tf3.ps 
gv tf3.ps &
rm -f tf3.data tf3.gpl 

# end
