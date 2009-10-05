#!/bin/bash
# -*- mode: shell-script; -*-

test_unary_eval > test_unary_eval.data


cat>test_unary_eval.gpl<<EOF

set grid
set title "mygsl: test of the 'unary_eval' and related classes"
set xrange [-pi:+pi]
set yrange [-1.5:+1.5]
set xlabel "x"
set ylabel "f(x)"

plot \
 'test_unary_eval.data' using 1:2 title "cos (x)" with lines, \
 '' using 1:(column(3)+0.02) title "cos (x)" with lines, \
 '' using 1:(column(4)+0.10) title "cos'(x)" with lines, \
 '' using 1:(column(5)+0.04) title "cos (x)" with lines, \
 '' using 1:(column(6)+0.12) title "cos'(x) [numeric]" with lines, \
 '' using 1:(column(7)+0.06) title "cos (x) [native]" with lines, \
 '' using 1:(column(8)+0.14) title "-sin (x) [native]" with lines
pause -1

EOF

gnuplot test_unary_eval.gpl
rm -f test_unary_eval.data test_unary_eval.gpl

# end
