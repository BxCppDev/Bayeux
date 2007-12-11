# mygsl - test_histogram.gnuplot

set title "mygsl - test_histogram"
set grid

plot [0:10.] \
  'test_histogram.data'  using 1:3 title "h" with steps , \
  'test_histogram2.data' using 1:3 title "h+" with steps 

#
# 'test_histogram3.data' using 1:3 title "h rebin" with steps

# end

