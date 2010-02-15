# -*- mode: gnuplot; -*-
#
# Plot the result of the test_box_vg sample program:
#

! ${GENVTX_BIN_DIR}/test_box_vg > test_box_vg.data

set title "genvtx::box_vg test program"
set xrange [-3:+3]
set yrange [-3:+3]
set zrange [-3:+3]

set xlabel "x"
set ylabel "y"
set zlabel "z"
splot \
      'test_box_vg.data' index 0 using 1:2:3 title "Surface" with dots, \
      'test_box_vg.data' index 1 using 1:2:3 title "Bulk" with dots, \
      'test_box_vg.data' index 2 using 1:2:3 title "Box" with lines
pause -1 "Hit [Enter]..."

! rm -f test_box_vg.data

# end 