# -*- mode: gnuplot; -*-
! __build-Linux-x86_64/testing/test_single_particle_generator -c -m > test_spg.data

set grid
set size ratio 1

set xrange [-2:+2]
set yrange [-2:+2]
set zrange [-2:+2]

set xlabel "x"
set ylabel "y"
set zlabel "z"

set title "Test single particle generator"
splot 'test_spg.data'  u (0.0):(0.0):(0.0):($2):($3):($4) title "Random direction" with vectors
pause -1 "Hit [Enter]..."

# end
