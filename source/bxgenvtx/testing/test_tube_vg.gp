# -*- mode: gnuplot; -*-
#
# Plot the result of the test_box_vg sample program:
#

## export GENVTX_BIN_DIR=__build-Linux-i686/testing
## gnuplot testing/test_tube_vg.gp

! ${GENVTX_BIN_DIR}/test_tube_vg > test_tube_vg.data

set title "genvtx::tube_vg test program"
set xrange [-5:+5]
set yrange [-5:+5]
set zrange [-5:+5]
# set xrange [*:*]
# set yrange [*:*]
# set zrange [*:*]

set xlabel "x"
set ylabel "y"
set zlabel "z"
splot \
      'test_tube_vg.data' index 0 using 1:2:3 title "Surface" with dots, \
      'test_tube_vg.data' index 1 using 1:2:3 title "Bulk" with dots, \
      'test_tube_vg.data' index 2 using 1:2:3 title "Tube" with lines
pause -1 "Hit [Enter]..."

! rm -f test_tube_vg.data

# end 