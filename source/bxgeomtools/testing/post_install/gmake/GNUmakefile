#

CPPFLAGS=-I../.. $(shell geomtools-config --cflags)
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
	$(RM) test_serializable_2
	$(RM) test_things.xml

.PHONY : run
run : run_1 run_2
	./$< 

.PHONY : run_1
run_1 : test_placement
	./$< 

.PHONY : run_2
run_2 : test_serializable_2
	./$< 

test_placement : ../../test_placement.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

test_serializable_2 : ../../test_serializable_2.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

