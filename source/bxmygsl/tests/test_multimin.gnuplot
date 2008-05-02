# -*- mode: gnuplot; -*- 
# mygsl - test_multimin.gnuplot

#! rm -f test_multimin.his test_multimin.sol
#! time ${MYGSL_ROOT}/Linux-i686/bin/test_multimin -l -d

set title "mygsl - test_multimin"

set grid

#set xrange [0:1024] 
set xrange [100:500] 
set yrange [0:] 
#set yrange [0:50000] 
plot 'test_multimin.hist' using 1:3 title "Data" with steps, \
     'test_multimin.sol' using 1:2 title "Fit" with lines


set term postscript color
set output "test_multimin.ps"
plot 'test_multimin.hist' using 1:3 title "Data" with steps, \
     'test_multimin.sol' using 1:2 title "Fit" with lines
set output
set term x11

#plot 'test_multimin.hist' using 1:3:(sqrt($3)) with errorbars, \
#     'test_multimin.sol' using 1:2 with lines

# end of test_multimin.gnuplot
