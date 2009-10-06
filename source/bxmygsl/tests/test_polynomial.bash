#!/bin/bash
# -*- mode: shell-script; -*-

test_polynomial > test_polynomial.data


cat>test_polynomial.gpl<<EOF

set grid
set title "mygsl: test of the 'polynomial' and related classes"
set xrange [0:10]
set yrange [*:*]
set xlabel "x"
set ylabel "P(x)"

plot \
 'test_polynomial.data' index 0 using 1:2 title "p0(x)" with lines, \
 ''                     index 1 using 1:2 title "p1(x)" with lines, \
 ''                     index 2 using 1:2 title "p2(x)" with lines
pause -1

EOF

gnuplot test_polynomial.gpl
rm -f test_polynomial.data test_polynomial.gpl

# end
