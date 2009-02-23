.PHONY : all
all : gamma_gsl gamma_cern

CXXFLAGS1=$(shell gsl-config --cflags) $(shell clhep-config --include)
LDFLAGS1=$(shell gsl-config --libs) $(shell clhep-config --libs)

gamma_gsl : gamma_gsl.cxx
	$(CXX) $(CXXFLAGS1) $< -o $@ $(LDFLAGS1) 

gamma_cern : gamma_cern.f
	$(F77) $< -o $@  $(shell cernlib -safe -u mathlib) 

.PHONY : clean
clean :
	$(RM) *~
	$(RM) *.data	
	$(RM) gamma_gsl	
	$(RM) gamma_cern

#