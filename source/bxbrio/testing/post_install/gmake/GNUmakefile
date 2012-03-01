#

CPPFLAGS=-I../.. $(shell brio-config --cflags)
LDFLAGS=$(shell brio-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_data_io

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_data_io test_data_io.brio

.PHONY : run
run : test_data_io
	./$< 

test_data_io : ../../test_data_io.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

