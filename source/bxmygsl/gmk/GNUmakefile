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

SYSTEM=$(OS)-$(ARCH)

.PHONY : all
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
gsl_version=$(shell $(gsl_config) --version)
gsl_prefix=$(shell $(gsl_config) --prefix)
$(warning NOTICE: Using special GSL version $(gsl_version) in $(gsl_prefix) (from GSL_BASE_DIR=$(GSL_BASE_DIR))!)
else
gsl_prefix=$(gsl_prefix_def)
gsl_version=$(gsl_version_def)
gsl_config=$(gsl_config_def)
$(warning NOTICE: Using default GSL version $(gsl_version) in $(gsl_prefix)!)
endif

DEPCPPFLAGS+=$(shell $(gsl_config) --cflags)
DEPLDFLAGS+=$(shell $(gsl_config) --libs)

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
vpath %$(PROGEXT)    $(PROGDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)
vpath %$(BINEXT)     $(BINDIR)

headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT)))
sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))
objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))

programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT) 2> /dev/null))

programs_test:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^test_)
programs:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep -v ^test_)

binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))

binaries_test:= $(programs_test:%$(PROGEXT)=%$(BINEXT))
#programs:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT)))
#binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
#binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))

.PHONY : all clean clean_special build_special libs lib_static lib_shared \
  bin bins bin_static bin_shared system test dist install uninstall 

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
	@echo "programs = $(programs)"
	@echo "binaries = $(binaries)"
	@echo "programs_test = $(programs_test)"
	@echo "binaries_test = $(binaries_test)"
	@echo "arch dir = $(ARCHDIR)"
	@echo "CPPFLAGS='$(CPPFLAGS)'"
	@echo "LDFLAGS='$(LDFLAGS)'"

########################################################################
.PHONY : build_special
build_special :
	@#echo "Build_special..."

.PHONY : install
install : 
	@echo "Installing..."
	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
	@#echo "Please source the '$(PACKAGE_ROOT)/$(LIBNAME).sh' or '$(PACKAGE_ROOT)/$(LIBNAME).csh' from your startup scripts!"

#########################################################################"
.PHONY : clean_special
clean_special :
	@echo "Clean_special..."

.PHONY : uninstall
uninstall : 
	@echo "Uninstalling..."
	test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""

#########################################################################"
.PHONY : system
system :
	@echo "$(SYSTEM)"

.PHONY : setup
setup :
	@echo "export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PACKAGE_DIR)/$(SYSTEM)/lib"

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

.PHONY : clean_bin_test
clean_bin_test : 
	find $(ARCHDIR)/bin -name "test_*" -a -executable -exec $(RM) \{\} \; 

.PHONY : distclean
distclean : uninstall clean
	@echo "Total cleaning..."

.PHONY : bin
bin : bin_shared

.PHONY : bin
bin_test : bin_test_shared

.PHONY : bins
bins : bin_shared bin_static 

.PHONY : bin_static
bin_static : $(binaries_static)

.PHONY : bin_shared
bin_shared : $(binaries)

.PHONY : bin_test_shared
bin_test_shared : $(binaries_test)

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

%.static$(BINEXT) : %$(PROGEXT) | lib_static
	@echo "Compiling statically linked program '$(@)'..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared 
	@echo "Compiling program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

.PHONY : dist
dist : distclean
	@echo "Making the distribution tarball '$(LIBNAME)-$(shell cat ../VERSION |tr -d '\n').tar.gz'"
	make clean
	tar cvzf "../../$(LIBNAME)-$(shell cat ../VERSION |tr -d '\n').tar.gz" ../../$(PACKAGENAME)/

.PHONY : bzdist
bzdist : distclean
	@echo "Making the distribution tarball '$(LIBNAME)-$(LIBVER).tar.bz2'"
	make clean
	tar cvjf "../../$(LIBNAME)-$(LIBVER).tar.bz2" --exclude ".svn" ../../$(PACKAGENAME)/

# end of GNUmakefile (language C++)
