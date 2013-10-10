# -*- mode: gnuplot; -*-
#
# Plot the result of the test_sphere_vg sample program:
#

! ${GENVTX_BIN_DIR}/test_sphere_vg -d > test_sphere_vg.data

set title "genvtx::sphere_vg test program"
set xrange [-3:+3]
set yrange [-3:+3]
set zrange [-3:+3]

set size ratio -1
set view equal xyz
set xyplane at -3.5

set xlabel "x"
set ylabel "y"
set zlabel "z"
splot \
      'test_sphere_vg.data' index 0 using 1:2:3 title "Sphere" with lines, \
      'test_sphere_vg.data' index 1 using 1:2:3 title "Side surface" with dots, \
      'test_sphere_vg.data' index 2 using 1:2:3 title "Bulk" with dots, \
      'test_sphere_vg.data' index 3 using 1:2:3 title "Other" with dots
pause -1 "Hit [Enter]..."

! rm -f test_sphere_vg.data

# end