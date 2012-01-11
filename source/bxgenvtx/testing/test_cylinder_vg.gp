# -*- mode: gnuplot; -*-
#
# Plot the result of the test_box_vg sample program:
#

! ${GENVTX_BIN_DIR}/test_cylinder_vg > test_cylinder_vg.data

set title "genvtx::cylinder_vg test program"
set xrange [-5:+5]
set yrange [-5:+5]
set zrange [-5:+5]

set xlabel "x"
set ylabel "y"
set zlabel "z"
splot \
      'test_cylinder_vg.data' index 0 using 1:2:3 title "Surface" with dots, \
      'test_cylinder_vg.data' index 1 using 1:2:3 title "Bulk" with dots, \
      'test_cylinder_vg.data' index 2 using 1:2:3 title "Cylinder" with lines
pause -1 "Hit [Enter]..."

! rm -f test_cylinder_vg.data

# end 