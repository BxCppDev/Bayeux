# -*- mode: make; -*-
# 
# Author: F. Mauger <mauger@lpccaen.in2p3.fr>
# Date:   2009-03-10
#
# Copyright (c) 2009 François Mauger
#

base_dir=..
use_fpu=0
use_eos_pba=0

ifneq ($(USE_FPU),)
use_fpu=1
endif

ifneq ($(USE_EOS_PBA),)
use_fpu=1
use_eos_pba=1
endif

CXXFLAGS=-fPIC
ifeq ($(use_fpu),1)
CPPFLAGS+=-DIOFACTORY_USE_FPU=1      # using `Floating point utility' by Johan Rade
endif

ifeq ($(use_eos_pba),1)
CPPFLAGS+=-DIOFACTORY_USE_EOS_PBA=1  # using `Portable Binary Archive' 
                                     # by Christian Pfligersdorffer
endif

CPPFLAGS+=-I.                    

# To scan the `boost' subdirectory with io_factory 
# and vault contribution if needed:
CPPFLAGS+=-I${base_dir}/include 

ifeq ($(use_eos_pba),1)
CPPFLAGS+=-I${base_dir}/include/eos_pba                    
endif

# Set cpp flags and link options aware of Boost pathes.
# IMHO the Boost library lacks such a `boost-config'
# script that enables to get compiler flags.
# Please change the following lines to suit to your system:
CPPFLAGS+=$(shell boost-config --cflags)
# Typical output is:
#   -I/opt/boost/pro/include/boost-1_34_1 
#   -I/usr/include/python2.5 
#   -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall 
#CPPFLAGS+=<your own stuff>

LDFLAGS=$(shell boost-config --ldflags serialization iostreams filesystem)
# Typical output is:
#   -fPIC -L/opt/boost/pro/lib 
#   -lboost_serialization-gcc42 
#   -lboost_iostreams-gcc42 
#   -lboost_filesystem-gcc42
#LDFLAGS+=<your own stuff>

.PHONY : all
all : test_rw_0 

.PHONY : obj
obj : io_factory.o

io_factory.o : ${base_dir}/src/io_factory.cc 
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