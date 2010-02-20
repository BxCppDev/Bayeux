# -*- mode: makefile; -*- 
# GNUmakefile (language C++)

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

COMPILER=$(CC)$(shell $(CC) --version | head -1 | cut -d ' ' -f 3)
SYSTEM=$(OS)-$(ARCH)
#-$(COMPILER)

.PHONY : all
all : lib

PACKAGENAME=geomtools
UPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "a-z" "A-Z")
LIBNAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
LIBFILE=lib$(LIBNAME).a
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}
LIBVER=$(shell cat ../VERSION |tr -d '\n')

BASEDIR=..
PACKAGE_ROOT=$(shell cd .. ; pwd)

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
ifeq ($(shell which gsl-config),)
$(error Sorry gsl-config not found!)
endif
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

# default with libmath dependency:
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

#CHEADEREXT=.h
#CSOURCEEXT=.c
#CPROGEXT=.c
#COBJEXT=.o

.SUFFIXES : $(HEADEREXT) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT)

vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(SOURCEEXT)  $(SRCDIR)
vpath %$(PROGEXT)    $(PROGDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)
vpath %$(BINEXT)     $(BINDIR)

#vpath %$(CHEADEREXT)  $(INCDIR)/$(PACKAGENAME)
#vpath %$(CSOURCEEXT)  $(SRCDIR)
#vpath %$(COBJEXT)     $(OBJDIR)
#vpath %$(CPROGEXT)    $(PROGDIR)

headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT) 2> /dev/null))

#sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null))
sources1 := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null | sed -e 's/i_object_3d.cc//g'))
### - force i_object_3d.cc to be the last in library 
### - force XXX.cc to be the last in library due to initialization of a static scoped pointer
sources := $(sources1) i_object_3d.cc

#csources := $(notdir $(shell ls $(SRCDIR)/*$(CSOURCEEXT) 2> /dev/null))
objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
#cobjects := $(csources:%$(CSOURCEEXT)=%$(COBJEXT))
#cobjectsWithPath := $(csources:%$(CSOURCEEXT)=$(OBJDIR)/%$(COBJEXT))
programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT) 2> /dev/null))

programs_test:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^test_)
programs:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep -v ^test_)

binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))

binaries_test:= $(programs_test:%$(PROGEXT)=%$(BINEXT))

.PHONY : help
help :
	@echo "GNUmakefile usage "
	@echo "----------------- "
	@echo ""
	@echo "make help      : print this help"
	@echo ""
	@echo "make           : default as 'make lib'"
	@echo "make lib       : build the shared library"
	@echo "make bin       : build the shared production executables"
	@echo "make bin_test  : build the shared test executables (not mandatory)"
	@echo "make install   : generates install stuff"
	@echo "                 - setup ($(LIBNAME).sh/$(LIBNAME).csh)"
	@echo "                 - $(LIBNAME)-config scripts"
	@echo "make uninstall : clean install stuff"
	@echo "make clean     : clean all object/binaries"
	@echo ""
	@echo "make system    : print system"
	@echo "make test      : print some infos"
	@echo ""
	@echo "make dist      : make a source tarball ($(LIBNAME)-$(LIBVER).tar.gz)"
	@echo "make bzdist    : make a source tarball ($(LIBNAME)-$(LIBVER).tar.bz2)"
	@echo ""

.PHONY : test
test:
	@echo "PACKAGE_DIR='$(PACKAGE_DIR)'"
	@echo "PACKAGENAME='$(PACKAGENAME)'"
	@echo "COMPILER='$(COMPILER)'"
	@echo "SYSTEM='$(SYSTEM)'"
	@echo "LIBNAME='$(LIBNAME)'"
	@echo "LIBVER='$(LIBVER)'"
	@echo "INCDIR='$(INCDIR)'"
	@echo "SRCDIR='$(SRCDIR)'"
	@echo "headers = $(headers)"
	@echo "sources = $(sources)"
	@echo "objects = $(objects)"
	@echo "programs = $(programs)"
	@echo "binaries = $(binaries)"
	@echo "programs_test = $(programs_test)"
	@echo "binaries_test = $(binaries_test)"
	@echo "arch dir = $(ARCHDIR)"
	@echo "CPPFLAGS='$(CPPFLAGS)'"
	@echo "LDFLAGS='$(LDFLAGS)'"

#########################################################################

.PHONY : build_special
build_special :
	@echo "Target 'build_special'..."

.PHONY : install
install : 
	@echo "Installing..."
	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config

#########################################################################

.PHONY : clean_special
clean_special :
	@echo "Target 'clean_special'..."

.PHONY : uninstall
uninstall : 
	@echo "Uninstalling..."
	test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""

#########################################################################

.PHONY : system
system :
	@echo "$(SYSTEM)"

.PHONY : setup
setup :
	@echo "export PATH=$(PACKAGE_DIR)/$(SYSTEM)/bin:$(PATH)"
	@echo "export LD_LIBRARY_PATH=$(PACKAGE_DIR)/$(SYSTEM)/lib:$(LD_LIBRARY_PATH)"

.PHONY : lib
lib : lib_shared

.PHONY : libs
libs : lib_static lib_shared

.PHONY : lib_static
lib_static : $(LIBFILE)

.PHONY : lib_shared
lib_shared : $(SHLIBFILE)

.PHONY : clean
clean : clean_special
	@echo "Cleaning..."
	find $(BASEDIR)/ -name "*~" -exec $(RM) \{\} \; 
	test -d $(ARCHDIR)/ && $(RM) -r $(ARCHDIR) || echo -n ""
	@echo "Cleaning done."
	@echo

.PHONY : distclean
distclean : uninstall clean
	@echo "Total cleaning..."
	@echo

.PHONY : bin
bin : bin_shared

.PHONY : bin_test
bin_test : bin_test_shared

.PHONY : bins
bins : bin_shared bin_static 

.PHONY : bin_static
bin_static : $(binaries_static)

.PHONY : bin_shared
bin_shared : $(binaries)

.PHONY : bin_test_shared
bin_test_shared : ${binaries_test}

$(LIBFILE) : $(objectsWithPath) 
	@echo "Building static lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	for file_object in $(objects); do \
	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
	ranlib $(LIBDIR)/$(notdir $(@))
	@echo "Building static lib '$(@)' done."
	@echo

$(SHLIBFILE) : $(objectsWithPath) 
	@echo "Building shared lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	@test -n "$(objectsWithPath)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) -o $(LIBDIR)/$(notdir $(@)) || echo ""
	@echo "Building shared lib '$(@)' done."
	@echo

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) %$(HEADEREXT)
	@echo "Compiling module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

%.static$(BINEXT) : %$(PROGEXT) | lib_static
	@echo "Compiling statically linked program '$(@)'..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared 
	@echo "Compiling program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

.PHONY : dist
dist : distclean
	echo "Making the distribution tarball '$(LIBNAME)-$(LIBVER).tar.gz'"
	make clean
	tar cvzf "../../$(LIBNAME)-$(LIBVER).tar.gz" --exclude ".svn" ../../$(PACKAGENAME)/

.PHONY : bzdist
bzdist : distclean
	echo "Making the distribution tarball '$(LIBNAME)-$(LIBVER).tar.bz2'"
	make clean
	tar cvjf "../../$(LIBNAME)-$(LIBVER).tar.bz2" --exclude ".svn" ../../$(PACKAGENAME)/

# end of GNUmakefile (language C++)
