# -*- mode: gnuplot; -*- 
# mygsl - test_histogram.gnuplot

set title "mygsl - test_histogram"
set grid

plot [0:10.] \
  'test_histogram.data'  using 1:3 title "h" with steps , \
  'test_histogram2.data' using 1:3 title "h+" with steps , \
  'test_histogram3.data' using 1:3 title "h2 rebin" with steps , \
  'test_histogram4.data' using 1:3 title "h3 rebin" with steps , \
  'test_histogram5.data' using 1:3 title "hh3" with steps , \
  'test_histogram6.data' using 1:3 title "hh4" with steps
pause 5

# end of test_histogram.gnuplot

