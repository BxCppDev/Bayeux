# -*- mode: gnuplot; -*-

set title "genvtx -- example ex01"

set xlabel "x (mm)"
set ylabel "y (mm)"
set zlabel "z (mm)"
set size ratio -1
set view equal xyz
#set xyplane at 0

set xrange [-100:+100]
set yrange [-100:+100]
set zrange [-50:+100]

splot "ex01_vertices.data" title "Generated vertex in the bulk PCB of all mezzanine boards" with dots
pause -1 "Hit [Enter]..."

# end