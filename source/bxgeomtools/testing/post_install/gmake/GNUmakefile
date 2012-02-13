#

CPPFLAGS=-I.. $(shell geomtools-config --cflags)
LDFLAGS=$(shell geomtools-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_placement

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_placement

.PHONY : run
run : test_placement
	./$< 

test_placement : ../test_placement.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

