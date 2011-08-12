# -*- mode: makefile; -*-
# GNUmakefile (language C++)

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
  F77=gfortran
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
  F77=gfortran
  LD=g++
  LDFLAGS=-O
  SOFLAGS=-dynamiclib -twolevel_namespace -undefined dynamic_lookup -dynamic -single_module
  SHLIBEXT=.dylib
  STATICFLAGS=
endif

SYSTEM_old=$(OS)-$(ARCH)
###COMPILER=-$(CC)$(shell $(CC) --version | head -1 | cut -d ' ' -f 3)
COMPILER=
SYSTEM=$(OS)-$(ARCH)

.PHONY : all
all : lib

PACKAGENAME=datatools
UPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "a-z" "A-Z")
LPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
LIBNAME=$(LPACKAGENAME)
LIBNAME_BIO=$(LIBNAME)_bio
LIBFILE=lib$(LIBNAME).a
LIBFILE_BIO=lib$(LIBNAME_BIO).a
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}
SHLIBFILE_BIO=lib$(LIBNAME_BIO)${SHLIBEXT}
LIBVER=$(shell cat ../VERSION | tr -d '\n')

BASEDIR=..
PACKAGE_ROOT=$(shell cd .. ; pwd)

PACKAGE_DIR=$(shell cd ${BASEDIR} ; pwd)

INCDIR=$(BASEDIR)/include
SRCDIR=$(BASEDIR)/src
PROGDIR=$(BASEDIR)/programs
TESTDIR=$(BASEDIR)/tests
SCRIPTSDIR=$(BASEDIR)/scripts
RESDIR=$(BASEDIR)/resources
ARCHDIR=$(BASEDIR)/$(SYSTEM)
ARCHDIR_old=$(BASEDIR)/$(SYSTEM_old)
OBJDIR=$(ARCHDIR)/obj
LIBDIR=$(ARCHDIR)/lib
BINDIR=$(ARCHDIR)/bin

CXXFLAGS=-fPIC
CPPFLAGS=-I. -I$(INCDIR)
LDFLAGS=-O -L$(LIBDIR) -l$(LIBNAME)

# python
PYVERSION=$(shell boost-config --python-version)
ifeq ($(shell which python${PYVERSION}),)
$(error Sorry Python ${PYVERSION} not found!)
endif
python_bin=python${PYVERSION}
python_version=$(shell $(python_bin) -V 2>&1 | tr -s ' ' | cut -d' ' -f2 | cut -d'.' -f1,2)
python_config=python$(python_version)-config
python_cflags=$(shell $(python_config) --cflags)
python_ldflags=$(shell $(python_config) --ldflags)

binlibdep=lib

DEPCPPFLAGS=
DEPLDFLAGS=
### Dependencies:
#DEP_HOOK

# boost dependency (from the 'boost_help' package) :
ifeq ($(strip $(BOOST_ROOT)),)
$(error Sorry boost is not configured, please define the BOOST_ROOT env)
endif
ifeq ($(shell which boost-config),)
$(error Sorry boost-config not found!)
endif
DEPCPPFLAGS+=$(shell boost-config --cflags)
DEPLDFLAGS+=$(shell boost-config --ldflags serialization iostreams filesystem)

# CLHEP dependency :
ifeq ($(strip $(shell which clhep-config 2> /dev/null)),)
$(error Sorry CLHEP is not setup, cannot find 'clhep-config')
endif
DEPCPPFLAGS+=$(shell clhep-config --include)
DEPLDFLAGS+=$(shell clhep-config --libs)

### Use or not Embedded Boost I/O:
EBIO_CFLAGS=
EBIO_LDFLAGS=
ifeq ($(DATATOOLS_EBIO),1)
DATATOOLS_EBIO=1
EBIO_CFLAGS=-DDATATOOLS_EBIO=1
$(warning WARNING: datatools uses 'Embedded Boost I/O' !)
else
DATATOOLS_EBIO=0
EBIO_CFLAGS=-DDATATOOLS_EBIO=0
$(warning WARNING: datatools does not use 'Embedded Boost I/O' !)
endif

### Use or not Embedded Boost I/O:
XBIO_CFLAGS=
XBIO_LDFLAGS=
ifeq ($(DATATOOLS_XBIO),1)
DATATOOLS_XBIO=1
XBIO_CFLAGS=-DDATATOOLS_XBIO=1
XBIO_LDFLAGS=-l$(LIBNAME_BIO)
binlibdep=lib_xbio
$(warning WARNING: datatools uses 'Auxiliary Boost I/O' !)
else
DATATOOLS_XBIO=0
XBIO_CFLAGS=-DDATATOOLS_XBIO=0
$(warning WARNING: datatools does not use 'Auxiliary Boost I/O' !)
endif

### Use or not KWSYS (2011-07-09 FM: always use it) :
DEPLDFLAGS+=-lkwsys -lkwsys_c

# 2011-07-09 FM : commented out
# KWSYS_FLAGS=
# ifneq ($(DATATOOLS_NO_KWSYS),1)
# DATATOOLS_NO_KWSYS=0
# DEPLDFLAGS+=-lkwsys -lkwsys_c
# $(warning WARNING: datatools uses 'Embedded kwsys' !)
# else
# DATATOOLS_NO_KWSYS=1
# KWSYS_FLAGS=-DDATATOOLS_NO_KWSYS=1
# $(warning WARNING: datatools does not use 'Embedded kwsys' !)
# endif

### End of dependencies.
CPPFLAGS1+=$(EBIO_CFLAGS) $(XBIO_CFLAGS) $(DEPCPPFLAGS)
LDFLAGS1+=$(DEPLDFLAGS)

# default with libmath dependency:
LDFLAGS1+=-lm

CPPFLAGS+=$(shell echo $(CPPFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py)
# ifneq ($(DATATOOLS_NO_KWSYS),1)
# DEPLDFLAGS_SORTED=-L$(LIBDIR)
# endif
DEPLDFLAGS_SORTED+=-L$(LIBDIR) $(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r)
LDFLAGS+=$(DEPLDFLAGS_SORTED)

# Special additional link flag for BIO DLL and executable :
LIBLDFLAGS_BIO=-L$(LIBDIR) -l$(LIBNAME)
EXELDFLAGS_BIO=-L$(LIBDIR) $(XBIO_LDFLAGS) -l$(LIBNAME)

CXXFLAGSSHFLAGS=-Wall -v
STATICFLAGS=-static

HEADEREXT=.h
HEADEREXTPP=.hpp
SOURCEEXT=.cc
SOURCEEXTPP=.cpp
PROGEXT=.cxx
OBJEXT=.o
LIBEXT=.a
BINEXT=

.SUFFIXES : $(HEADEREXT) $(HEADEREXTPP) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT)

vpath %$(HEADEREXT)   $(INCDIR)/$(PACKAGENAME)
vpath %$(HEADEREXTPP) $(INCDIR)/$(PACKAGENAME)
vpath %$(SOURCEEXT)   $(SRCDIR)
vpath %$(SOURCEEXTPP) $(SRCDIR)
vpath %$(PROGEXT)     $(PROGDIR)
vpath test_%$(PROGEXT) $(TESTDIR)
vpath %$(OBJEXT)      $(OBJDIR)
vpath %$(LIBEXT)      $(LIBDIR)
vpath %$(SHLIBEXT)    $(LIBDIR)
vpath %$(BINEXT)      $(BINDIR)

headers := $(notdir $(shell find $(INCDIR)/$(PACKAGENAME) -name "*$(HEADEREXT)" ))
sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))

### If uncommented, this code force base_service.cc to be the last in library
# due to initialization of a static scoped pointer or similar stuff:
sources1 := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null | sed -e "s@/base_service.cc@/@g" -e "s@/the_serializable.cc@/@g"))

### the_serializable.cc has a specizal treatment :
ifeq ($(DATATOOLS_EBIO),1)
sources := the_serializable.cc $(sources1)
else
sources := $(sources1)
endif
sources += base_service.cc
sources_bio :=  $(notdir $(shell ls $(SRCDIR)/the_serializable$(SOURCEEXT) 2> /dev/null))

### -e "s@/library_loader.cc@/@g"

# ifneq ($(DATATOOLS_NO_KWSYS),1)
# # $(warning WARNING: datatools uses the 'library_loader' class !)
# sources := $(sources) library_loader.cc
# endif
objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
objects_bio := $(sources_bio:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath_bio := $(sources_bio:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))

programs_all := $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT) 2> /dev/null))
programs_test := $(notdir $(shell ls $(TESTDIR)/*$(PROGEXT) 2> /dev/null))
#programs_test := $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^test_)
#programs_kwsys_test := $(notdir $(shell ls $(TESTDIR)/^kwsys_test_*$(PROGEXT) 2> /dev/null))
#programs_kwsys_test := $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^kwsys_test_)
programs := $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep -v ^test_)

binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
#binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))
binaries_test:= $(programs_test:%$(PROGEXT)=%$(BINEXT))

.PHONY : help
help :
	@echo "GNUmakefile usage "
	@echo "----------------- "
	@echo ""
	@echo "make help       : print this help"
	@echo ""
	@echo "make            : default as 'make lib'"
	@echo "make lib        : build the shared library"
	@echo "make bin        : build the shared production executables"
	@echo "make bin_test   : build the shared test executables (not mandatory)"
	@echo "make install    : generates install stuff"
	@echo "                  - setup ($(LIBNAME).sh/$(LIBNAME).csh)"
	@echo "                  - $(LIBNAME)-config scripts"
	@echo "make uninstall  : clean install stuff"
	@echo "make clean      : clean all object/binaries"
	@echo "make clean_test : clean test binaries"
	@echo ""
	@echo "make system     : print system"
	@echo "make test       : print some infos"
	@echo ""
	@echo "make dist       : make a source tarball ($(LIBNAME)-$(LIBVER).tar.gz)"
	@echo "make bzdist     : make a source tarball ($(LIBNAME)-$(LIBVER).tar.bz2)"
	@echo ""

.PHONY : dump
dump:
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
	@echo "sources_bio = $(sources_bio)"
	@echo "objects_bio = $(objects_bio)"
	@echo "programs = $(programs)"
	@echo "binaries = $(binaries)"
	@echo "programs_test = $(programs_test)"
	#@echo "kwsys_programs_test = $(kwsys_programs_test)"
	@echo "binaries_test = $(binaries_test)"
	@echo "arch dir = $(ARCHDIR)"
	@echo "CPPFLAGS='$(CPPFLAGS)'"
	@echo "LDFLAGS='$(LDFLAGS)'"

#########################################################################

.PHONY : build_special
build_special :
	@echo "NOTICE: Target 'build_special'..." >&2

.PHONY : install
install :
	@echo "NOTICE: Installing..." >&2
	@#cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@#cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@#ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
	@echo "NOTICE: Installing done." >&2

#########################################################################

.PHONY : clean_special
clean_special :
	@echo "NOTICE: Target 'clean_special'..." >&2

.PHONY : uninstall
uninstall :
	@echo "NOTICE: Nothing special..." >&2

#########################################################################

.PHONY : system
system :
	@echo "$(SYSTEM)"

.PHONY : lib_xbio
lib_xbio : lib lib_bio

.PHONY : lib
lib : lib_shared

.PHONY : lib_bio
lib_bio : lib_shared_bio

.PHONY : libs
libs : lib_static lib_shared

.PHONY : lib_static
lib_static : $(LIBFILE)

.PHONY : lib_shared
lib_shared : $(SHLIBFILE)
	@#echo "SHLIBFILE=$(SHLIBFILE)"

.PHONY : lib_shared_bio lib_shared
lib_shared_bio : $(SHLIBFILE_BIO)
	@#echo "SHLIBFILE_BIO=$(SHLIBFILE_BIO)"

.PHONY : clean
clean : clean_special
	@echo "NOTICE: Cleaning..." >&2
	find $(BASEDIR)/ -name "*~" -exec $(RM) \{\} \;  || echo -n ""
	test -d $(ARCHDIR)/ && $(RM) -r $(ARCHDIR) || echo -n ""
	@echo "NOTICE: Cleaning done." >&2

.PHONY : clean_bin_test
clean_bin_test :
	find $(ARCHDIR)/bin -name "test_*" -a -executable -exec $(RM) \{\} \;  || echo -n ""

.PHONY : distclean
distclean : uninstall clean
	@echo "NOTICE: Total cleaning..." >&2

.PHONY : bin
bin : bin_shared

.PHONY : bin
bin_test : bin_test_shared

.PHONY : bins
bins : bin_shared #bin_static

# .PHONY : bin_static
# bin_static : $(binaries_static)
# 	@echo "NOTICE: Building static binaries done." >&2

.PHONY : bin_shared
bin_shared : $(binaries)
	@echo "NOTICE: Building shared binaries done." >&2
	@echo

.PHONY : bin_test_shared
bin_test_shared : $(binaries_test)

$(LIBFILE) : $(objectsWithPath)
	@echo "NOTICE: Building static lib '$(@)'..." >&2
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	for file_object in $(objects); do \
	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
	ranlib $(LIBDIR)/$(notdir $(@))
	@echo "NOTICE: Building static lib '$(@)' done." >&2

$(SHLIBFILE) : $(objectsWithPath)
	@echo "NOTICE: Building shared lib '$(@)'..." >&2
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	@test -n "$(objectsWithPath)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(DEPLDFLAGS_SORTED) -o $(LIBDIR)/$(notdir $(@)) || echo ""
	@echo "NOTICE: Building shared lib '$(@)' done." >&2
	@echo

$(SHLIBFILE_BIO) : $(objectsWithPath_bio)
	@echo "NOTICE: Building shared auxiliary BIO lib '$(@)'..." >&2
	@#echo "NOTICE: DEPLDFLAGS_SORTED=$(DEPLDFLAGS_SORTED)" >&2
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	@test -n "$(objectsWithPath_bio)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(LIBLDFLAGS_BIO) $(DEPLDFLAGS_SORTED) -o $(LIBDIR)/$(notdir $(@)) || echo ""
	@echo "NOTICE: Building shared lib '$(@)' done." >&2
	@echo

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT)
	@echo "NOTICE: Compiling module '$(@)'..." >&2
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

# %.static$(BINEXT) : %$(PROGEXT) | lib_static
# 	@echo "NOTICE: Compiling statically linked program '$(@)'..." >&2
# 	@test -d $(BINDIR) || mkdir -p $(BINDIR)
# 	$(CXX) -I. -I$(PROGDIR) $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

test_%$(BINEXT) : test_%$(PROGEXT) | lib_shared $(binlibdep)
	@echo "NOTICE: EXELDFLAGS_BIO=$(EXELDFLAGS_BIO)" >&2
	@echo "NOTICE: Compiling test program '$(@)' with shared library..." >&2
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. -I$(TESTDIR) $(CPPFLAGS) $(CXXFLAGS) $< $(EXELDFLAGS_BIO) $(DEPLDFLAGS_SORTED) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared $(binlibdep)
	@echo "NOTICE: EXELDFLAGS_BIO=$(EXELDFLAGS_BIO)" >&2
	@echo "NOTICE: Compiling program '$(@)' with shared library..." >&2
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. -I$(PROGDIR) $(shell $(python_config) --cflags) $(CPPFLAGS) $(CXXFLAGS) $< $(EXELDFLAGS_BIO) $(DEPLDFLAGS_SORTED) -o $(BINDIR)/$(notdir $(@))

.PHONY : dist
dist : distclean
	@echo "NOTICE: Making the distribution tarball '$(LIBNAME)-$(LIBVER).tar.gz'" >&2
	make clean
	tar cvzf "../../$(LIBNAME)-$(LIBVER).tar.gz" --exclude ".svn" ../../$(PACKAGENAME)/

.PHONY : bzdist
bzdist : distclean
	@echo "NOTICE: Making the distribution tarball '$(LIBNAME)-$(LIBVER).tar.bz2'" >&2
	make clean
	tar cvjf "../../$(LIBNAME)-$(LIBVER).tar.bz2" --exclude ".svn" ../../$(PACKAGENAME)/

# end of GNUmakefile (language C++)
