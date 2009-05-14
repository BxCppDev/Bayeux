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

SYSTEM_old=$(OS)-$(ARCH)
###COMPILER=-$(CC)$(shell $(CC) --version | head -1 | cut -d ' ' -f 3)
COMPILER=
SYSTEM=$(OS)-$(ARCH)

.PHONY : all
all : lib

PACKAGENAME=datatools
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
ARCHDIR_old=$(BASEDIR)/$(SYSTEM_old)
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

# boost dependency (from the 'boost_help' package) :
ifeq ($(strip $(BOOST_ROOT)),)
$(error Sorry boost is not configured; please define the BOOST_ROOT env)
endif
ifeq ($(shell which boost-config),)
$(error Sorry boost-config not found!)
endif
DEPCPPFLAGS+=$(shell boost-config --cflags)
DEPLDFLAGS+=$(shell boost-config --ldflags serialization iostreams filesystem)

### depends on Endian:
DEPCPPFLAGS+=-I$(PACKAGE_DIR)/boost/vault/endian

### depends on Floating point utility:
DATATOOLS_NO_FPU=$(shell ls -1 $(PACKAGE_DIR)/gmk/datatools_without_fpu.config 2> /dev/null)
FPU_FLAGS=
ifeq ($(DATATOOLS_NO_FPU),)
DATATOOLS_NO_FPU=0
$(warning WARNING: datatools uses 'Floating point utilities' (FPU)!)
FPU_FLAGS=-DIOFACTORY_USE_FPU=1
DEPCPPFLAGS+=-I$(PACKAGE_DIR)/boost/vault/fpu
else
DATATOOLS_NO_FPU=1
$(warning WARNING: datatools does not use FPU!)
endif


DATATOOLS_NO_EOS_PBA=$(shell ls -1 $(PACKAGE_DIR)/gmk/datatools_without_eos_pba.config 2> /dev/null)
EOS_PBA_FLAGS=
ifeq ($(DATATOOLS_NO_EOS_PBA),)
DATATOOLS_NO_EOS_PBA=0
$(warning WARNING: datatools uses 'Portable Binary Archive' (PBA from eos)!) 
EOS_PBA_FLAGS=-DIOFACTORY_USE_EOS_PBA=1
DEPCPPFLAGS+=-I$(PACKAGE_DIR)/boost/vault/eos_pba
else
DATATOOLS_NO_EOS_PBA=1
$(warning WARNING: datatools does not use EOS_PBA!)
endif

CPPFLAGS1+=-DDATATOOLS_USE_TREE_DUMP=1

### End of dependencies.
CPPFLAGS1+=$(EOS_PBA_FLAGS) $(FPU_FLAGS) $(DEPCPPFLAGS) 
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

.SUFFIXES : $(HEADEREXT) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT)

vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(SOURCEEXT)  $(SRCDIR)
vpath %$(PROGEXT)    $(PROGDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)
vpath %$(BINEXT)     $(BINDIR)

headers := $(notdir $(shell find $(INCDIR)/$(PACKAGENAME) -name "*$(HEADEREXT)" ))
sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))
objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))

##headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT) 2> /dev/null))
#headers := $(notdir $(shell find $(INCDIR)/$(PACKAGENAME) -name "*$(HEADEREXT)" ))
#sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))
#sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null))
#objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
#objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))


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
	@#cd $(BASEDIR)/boost/vault; make 

.PHONY : install
install : 
	@test "x$(DATATOOLS_NO_EOS_PBA)" = "x1" && make install_no_eos_pba || make install_eos_pba

.PHONY : install_eos_pba
install_eos_pba :  
	@echo "installing with EOS_PBA..."
	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config

	@echo "installing with EOS_PBA done."
	@echo ""

.PHONY : install_no_eos_pba
install_no_eos_pba : 
	@echo "installing without EOS_PBA..."
	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" -e "s@NO_EOS_PBA 0@NO_EOS_PBA 1@g" > $(BASEDIR)/$(LIBNAME).csh
	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" -e "s@NO_EOS_PBA=0@NO_EOS_PBA=1@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config

	@echo "installing without EOS_PBA done."
	@echo ""

#########################################################################

.PHONY : clean_special
clean_special :
	@echo "Target 'clean_special'..."
	@#cd $(BASEDIR)/boost/vault; make clean

.PHONY : uninstall
uninstall : 
	@echo "Uninstalling..."
	test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""
	@echo "uninstalling done."
	@echo ""

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

.PHONY : bin
bin_test : bin_test_shared

.PHONY : bins
bins : bin_shared bin_static 

.PHONY : bin_static
bin_static : $(binaries_static)
	@echo "Building static binaries done."
	@echo

.PHONY : bin_shared
bin_shared : $(binaries)
	@echo "Building shared binaries done."
	@echo

.PHONY : bin_test_shared
bin_test_shared : $(binaries_test)

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

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) 
	@echo "Compiling module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

%.static$(BINEXT) : %$(PROGEXT) | lib_static
	@echo "Compiling statically linked program '$(@)'..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. -I$(PROGDIR) $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared 
	@echo "Compiling program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. -I$(PROGDIR) $(CPPFLAGS) $(CXXFLAGS) $(SOFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

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
