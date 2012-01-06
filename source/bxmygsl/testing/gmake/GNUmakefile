#

CPPFLAGS=-I.. $(shell mygsl-config --cflags)
LDFLAGS=$(shell mygsl-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_histogram

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_histogram
	$(RM) test_histogram*.data
	$(RM) test_histogram.store

.PHONY : run
run : test_histogram
	./$< 
	gnuplot test_histogram.gp

test_histogram : ../test_histogram.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

