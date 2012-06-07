# -*- mode: make; -*-
# GNUMakefile

CPPFLAGS=-I. -I../../Export $(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : progs progs_alt

.PHONY : clean
clean :
	$(RM) *~
	$(RM) prog1
	$(RM) prog2
	$(RM) prog1_alt
	$(RM) prog2_alt
	$(RM) *.data
	$(RM) *.xml
	$(RM) *.log

.PHONY : run
run : progs run.sh 
	./run.sh

.PHONY : progs
progs : prog1 prog2	

prog1 : prog1.cxx a.hpp b.hpp
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

prog2 : prog2.cxx a.hpp b.hpp
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

.PHONY : progs_alt
progs_alt : prog1_alt prog2_alt	


prog1_alt : prog1.cxx a.hpp b.hpp
	$(CXX) -DA_CLASS_ALT -DB_CLASS_ALT $(CPPFLAGS) $< -o $@ $(LDFLAGS)

prog2_alt  : prog2.cxx a.hpp b.hpp
	$(CXX) -DA_CLASS_ALT -DB_CLASS_ALT $(CPPFLAGS) $< -o $@ $(LDFLAGS)


# end of GNUMakefile