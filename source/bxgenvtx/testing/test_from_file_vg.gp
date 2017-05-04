# -*- mode: gnuplot; -*-
#
# Plot the result of the test_from_file_vg sample program:
#

! ${GENVTX_BIN_DIR}/test_from_file_vg -d > test_from_file_vg.data

set title "genvtx::from_file_vg test program"
set xrange [-3:+3]
set yrange [-3:+3]
set zrange [-3:+3]

set xlabel "x"
set ylabel "y"
set zlabel "z"
splot \
      'test_from_file_vg.data' index 0 using 1:2:3 title "From file" with dots

pause -1 "Hit [Enter]..."

! rm -f test_from_file_vg.data

# end 