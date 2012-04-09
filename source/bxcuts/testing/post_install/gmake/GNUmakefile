#

CPPFLAGS=-I../.. $(shell cuts-config --cflags)
LDFLAGS=$(shell cuts-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_manager

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_manager

.PHONY : run
run : test_manager
	./$< 

test_manager : ../../test_manager.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

