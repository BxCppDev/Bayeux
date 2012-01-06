#!/bin/bash
# -*- mode: shell-script; -*-

test_multi_eval > test_multi_eval.data


cat>test_multi_eval.gpl<<EOF

set grid
set title "mygsl: test of the 'multi_eval' class"
set xrange [-6:+6]
set yrange [-4:+4]
set zrange [0:*]
set xlabel "x"
set ylabel "y"
set zlabel "f(x,y)"

splot \
 'test_multi_eval.data' index 0 using 1:2:3 title "hypot (x,y)" with lines lt 1, \
 ''                     index 1 using 1:2:3 title "hypot (3,y)" with lines lt 2 lw 2 , \
 ''                     index 2 using 1:2:3 title "hypot (x,0.5)" with lines lw 2 lt 3 , \
 ''                     index 3 using 1:2:3 title "hypot (x,1.5)" with lines lw 2 lt 4
pause -1

EOF

gnuplot test_multi_eval.gpl
rm -f test_multi_eval.data test_multi_eval.gpl

# end
