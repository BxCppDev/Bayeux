CPPFLAGS=$(shell bayeux-config --cflags)
LDFLAGS=$(shell bayeux-config --ldflags --with-geant4)
BXLIBDIR=$(shell bayeux-config --libdir)
ROOTLIBDIR=$(shell root-config --libdir)
G4LIBDIR=$(shell geant4-config --prefix)/lib

.PHONY : all
all : bxmctoolsg4

.PHONY : clean
clean :
	$(RM) bxmctoolsg4

.PHONY : run
run : bxmctoolsg4
	LD_LIBRARY_PATH=$(BXLIBDIR):$(ROOTLIBDIR):$(G4LIBDIR):${LD_LIBRARY_PATH} ./bxmctoolsg4

bxmctoolsg4 : bxmctoolsg4.cc
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)
