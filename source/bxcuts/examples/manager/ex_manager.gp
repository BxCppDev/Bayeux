# -*- mode: gnuplot; -*-

set grid
set xlabel "x"
set ylabel "y"
set zlabel "z"
set xrange [-1:+1]
set yrange [-1:+1]
set zrange [-1:+1]

set title "Original data points"
splot \
  "ex_manager.orig.data" index 0 title "" with dots lt -1, \
  "ex_manager.orig.data" index 1 title "" with dots lt 3, \
  "ex_manager.orig.data" index 2 title "" with dots lt 2, \
  "ex_manager.orig.data" index 3 title "" with dots lt 1
pause -1 'Hit [Enter]...'

set title "Selected data points"
splot \
  "ex_manager.selected.data" index 0 title "" with dots lt -1, \
  "ex_manager.selected.data" index 1 title "" with dots lt 3, \
  "ex_manager.selected.data" index 2 title "" with dots lt 2, \
  "ex_manager.selected.data" index 3 title "" with dots lt 1
pause -1 'Hit [Enter]...'


# end