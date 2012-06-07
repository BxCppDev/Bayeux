# -*- mode: make; -*-
# GNUMakefile

CPPFLAGS=-I../../Export $(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : prog1 prog2

.PHONY : clean
clean :
	$(RM) *~
	$(RM) prog1
	$(RM) prog2
	$(RM) *.data

.PHONY : run
run :	run1 run2
	od -t u1 a.data > a.log
	od -t u1 b.data > b.log
	diff a.log b.log || echo ""

.PHONY : run1
run1 :	prog1
	./$<

.PHONY : run2
run2 :	prog2
	./$<


prog1 : prog1.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

prog2 : prog2.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)


# end of GNUMakefile