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

### depends on Endian:
#DEPCPPFLAGS+=-I$(PACKAGE_DIR)/boost/vault/endian
###DEPLDFLAGS+=-lcurses

### depends on Floating point utility:
FPU_FLAGS=
ifeq ($(DATATOOLS_NO_FPU),)
DATATOOLS_NO_FPU=0
$(warning WARNING: datatools uses 'Floating point utilities' (FPU) !)
FPU_FLAGS=-DIOFACTORY_USE_FPU=1
##DEPCPPFLAGS+=-I$(PACKAGE_DIR)/boost/vault/fpu
else
DATATOOLS_NO_FPU=1
$(warning WARNING: datatools does not use FPU !)
endif

### Use or not Embedded Boost I/O:
EBIO_FLAGS=
ifeq ($(DATATOOLS_NO_EBIO),)
DATATOOLS_NO_EBIO=0
$(warning WARNING: datatools uses 'Embedded Boost I/O' !)
EBIO_FLAGS=
else
DATATOOLS_NO_EBIO=1
EBIO_FLAGS=-DDATATOOLS_NO_EBIO=1
$(warning WARNING: datatools does not use 'Embedded Boost I/O' !)
endif

### Use Portable Binary Archive :
PBA_FLAGS=
ifeq ($(DATATOOLS_NO_PBA),)
DATATOOLS_NO_PBA=0
$(warning WARNING: datatools uses 'Portable Binary Archive' !) 
PBA_FLAGS=-DIOFACTORY_USE_PBA=1
else
DATATOOLS_NO_PBA=1
$(warning WARNING: datatools does not use PBA !)
endif

### End of dependencies.
CPPFLAGS1+=$(PBA_FLAGS) $(FPU_FLAGS) $(EBIO_FLAGS) $(DEPCPPFLAGS) 
LDFLAGS1+=$(DEPLDFLAGS) 

# default with libmath dependency:
LDFLAGS1+=-lm 

CPPFLAGS+=$(shell echo $(CPPFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py) 
LDFLAGS+=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 

CXXFLAGSSHFLAGS=-Wall -v
STATICFLAGS=-static

SHFLAGS=$(LDFLAGS) -ldl 

HEADEREXT=.h
HEADEREXTPP=.hpp
SOURCEEXT=.cc
SOURCEEXTPP=.cpp
PROGEXT=.cxx
OBJEXT=.o
LIBEXT=.a
BINEXT=

.SUFFIXES : $(HEADEREXT) $(HEADEREXTPP) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT)

vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(HEADEREXTPP)  $(INCDIR)/$(PACKAGENAME)
vpath %$(SOURCEEXT)  $(SRCDIR)
vpath %$(SOURCEEXTPP)  $(SRCDIR)
vpath %$(PROGEXT)    $(PROGDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)
vpath %$(BINEXT)     $(BINDIR)

headers := $(notdir $(shell find $(INCDIR)/$(PACKAGENAME) -name "*$(HEADEREXT)" )) quasi_portable_binary_iarchive.hpp quasi_portable_binary_oarchive.hpp
sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))

### If uncommented, this code force my_data.cc to be the last in library 
# due to initialization of a static scoped pointer or similar stuff:
sources1 := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null | sed -e "s@/base_service.cc@/@g"))
sources := $(sources1) base_service.cc

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
	@echo "NOTICE: Target 'build_special'..." >&2

.PHONY : install
install : 
	@echo "NOTICE: Installing..." >&2
	@#cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@#cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@#ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
	@echo "NOTICE: Installing done." >&2

#	#@test "x$(DATATOOLS_NO_PBA)" = "x1" && make install_no_pba || make install_pba

# .PHONY : install_eos_pba
# install_eos_pba :  
# 	@echo "NOTICE: Installing with EOS_PBA..." >&2
# 	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
# 	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
# 	@test -d $(BINDIR) || mkdir -p $(BINDIR)
# 	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
# 	@echo "NOTICE: Installing with EOS_PBA done." >&2

# .PHONY : install_no_eos_pba
# install_no_eos_pba : 
# 	@echo "NOTICE: Installing without EOS_PBA..." >&2
# 	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" -e "s@NO_EOS_PBA 0@NO_EOS_PBA 1@g" > $(BASEDIR)/$(LIBNAME).csh
# 	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" -e "s@NO_EOS_PBA=0@NO_EOS_PBA=1@g" > $(BASEDIR)/$(LIBNAME).sh
# 	@test -d $(BINDIR) || mkdir -p $(BINDIR)
# 	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
# 	@echo "NOTICE: Installing without EOS_PBA done." >&2

#########################################################################

.PHONY : clean_special
clean_special :
	@echo "NOTICE: Target 'clean_special'..." >&2
	rm -f my_bag.conf my_bag_of_bags.conf
	rm -f test_data_serialization.*_
	rm -f test_multi_properties_*.conf
	rm -f test_nans_ar.*
	rm -f test_properties.*
	rm -f test_serialization*_

.PHONY : uninstall
uninstall : 
	@echo "NOTICE: Uninstalling..." >&2
	@#test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	@#test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	@#test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""
	@echo "NOTICE: Uninstalling done." >&2

#########################################################################

.PHONY : system
system :
	@echo "$(SYSTEM)"

#.PHONY : setup
#setup :
#	@echo "export PATH=$(PACKAGE_DIR)/$(SYSTEM)/bin:$(PATH)"
#	@echo "export LD_LIBRARY_PATH=$(PACKAGE_DIR)/$(SYSTEM)/lib:$(LD_LIBRARY_PATH)"

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
bins : bin_shared bin_static 

.PHONY : bin_static
bin_static : $(binaries_static)
	@echo "NOTICE: Building static binaries done." >&2

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
	@test -n "$(objectsWithPath)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) -o $(LIBDIR)/$(notdir $(@)) || echo ""
	@echo "NOTICE: Building shared lib '$(@)' done." >&2
	@echo

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) 
	@echo "NOTICE: Compiling module '$(@)'..." >&2
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

%.static$(BINEXT) : %$(PROGEXT) | lib_static
	@echo "NOTICE: Compiling statically linked program '$(@)'..." >&2
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. -I$(PROGDIR) $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

### 2009-11-12 FM: remove --shared flags (produce segmentation fault)
%$(BINEXT) : %$(PROGEXT) | lib_shared 
	@echo "NOTICE: Compiling program '$(@)' with shared library..." >&2
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. -I$(PROGDIR) $(CPPFLAGS) $(CXXFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

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
