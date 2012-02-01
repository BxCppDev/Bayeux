#

CPPFLAGS=-I.. $(shell materials-config --cflags)
LDFLAGS=$(shell materials-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_materials

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_materials

.PHONY : run
run : test_materials
	./$< 

test_materials : ../test_materials.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

