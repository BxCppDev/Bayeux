

CFLAGS=$(shell boost-config --cflags)
LDFLAGS=$(shell boost-config --ldflags serialization)

.PHONY : all
all : p1

.PHONY : clean
clean : 
	$(RM) *~
	$(RM) p1	


p1 : p1.cxx	
	$(CXX) $(CFLAGS) $< $(LDFLAGS) -o $@