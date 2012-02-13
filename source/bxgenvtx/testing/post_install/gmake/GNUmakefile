#

CPPFLAGS=-I../.. $(shell genvtx-config --cflags)
LDFLAGS=$(shell genvtx-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_genvtx

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_genvtx

.PHONY : run
run : test_genvtx
	./$< 

test_genvtx : ../../test_genvtx.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

