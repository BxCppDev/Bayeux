#

CPPFLAGS=-I../.. $(shell emfield-config --cflags)
LDFLAGS=$(shell emfield-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_emfield

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_emfield

.PHONY : run
run : test_emfield
	./$< 

test_emfield : ../../test_emfield.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

