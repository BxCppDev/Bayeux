CPPFLAGS=$(shell bayeux-config --cflags)
LDFLAGS=$(shell bayeux-config --ldflags --with-geant4)
BXLIBDIR=$(shell bayeux-config --libdir)
ROOTLIBDIR=$(shell root-config --libdir)
G4LIBDIR=$(shell geant4-config --prefix)/lib

.PHONY : all
all : bxexample01

.PHONY : clean
clean :
	$(RM) bxexample01
	$(RM) bxexample01_brio.brio
	$(RM) bxexample01_datatools.conf

.PHONY : run
run : bxexample01
	LD_LIBRARY_PATH=$(BXLIBDIR):$(ROOTLIBDIR):$(G4LIBDIR):${LD_LIBRARY_PATH} ./bxexample01

bxexample01 : bxexample01.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)
