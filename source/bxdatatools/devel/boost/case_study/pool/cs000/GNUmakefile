# -*- mode: make; -*-
# 
# Author: F. Mauger <mauger@lpccaen.in2p3.fr>
# Date:   2009-03-10
#
# Copyright (c) 2008 François Mauger
#

CXXFLAGS=-fPIC
CPPFLAGS=-DDATATOOLS_USE_FPU=1   # using `Floating point utility' by Johan Rade
CPPFLAGS+=-DDATATOOLS_USE_PBA=1  # using `Portable Binary Archive' 
                                 # by Christian Pfligersdorffer
CPPFLAGS+=-DBOOST_VERSION=103400 # using Boost 1.34.1

# Set cpp flags and link options aware of Boost path
# (you'd have to change this to suit your system)
CPPFLAGS+=-I.                    
# To scan the `boost' subdirectory with vault contribution

CPPFLAGS+=$(shell boost-config --cflags)
# Typical output is:
#   -I/opt/boost/pro/include/boost-1_34_1 
#   -I/usr/include/python2.5 
#   -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall 

LDFLAGS=$(shell boost-config --ldflags serialization iostreams filesystem)
# Typical output is:
#   -fPIC -L/opt/boost/pro/lib 
#   -lboost_serialization-gcc42 
#   -lboost_iostreams-gcc42 
#   -lboost_filesystem-gcc42

.PHONY : all
all :  test_rw_0 

.PHONY : obj
obj : io_factory.o

io_factory.o : io_factory.cc | io_factory.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@ 

test_rw_0 : test_rw_0.cxx | obj
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< io_factory.o -o $@ $(LDFLAGS)

# be careful with this target!
.PHONY : atom
atom : clean
	$(RM) *.xml
	$(RM) *.txt
	$(RM) *.data
	$(RM) *.gz
	$(RM) *.bz2

.PHONY : clean
clean :
	find . -name "*~" -exec $(RM) \{\} \;
	$(RM) test_rw_0
	$(RM) io_factory.o


# end