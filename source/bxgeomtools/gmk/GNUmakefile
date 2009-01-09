# -*- mode: makefile; -*- 
# GNUmakefile (language C++)

###SHELL=/bin/bash
OS=$(shell uname -s)
ifneq ($(OS),Linux)
ifneq ($(OS),Darwin)
$(error Sorry only 'Linux' and 'Darwin' are supported!)
endif
endif

ARCH=$(shell uname -m | sed 's/ //g')
#ifeq ($(ARCH),Power Macintosh)
#ARCH=PowerMacintosh
#endif

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

all : lib

###all : libs bin build_special

PACKAGENAME=geomtools
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
CPPFLAGS=-I. -I$(INCDIR)
LDFLAGS=-O -L$(LIBDIR) -l$(LIBNAME)

DEPCPPFLAGS=
DEPLDFLAGS=
### Dependencies:
#DEP_HOOK

# GSL dependency:
#ifeq ($(shell which gsl-config),)
#$(error Sorry gsl-config not found!)
#endif
DEPCPPFLAGS+=$(shell gsl-config --cflags)
DEPLDFLAGS+=$(shell gsl-config --libs)

# CLHEP dependency:
ifeq ($(strip $(CLHEP_BASE_DIR)),)
$(error Sorry CLHEP is not configured!)
endif
ifeq ($(shell which clhep-config),)
$(error Sorry clhep-config not found!)
endif
DEPCPPFLAGS+=$(shell clhep-config --include)
DEPLDFLAGS+=$(shell clhep-config --libs)

# datatools:
ifeq ($(shell which datatools-config),)
$(error Sorry datatools-config not found!)
endif
DEPCPPFLAGS+=$(shell datatools-config --cflags)
DEPLDFLAGS+=$(shell datatools-config --ldflags)

### End of dependencies.
CPPFLAGS1+=$(DEPCPPFLAGS) 
LDFLAGS1+=$(DEPLDFLAGS) 
LDFLAGS1+=-lm 

CPPFLAGS+=$(shell echo $(CPPFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py) 
LDFLAGS+=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 

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
	@echo "CPPFLAGS='$(CPPFLAGS)'"
	@echo "LDFLAGS='$(LDFLAGS)'"

#########################################################################"
build_special :
	@echo "Target 'build_special'..."

install : 
	@echo "Installing..."
	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
	@#echo "Please source the '$(PACKAGE_ROOT)/$(LIBNAME).sh' or '$(PACKAGE_ROOT)/$(LIBNAME).csh' from your startup scripts!"

#########################################################################"
clean_special :
	@echo "Target 'clean_special'..."

uninstall : 
	@echo "Uninstalling..."
	test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	#@echo "ok1"
	test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	#@echo "ok2"
	test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""
	#@echo "ok3"
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
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) lib_shared 
	@echo "Compiling program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

dist : distclean
	echo "Making the distribution tarball '$(LIBNAME)-$(shell cat ../VERSION |tr -d '\n').tar.gz'"
	make clean
	tar cvzf "../../$(LIBNAME)-$(shell cat ../VERSION |tr -d '\n').tar.gz" ../../$(PACKAGENAME)/

# end of GNUmakefile (language C++)
