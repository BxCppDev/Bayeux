# -*- mode: gnuplot; -*-

set grid
set view equal xyz
set xlabel "x"
set ylabel "y"
set zlabel "z"
set xrange [-1:+1]
set yrange [-1:+1]
set zrange [-1:+1]
set title "original data points"
splot \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.orig.data" index 0 title "" with dots lt -1, \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.orig.data" index 1 title "" with dots lt 3, \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.orig.data" index 2 title "" with dots lt 2, \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.orig.data" index 3 title "" with dots lt 1
pause -1 'Hit [Return]...'

set title "selected data points"
splot \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.selected.data" index 0 title "" with dots lt -1, \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.selected.data" index 1 title "" with dots lt 3, \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.selected.data" index 2 title "" with dots lt 2, \
  "<cat ${BAYEUX_DEV_BUILD_DIR}/test_manager.selected.data" index 3 title "" with dots lt 1
pause -1 'Hit [Return]...'

# end