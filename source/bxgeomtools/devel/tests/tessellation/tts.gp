# -*- mode: gnuplot; -*-

! make tts
! ./tts > tts.data

set title "Tessellation test"

set xlabel "x"
set ylabel "y"
set zlabel "z"
set size ratio -1
set view equal xyz
set xyplane at -10
#scansforward
#scansbackward
#scansautomatic
#depthorder
#interpolate 5,5
set pm3d at s depthorder corners2color geomean hidden3d 100
###depthorder corners2color geomean 
#set style data pm3d
#set pm3d hidden3d 100
set style line 100 lt 1 lw 0.5
unset hidden3d
unset surf
set colorbox vertical user front 
#set cbrange[0:10]
#set palette rgb -3,-3,-3
#set palette rgb 10,1,1
#set palette 
#set palette  rgb 7,0,0
set palette model RGB
set palette defined ( \
 0 "yellow"   , \
 10 "orange", \
 11 "blue" , \
 21 "violet )


# set palette defined ( \
#  0 "black"   , \
#  1 "red", \
#  2 "orange", \
#  3 "yellow", \
#  4 "green" , \
#  5 "blue" , \
#  6 "magenta" , \
#  7 "violet"  , \
#  10 "black"   , \
#  11 "dark-grey", \
#  12 "grey" , \
#  13 "light-grey" , \
#  14 "grey90" )

show palette
#test palette
#pause -1 

sz=42.5
set xrange [-sz:sz]
set yrange [-sz:sz]
set zrange [-sz:sz]

splot "tiles3.data" using 1:2:3:4 notitle with pm3d

#replot "tiles3.data" using 1:2:3 notitle with lines
#splot "tts.data" using 1:2:3:4 notitle with pm3d
pause -1 "Hit [Enter]..."

! cat tts.data
! #rm -f tts.data

# end
