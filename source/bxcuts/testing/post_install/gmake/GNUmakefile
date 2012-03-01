#

CPPFLAGS=-I../.. $(shell cuts-config --cflags)
LDFLAGS=$(shell cuts-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_cuts

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_cuts

.PHONY : run
run : test_cuts
	./$< 

test_cuts : ../../test_cuts.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

