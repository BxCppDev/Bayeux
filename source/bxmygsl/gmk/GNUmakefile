# -*- mode: makefile; -*- 
# GNUmakefile (language C++)

SHELL=/bin/bash
OS=$(shell uname -s)
ifneq ($(OS),Linux)
ifneq ($(OS),Darwin)
$(error Sorry only 'Linux' and 'Darwin' are supported!)
endif
endif

ARCH=$(shell uname -m | sed 's/ //g')

MAKE=gmake
ifeq ($(OS),Linux)
  MAKE=make
  CXX=g++
  CC=gcc
  F77=g77
  LD=g++
  LDFLAGS=-O
  SOFLAGS=-shared
  SHLIBEXT=.so
  STATICFLAGS=-static
endif
ifeq ($(OS),Darwin)
  MAKE=make
  CXX=g++
  CC=gcc
  F77=g77
  LD=g++
  LDFLAGS=-O
  SOFLAGS=-dynamiclib -twolevel_namespace -undefined dynamic_lookup -dynamic -single_module
  SHLIBEXT=.dylib
  STATICFLAGS=
endif

SYSTEM=$(OS)-$(ARCH)

all : libs bins build_special

PACKAGENAME=mygsl
UPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "a-z" "A-Z")
LIBNAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
LIBFILE=lib$(LIBNAME).a
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}

BASEDIR=..
PACKAGE_ROOT=$(shell cd .. ; pwd)
#$(shell pwd)/..

PACKAGE_DIR=$(shell cd ${BASEDIR} ; pwd)

INCDIR=$(BASEDIR)/include
SRCDIR=$(BASEDIR)/src
PROGDIR=$(BASEDIR)/programs
SCRIPTSDIR=$(BASEDIR)/scripts
RESDIR=$(BASEDIR)/resources
ARCHDIR=$(BASEDIR)/$(SYSTEM)
OBJDIR=$(ARCHDIR)/obj
LIBDIR=$(ARCHDIR)/lib
BINDIR=$(ARCHDIR)/bin

CXXFLAGS=-fPIC
CPPFLAGS=-I$(INCDIR)
LDFLAGS=-O -L$(LIBDIR) -l$(LIBNAME)

DEPCPPFLAGS=
DEPLDFLAGS=
### Dependencies
#DEP_HOOK

# GSL dependency:
gsl_config_def=$(shell which gsl-config)
gsl_version_def=$(shell $(gsl_config) --version)
gsl_prefix_def=$(shell $(gsl_config) --prefix)
gsl_test_path=$(GSL_BASE_DIR)
#$(warning gsl_test_path=$(gsl_test_path))	
ifneq ($(strip $(GSL_BASE_DIR)),)
gsl_config=$(GSL_BASE_DIR)/bin/gsl-config
$(warning Using special GSL version in $(shell $(gsl_config) --prefix) (from GSL_BASE_DIR=$(GSL_BASE_DIR))!)
gsl_version=$(shell $(gsl_config) --version)
gsl_prefix=$(shell $(gsl_config) --prefix)
else
$(warning Using default GSL version in $(shell $(gsl_config) --prefix)!)
gsl_prefix=$(gsl_prefix_def)
gsl_version=$(gsl_version_def)
gsl_config=$(gsl_config_def)
endif

DEPCPPFLAGS+=$(shell $(gsl_config) --cflags)
DEPLDFLAGS+=$(shell $(gsl_config) --libs)

# CLHEP samples:
#ifeq ($(strip $(CLHEP_BASE_DIR)),)
#$(error Sorry CLHEP is not configured!)
#endif
#ifeq ($(shell which clhep-config),)
#$(error Sorry clhep-config not found!)
#endif
#DEPCPPFLAGS+=$(shell clhep-config --include)
#DEPLDFLAGS+=$(shell clhep-config --libs)

# HDF5 samples:
#ifeq ($(strip $(HDF5_PREFIX)),)
#$(error Sorry HDF5 is not configured!)
#endif
#ifeq ($(shell which hdf5-config),)
#$(error Sorry hdf5-config not found!)
#endif
#DEPCPPFLAGS+=$(shell hdf5-config --include)
#DEPLDFLAGS+=$(shell hdf5-config --libs)

# ROOT samples:
#ifeq ($(strip $(ROOTSYS)),)
#$(warning Sorry ROOT is not configured!)
#else
#$(warning ROOTSYS=$(ROOTSYS))
#endif
#ifeq ($(shell which root-config),)
#$(error Sorry root-config not found!)
#endif
#DEPCPPFLAGS+=$(shell root-config --cflags)
#DEPLDFLAGS+=$(shell root-config --libs)

### End of dependencies
CPPFLAGS+=$(DEPCPPFLAGS) 
LDFLAGS+=$(DEPLDFLAGS) 

LDFLAGS+=-lm 
CXXFLAGSSHFLAGS=-Wall -v
STATICFLAGS=-static

SHFLAGS=$(LDFLAGS) -ldl 

HEADEREXT=.h
SOURCEEXT=.cc
PROGEXT=.cxx
OBJEXT=.o
LIBEXT=.a
BINEXT=

.SUFFIXES : $(HEADEREXT) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT)

vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(SOURCEEXT)  $(SRCDIR)
vpath %$(PROGEXT)    $(PRGDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)
vpath %$(BINEXT)     $(PROGDIR)

headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT)))
sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))
objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
programs:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT)))
binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))

.PHONY : all clean clean_special build_special libs lib_static lib_shared \
  bin bins bin_static bin_shared system test dist install uninstall 

test:
	@echo "Default GSL version='$(gsl_version_def)'"
	@echo "Default GSL prefix='$(gsl_prefix_def)'"
	@echo "Default GSL config='$(gsl_config_def)'"
	@echo "GSL_BASE_DIR='$(GSL_BASE_DIR)'"
	@echo "Current GSL version='$(gsl_version)'"
	@echo "Current GSL config='$(gsl_config)'"
	@echo "Current GSL prefix='$(gsl_prefix)'"
	@echo "PACKAGE_DIR='$(PACKAGE_DIR)'"
	@echo "PACKAGENAME='$(PACKAGENAME)'"
	@echo "LIBNAME='$(LIBNAME)'"
	@echo "SYSTEM='$(SYSTEM)'"
	@echo "INCDIR='$(INCDIR)'"
	@echo "SRCDIR='$(SRCDIR)'"
	@echo "headers = $(headers)"
	@echo "sources = $(sources)"
	@echo "objects = $(objects)"
	@echo "arch dir = $(ARCHDIR)"

#########################################################################"
build_special :
	@echo "build_special..."

install : 
	@echo "installing..."
	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
	@#echo "Please source the '$(PACKAGE_ROOT)/$(LIBNAME).sh' or '$(PACKAGE_ROOT)/$(LIBNAME).csh' from your startup scripts!"

#########################################################################"
clean_special :
	@echo "clean_special..."

uninstall : 
	@echo "uninstalling..."
	test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	@echo "ok1"
	test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	@echo "ok2"
	test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""
	@echo "ok3"
	@#echo "Please remove the sourcing of '$(LIBNAME).sh' or '$(LIBNAME).csh' from your startup scripts!"

#########################################################################"
system :
	@echo "$(SYSTEM)"

setup :
	@echo "export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PACKAGE_DIR)/$(SYSTEM)/lib"

lib : lib_shared

libs : lib_static lib_shared

lib_static : $(LIBFILE)

lib_shared : $(SHLIBFILE)

clean : clean_special
	@echo "Cleaning..."
	find $(BASEDIR)/ -name "*~" -exec $(RM) \{\} \; 
	test -d $(ARCHDIR)/ && $(RM) -r $(ARCHDIR) || echo -n ""

distclean : uninstall clean
	@echo "Total cleaning..."

bin : bin_shared

bins : bin_shared bin_static 

bin_static : $(binaries_static)

bin_shared : $(binaries)

$(LIBFILE) : $(objectsWithPath) 
	@echo "Building static lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	for file_object in $(objects); do \
	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
	ranlib $(LIBDIR)/$(notdir $(@))

$(SHLIBFILE) : $(objectsWithPath) 
	@echo "Building shared lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	$(LD) $(CXXFLAGS) $(SOFLAGS) $(^) -o $(LIBDIR)/$(notdir $(@))

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) %$(HEADEREXT)
	@echo "Compiling module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

%.static$(BINEXT) : %$(PROGEXT) lib_static
	@echo "Compiling statically linked program '$(@)'..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) lib_shared 
	@echo "Compiling program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

dist : distclean
	echo "Making the distribution tarball '$(LIBNAME)-$(shell cat ../VERSION |tr -d '\n').tar.gz'"
	make clean
	tar cvzf "../../$(LIBNAME)-$(shell cat ../VERSION |tr -d '\n').tar.gz" ../../$(PACKAGENAME)/

# end of GNUmakefile (language C++)
