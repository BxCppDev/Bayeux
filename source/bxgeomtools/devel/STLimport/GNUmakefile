#CPPFLAGS=$(shell boost-config --cflags)
#LDFLAGS=$(shell boost-config --ldflags)
CPPFLAGS=$(shell geomtools-config --cflags)
LDFLAGS=$(shell geomtools-config --ldflags)

.PHONY : all
all : test0

test0 : test0.cxx
	@echo "CPPFLAGS=$(CPPFLAGS)"
	$(CXX) $(CPPFLAGS) $< $(LDFLAGS) -o $@

PHONY : clean
clean : 
	$(RM) *~
	$(RM) test0


# end
