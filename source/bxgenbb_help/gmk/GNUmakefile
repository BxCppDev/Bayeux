# -*- mode: makefile; -*- 
# GNUmakefile (language C++/F77)

test_bash=$(shell which bash)
ifeq ($(test_bash),)
$(error Sorry bash is not available on your system!)
endif

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

# Choices: F77_COMP=g77 or F77_COMP=gfortran
ifneq ($(GENBB_HELP_F77_COMP),)
F77=$(GENBB_HELP_F77_COMP)
$(warning Force $(GENBB_HELP_F77_COMP) compiler!)
endif

COMPILER=$(CC)$(shell $(CC) --version | head -1 | cut -d ' ' -f 3)
SYSTEM=$(OS)-$(ARCH)

.PHONY : all
all : lib

PACKAGENAME=genbb_help
UPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "a-z" "A-Z")
LPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
LIBNAME=$(LPACKAGENAME)
LIBNAME_BIO=$(LIBNAME)_bio
LIBNAMEF=$(LIBNAME)_f77
LIBFILE=lib$(LIBNAME).a
LIBFILE_BIO=lib$(LIBNAME_BIO).a
LIBFILEF=lib$(LIBNAME)_f77.a
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}
SHLIBFILE_BIO=lib$(LIBNAME_BIO)${SHLIBEXT}
SHLIBFILE_F77=lib$(LIBNAME)_f77${SHLIBEXT}

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
OBJDIR=$(ARCHDIR)/obj
LIBDIR=$(ARCHDIR)/lib
BINDIR=$(ARCHDIR)/bin

FPICFLAG=-fPIC
CXXFLAGS=$(FPICFLAG)
F77FLAGS=$(FPICFLAG)

test_f77=$(shell which $(F77))
ifeq ($(test_f77),)
$(error no $(F77) is available!)
endif

ifeq ($(F77),g77)
F77FLAGS+=-fno-automatic -finit-local-zero -Wno-globals -fno-globals
F77FLAGS+=-fno-second-underscore
endif

ifeq ($(F77),gfortran)
F77FLAGS+=
endif

processor=$(shell uname -p)
ifeq ($(strip $(processor)),x86_64)
dummy=$(shell echo "Processor: 64-bits")
else
#CXXFLAGS+= -Df2cFortran
dummy=$(shell echo "Processor: 32-bits")
endif

binlibdep=lib

###########################################################################
DEPCPPFLAGS_CXX=
DEPLDFLAGS_CXX=
DEPCPPFLAGS_F77=
DEPLDFLAGS_F77=

### Dependencies:
#DEP_HOOK

# geomtools dependency:
ifeq ($(shell which geomtools-config),)
$(error Sorry geomtools-config not found!)
endif
DEPCPPFLAGS_CXX+=$(shell geomtools-config --cflags)
DEPLDFLAGS_CXX+=$(shell geomtools-config --ldflags)

# f2c dependency:
ifeq ($(F77),g77)
DEPLDFLAGS_CXX+=-lg2c
endif

ifeq ($(F77),gfortran)
DEPLDFLAGS_CXX+=-lgfortran
endif

### Use or not Embedded Boost I/O:
EBIO_CFLAGS=
EBIO_LDFLAGS=
ifeq ($(GENBB_HELP_EBIO),1)
GENBB_HELP_EBIO=1
EBIO_CFLAGS=-DGENBB_HELP_EBIO=1
$(warning WARNING: genbb_help uses 'Embedded Boost I/O' !)
else
GENBB_HELP_EBIO=0
EBIO_CFLAGS=-DGENBB_HELP_EBIO=0
$(warning WARNING: genbb_help does not use 'Embedded Boost I/O' !)
endif

### Use or not Embedded Boost I/O:
XBIO_CFLAGS=
XBIO_LDFLAGS=
ifeq ($(GENBB_HELP_XBIO),1)
GENBB_HELP_XBIO=1
XBIO_CFLAGS=-DGENBB_HELP_XBIO=1
XBIO_LDFLAGS=-l$(LIBNAME_BIO)
binlibdep=lib_xbio
$(warning WARNING: genbb_help uses 'Auxiliary Boost I/O' !)
else
GENBB_HELP_XBIO=0
XBIO_CFLAGS=-DGENBB_HELP_XBIO=0
$(warning WARNING: genbb_help does not use 'Auxiliary Boost I/O' !)
endif

$(warning WARNING: genbb_help EBIO_FLAGS=$(EBIO_CFLAGS)) 
$(warning WARNING: genbb_help XBIO_FLAGS=$(XBIO_CFLAGS)) 

# mygsl dependency :
DEPCPPFLAGS_F77+=$(shell mygsl-config --cflags)  
DEPLDFLAGS_F77+=$(shell mygsl-config --ldflags)  

DEPCPPFLAGS_CXX+=$(shell mygsl-config --cflags)  
DEPLDFLAGS_CXX+=$(shell mygsl-config --ldflags)  

### End of dependencies.
###########################################################################
DEPCPPFLAGS_CXX+=$(EBIO_CFLAGS) $(XBIO_CFLAGS) $(DEPCPPFLAGS) 

CPPFLAGS_DEFFUNC1=
CPPFLAGS_DEFFUNC2=
# Force only one underscore for extern Fortran77/C+ names:
underscore_mode=one
$(warning Force only one underscore for extern Fortran77/C++ names)
ifeq ($(underscore_mode),one)
CPPFLAGS_DEFFUNC1+=-Dfermi_wrap=fermi_wrap_
CPPFLAGS_DEFFUNC1+=-Drng_shoot_reset=rng_shoot_reset_
CPPFLAGS_DEFFUNC1+=-Drng_shoot_flat=rng_shoot_flat_

CPPFLAGS_DEFFUNC2+=-Dgenbbsub=genbbsub_
CPPFLAGS_DEFFUNC2+=-Dset_genbb_random_seed=set_genbb_random_seed_
CPPFLAGS_DEFFUNC2+=-Dgrndm=grndm_
CPPFLAGS_DEFFUNC2+=-Dgenbb_init_enrange=genbb_init_enrange_
CPPFLAGS_DEFFUNC2+=-Dgenevent=genevent_
CPPFLAGS_DEFFUNC2+=-Denrange=enrange_
endif

### F77 code:
DEPCPPFLAGS_F77_SORTED=$(shell echo $(DEPCPPFLAGS_F77) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py) 
CPPFLAGS_F77=-I$(INCDIR)
CPPFLAGS_F77+=$(CPPFLAGS_DEFFUNC1)
CPPFLAGS_F77+=$(DEPCPPFLAGS_F77_SORTED) 

DEPLDFLAGS_F77_SORTED=$(shell echo $(DEPLDFLAGS_F77) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 
LDFLAGS_F77=-O -L$(LIBDIR) -l$(LIBNAMEF)
LDFLAGS_F77+=$(DEPLDFLAGS_F77_SORTED)

$(warning WARNING: genbb_help DEPCPPFLAGS_F77_SORTED=$(DEPCPPFLAGS_F77_SORTED))  
$(warning WARNING: genbb_help DEPLDFLAGS_F77_SORTED=$(DEPLDFLAGS_F77_SORTED)) 

### C++ code:
DEPCPPFLAGS_CXX_SORTED=$(shell echo $(DEPCPPFLAGS_CXX) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py) 
CPPFLAGS_CXX=-I$(INCDIR)
CPPFLAGS_CXX+=$(CPPFLAGS_DEFFUNC1)
CPPFLAGS_CXX+=$(CPPFLAGS_DEFFUNC2)
CPPFLAGS_CXX+=$(DEPCPPFLAGS_CXX_SORTED) 

DEPLDFLAGS_CXX_SORTED=$(shell echo $(DEPLDFLAGS_CXX) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 
LDFLAGS_CXX=-O -L$(LIBDIR) -l$(LIBNAME) -l$(LIBNAMEF)
LDFLAGS_CXX+=$(DEPLDFLAGS_CXX_SORTED)

# Special additional link flag for BIO DLL and executable :
LIBLDFLAGS=-L$(LIBDIR) -l$(LIBNAMEF)  
LIBLDFLAGS_BIO=-L$(LIBDIR) -l$(LIBNAME) -l$(LIBNAMEF)  
EXELDFLAGS_BIO=-L$(LIBDIR) $(XBIO_LDFLAGS) -l$(LIBNAME) -l$(LIBNAMEF)    

#########################################################################################

HEADEREXT=.h
SOURCEEXT=.cc
PROGEXT=.cxx
OBJEXT=.o

F77_HEADEREXT=.inc
F77_SOURCEEXT=.f
F77_OBJEXT=.f77.o
F77_PROGEXT=.F
F77_BINEXT=.f77.exe

LIBEXT=.a
BINEXT=

.SUFFIXES : $(HEADEREXT) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT) $(F77_HEADEREXT) $(F77_SOURCEEXT) $(F77_OBJEXT) $(F77_BINEXT) 

vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(SOURCEEXT)  $(SRCDIR)
vpath %$(PROGEXT)    $(PROGDIR)
vpath test_%$(PROGEXT)    $(TESTDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(BINEXT)     $(BINDIR)

vpath %$(F77_HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(F77_SOURCEEXT)  $(SRCDIR)
vpath %$(F77_OBJEXT)     $(OBJDIR)
vpath %$(F77_PROGEXT)    $(PROGDIR)
vpath test_%$(F77_PROGEXT)    $(TESTDIR)
vpath %$(F77_BINEXT)     $(BINDIR)

vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)

headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT)))
sources1 := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))
sources1 := $(shell echo $(sources1) | sed -e 's/rng.cc//g' -e 's/fermi.cc//g' -e 's/the_serializable.cc//g')

# standard mode : 
ifeq ($(GENBB_HELP_EBIO),0)
sources := $(sources1)
else
# only for GENBB_HELP_EBIO==1 mode :
sources := the_serializable.cc $(sources1)
endif

sources_bio :=  $(notdir $(shell ls $(SRCDIR)/the_serializable$(SOURCEEXT) 2> /dev/null))

objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))

objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
objects_bio := $(sources_bio:%$(SOURCEEXT)=%$(OBJEXT)) 
objectsWithPath_bio := $(sources_bio:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT)) 

programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT) 2> /dev/null))
programs_test:= $(notdir $(shell ls $(TESTDIR)/test_*$(PROGEXT) 2> /dev/null))
#programs_test:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^test_)
programs:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep -v ^test_)
binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
#binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))
binaries_test:= $(programs_test:%$(PROGEXT)=%$(BINEXT))

crossed_headers := fermi.h rng.h
crossed_sources := $(notdir $(shell ls $(SRCDIR)/fermi.cc $(SRCDIR)/rng.cc))
crossed_objects := $(crossed_sources:%$(SOURCEEXT)=%$(OBJEXT))
crossed_objectsWithPath := $(crossed_sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))

f77_headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(F77_HEADEREXT)))
f77_sources := $(notdir $(shell ls $(SRCDIR)/*$(F77_SOURCEEXT)))
f77_objects := $(f77_sources:%$(F77_SOURCEEXT)=%$(F77_OBJEXT))
f77_objectsWithPath := $(f77_sources:%$(F77_SOURCEEXT)=$(OBJDIR)/%$(F77_OBJEXT))
f77_programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(F77_PROGEXT) 2> /dev/null))
f77_programs_test:= $(notdir $(shell ls $(TESTDIR)/test_*$(F77_PROGEXT) 2> /dev/null))
#f77_programs_test:= $(shell echo $(f77_programs_all) | tr ' ' '\n' | grep .F$ | grep ^test_)
f77_programs:= $(shell echo $(f77_programs_all) | tr ' ' '\n' | grep .F$ | grep -v ^test_)
f77_binaries:= $(f77_programs:%$(F77_PROGEXT)=%$(F77_BINEXT))
f77_binaries_test:= $(f77_programs_test:%$(F77_PROGEXT)=%$(F77_BINEXT))

#################################################################################"

.PHONY : help
help :
	@echo "GNUmakefile usage "
	@echo "----------------- "
	@echo ""
	@echo "make help      : print this help"
	@echo ""
	@echo "make           : default as 'make lib'"
	@echo "make lib       : build the shared library"
	@echo "make bin       : build the shared executables"
	@echo "make install   : generates install stuff"
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
	@echo "LIBDIR='$(LIBDIR)'"
	@echo ""
	@echo "C++ headers = $(headers)"
	@echo ""
	@echo "C++ sources = $(sources)"
	@echo ""
	@echo "C++ objects = $(objects)"
	@echo ""
	@echo "C++ sources (bio) = $(sources_bio)"
	@echo ""
	@echo "C++ objects (bio) = $(objects_bio)"
	@echo ""
	@echo "C++ objectsWithPath = $(objectsWithPath)"
	@echo ""
	@echo "C++ programs = $(programs)"
	@echo ""
	@echo "C++ binaries = $(binaries)"
	@echo ""
	@echo "C++ programs_test = $(programs_test)"
	@echo ""
	@echo "C++ binaries_test = $(binaries_test)"
	@echo ""
	@echo "X headers = $(crossed_headers)"
	@echo ""
	@echo "X sources = $(crossed_sources)"
	@echo ""
	@echo "X objects = $(crossed_objects)"
	@echo ""
	@echo "F77 headers = $(f77_headers)"
	@echo ""
	@echo "F77 sources = $(f77_sources)"
	@echo ""
	@echo "F77 objects = $(f77_objects)"
	@echo ""
	@echo "F77 programs = $(f77_programs)"
	@echo ""
	@echo "F77 binaries = $(f77_binaries)"
	@echo ""
	@echo "F77 programs_test = $(f77_programs_test)"
	@echo ""
	@echo "F77 binaries_test = $(f77_binaries_test)"
	@echo ""
	@echo "arch dir = $(ARCHDIR)"
	@echo "CPPFLAGS='$(CPPFLAGS)'"
	@echo "LDFLAGS='$(LDFLAGS)'"

##########################################################

.PHONY : build_special
build_special :
	@echo "Target 'build_special'..."

.PHONY : install
install : 
	@echo "Installing..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)

##########################################################

.PHONY : clean_special
clean_special :
	#@echo "Target 'clean_special'..."

.PHONY : uninstall_special
uninstall : 
	#@echo "Uninstalling..."

##########################################################

.PHONY : system
system :
	@echo "$(SYSTEM)"

.PHONY : setup
setup :
	@echo "export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PACKAGE_DIR)/$(SYSTEM)/lib"

.PHONY : lib_xbio
lib_xbio : lib lib_bio

.PHONY : lib
lib : lib_shared_f77 lib_shared 

.PHONY : lib_bio
lib_bio : lib_shared_bio

.PHONY : lib_shared_bio
lib_shared_bio : $(SHLIBFILE_BIO) lib_shared
	@echo "SHLIBFILE_BIO=$(SHLIBFILE_BIO)"

.PHONY : libs
libs : lib_shared

# .PHONY : lib_static
# lib_static : $(LIBFILE)

.PHONY : lib_shared
lib_shared : $(SHLIBFILE)

.PHONY : lib_shared_f77
lib_shared_f77 : $(SHLIBFILE_F77)

.PHONY : clean
clean : clean_special
	@echo "Cleaning..."
	find $(BASEDIR)/ -name "*~" -exec $(RM) \{\} \; || echo -n "" 
	test -d $(ARCHDIR)/ && $(RM) -r $(ARCHDIR) || echo -n ""

.PHONY : clean_bin_test
clean_bin_test : 
	find $(ARCHDIR)/bin -name "test_*" -a -executable -exec $(RM) \{\} \; || echo -n 

.PHONY : distclean
distclean : uninstall clean
	@echo "Total cleaning..."

.PHONY : bin
bin : bin_shared bin_shared_f77

.PHONY : bin_test
bin_test : bin_test_shared

.PHONY : bins
bins : bin_shared bin_shared_f77 #bin_static 

# .PHONY : bin_static
# bin_static : $(binaries_static)

.PHONY : bin_shared
bin_shared : $(binaries) 

.PHONY : bin_shared
bin_shared_f77 : $(f77_binaries)

.PHONY : bin_test_shared
bin_test_shared : $(binaries_test) $(f77_binaries_test)

test_%$(F77_BINEXT) : test_%$(F77_PROGEXT) | lib_shared_f77 
	@echo "Compiling test F77 program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(F77) -I$(TESTDIR) $(F77FLAGS) $(CPPFLAGS_F77) $< $(LDFLAGS_F77) -o $(BINDIR)/$(notdir $(@))

test_%$(BINEXT) : test_%$(PROGEXT) | lib_shared lib_shared_f77 $(binlibdep)
	@echo "Compiling test C++ program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I$(TESTDIR) $(CXXFLAGS) $(CPPFLAGS_CXX) $< $(EXELDFLAGS_BIO) $(LDFLAGS_CXX) -o $(BINDIR)/$(notdir $(@))

%$(F77_BINEXT) : %$(F77_PROGEXT) | lib_shared_f77 
	@echo "Compiling F77 program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(F77) $(F77FLAGS) $(CPPFLAGS_F77) $< $(LDFLAGS_F77) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared lib_shared_f77 $(binlibdep)
	@echo "Compiling C++ program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS_CXX) $< $(EXELDFLAGS_BIO) $(LDFLAGS_CXX) -o $(BINDIR)/$(notdir $(@))

# $(LIBFILE) : $(objectsWithPath)  $(f77_objectsWithPath) 
# 	@echo "Building static lib '$(@)'..."
# 	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
# 	for file_object in $(objects); do \
# 	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
# 	ranlib $(LIBDIR)/$(notdir $(@))

# $(LIBFILEF) : $(f77_objectsWithPath) 
# 	@echo "Building static lib '$(@)'..."
# 	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
# 	for file_object in $(objects); do \
# 	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
# 	ranlib $(LIBDIR)/$(notdir $(@))

$(SHLIBFILE_BIO) : $(objectsWithPath_bio) #| $(SHLIBFILE) 
	@echo "NOTICE: Building shared auxiliary BIO lib '$(@)'..." >&2
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	@test -n "$(objectsWithPath_bio)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(LIBLDFLAGS_BIO) $(DEPLDFLAGS_CXX_SORTED) -o $(LIBDIR)/$(notdir $(@)) || echo ""
	@echo "NOTICE: Building shared lib '$(@)' done." >&2
	@echo

$(SHLIBFILE) : $(objectsWithPath) | $(SHLIBFILE_F77) 
	@echo "Building shared lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	$(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(LIBLDFLAGS) $(DEPLDFLAGS_CXX_SORTED) -o $(LIBDIR)/$(notdir $(@))

$(SHLIBFILE_F77) : $(f77_objectsWithPath) $(crossed_objectsWithPath)  
	@echo "Building shared lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	$(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(DEPLDFLAGS_F77_SORTED) -o $(LIBDIR)/$(notdir $(@))

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) %$(HEADEREXT)
	@echo "Compiling module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS_CXX) $< -o $(OBJDIR)/$(notdir $(@))

$(OBJDIR)/%$(F77_OBJEXT) : %$(F77_SOURCEEXT) #%$(F77_HEADEREXT)
	@echo "Compiling F77 module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(F77) -c $(F77FLAGS) $(CPPFLAGS_F77) $< -o $(OBJDIR)/$(notdir $(@))

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

# end of GNUmakefile (language C++/F77)
