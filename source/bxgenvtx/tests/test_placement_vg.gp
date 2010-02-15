# -*- mode: gnuplot; -*-
#
# Plot the result of the test_placement_vg sample program:
#

! ${GENVTX_BIN_DIR}/test_placement_vg > test_placement_vg.data

set title "genvtx::placement_vg test program"
set xrange [-2:+8]
set yrange [-2:+8]
set zrange [-2:+8]

set xlabel "x"
set ylabel "y"
set zlabel "z"

scale=3
set arrow 1 from 0,0,0 to scale*1,0,0 head front lt 1
set arrow 2 from 0,0,0 to 0,scale*1,0 head front lt 2
set arrow 3 from 0,0,0 to 0,0,scale*1 head front lt 3

splot \
      'test_placement_vg.data' index 0 using 1:2:3 title "Placement" with lines, \
      'test_placement_vg.data' index 1 using 1:2:3 title "Surface" with dots

pause -1 "Hit [Enter]..."

unset arrow
! rm -f test_placement_vg.data

# end 