#!/bin/bash
# -*- mode: shell-script; -*-

echo "Test linear_regression ..."
test_linear_regression > test_linear_regression.data

cat>test_linear_regression.gpl<<EOF

set grid
set title "mygsl: test of the 'linear_regression' class"
set xrange [0:10]
set yrange [0:12]
set xlabel "x"
set ylabel "f(x)"

#index 0 using 1:2 title "Datapoints" with points pt 6 ps 1.0  lt 1, \\

plot \\
 'test_linear_regression.data' index 0 using 1:2:3 title "Datapoints" with errorbars lt 1, \\
 ''                            index 1 using 1:2 title "Weighted/unweighted Least-squares fit (WLS/ULS)" with lines, \\
 ''                            index 2 using 1:(column(2)+column(3)) title "Upper confidence ULS fit" with lines lt 4, \\
 ''                            index 2 using 1:(column(2)-column(3)) title "Lower confidence ULS fit" with lines lt 4, \\
 ''                            index 4 using 1:(column(2)+column(3)) title "Upper confidence WLS fit" with lines lt 5, \\
 ''                            index 4 using 1:(column(2)-column(3)) title "Lower confidence WLS fit" with lines lt 5
pause -1

EOF

gnuplot test_linear_regression.gpl

rm -f test_linear_regression.data test_linear_regression.gpl

# end
