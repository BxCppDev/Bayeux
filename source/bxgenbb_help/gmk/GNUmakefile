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
  #F77=g77
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
  #F77=g77
  F77=gfortran
  LD=g++
  LDFLAGS=-O
  SOFLAGS=-dynamiclib -twolevel_namespace -undefined dynamic_lookup -dynamic -single_module
  SHLIBEXT=.dylib
  STATICFLAGS=
endif

# Choices:
# F77_COMP=g77
# F77_COMP=gfortran
ifneq ($(F77_COMP),)
F77=$(F77_COMP)
$(Force $(F77_COMP) compiler!)
endif

SYSTEM=$(OS)-$(ARCH)

.PHONY : all
all : lib

PACKAGENAME=genbb_help
UPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "a-z" "A-Z")
LIBNAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
LIBFILE=lib$(LIBNAME).a
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}

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
F77FLAGS=-fPIC
test_f77=$(shell which $(F77))
ifeq ($(test_f77),)
$(error no $(F77) is available!)
#else
#$(warning found $(F77) compiler!)
endif


#CXXFLAGS=-fPIC -Df2cFortran

ifeq ($(F77),g77)
F77FLAGS+=-fno-automatic -finit-local-zero -Wno-globals -fno-globals 
endif

ifeq ($(F77),gfortran)
F77FLAGS+= 
endif

processor=$(shell uname -p)
ifeq ($(strip $(processor)),x86_64)
dummy=$(shell echo "Processor: 64bits")
else
CXXFLAGS+= #-malign-double
F77FLAGS+= #-malign-double
endif


CPPFLAGS=-I. -I$(INCDIR)
LDFLAGS=-O -L$(LIBDIR) -l$(LIBNAME)

DEPCPPFLAGS=
DEPLDFLAGS=
### Dependencies:
#DEP_HOOK

# CLHEP dependency:
#ifeq ($(shell which clhep-config),)
#$(error Sorry clhep-config not found!)
#endif
DEPCPPFLAGS+=$(shell clhep-config --include)
DEPLDFLAGS+=$(shell clhep-config --libs)

DEPCPPFLAGS+=$(shell gsl-config --cflags)
DEPLDFLAGS+=$(shell gsl-config --libs)

DEPCPPFLAGS+=$(shell mygsl-config --cflags)
DEPLDFLAGS+=$(shell mygsl-config --ldflags)

#Here you should add stuff for 'CERNLIB':
#DEPCPPFLAGS+=
#DEPLDFLAGS+=

# cernlib
ifeq ($(strip $(CERN_ROOT)),)
$(error Sorry CERNLIB (env. CERN_ROOT) is not configured!)
endif
DEPCPPFLAGS+=-I$(CERN_ROOT)/include
DEPLDFLAGS+=$(shell cernlib packlib mathlib kernlib geant321)

# f2c dependency:
ifeq ($(F77),g77)
DEPLDFLAGS+=-lg2c
endif

ifeq ($(F77),gfortran)
DEPLDFLAGS+=-lgfortran
endif

CPPFLAGS1=
CPPFLAGS2=

ifeq ($(F77),g77)
CPPFLAGS1+=-Dfermi_func_shape_only=fermi_func_shape_only__
CPPFLAGS1+=-Dfermi_wrap=fermi_wrap__
CPPFLAGS1+=-Dfermi_func=fermi_func__
CPPFLAGS1+=-Dfermi_func_nr_approx=fermi_func_nr_approx__
CPPFLAGS1+=-Drng_shoot_reset=rng_shoot_reset__
CPPFLAGS1+=-Drng_shoot_flat=rng_shoot_flat__
endif

ifeq ($(F77),gfortran)
CPPFLAGS1+=-Dfermi_func_shape_only=fermi_func_shape_only_
CPPFLAGS1+=-Dfermi_wrap=fermi_wrap_
CPPFLAGS1+=-Dfermi_func=fermi_func_
CPPFLAGS1+=-Dfermi_func_nr_approx=fermi_func_nr_approx_
CPPFLAGS1+=-Drng_shoot_reset=rng_shoot_reset_
CPPFLAGS1+=-Drng_shoot_flat=rng_shoot_flat_
endif

### End of dependencies.
CPPFLAGS1+=$(DEPCPPFLAGS) 
CPPFLAGS2+=$(DEPCPPFLAGS) 
LDFLAGS1+=$(DEPLDFLAGS) 
LDFLAGS1+=-lm 

CPPFLAGS+=$(shell echo $(CPPFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py) 
LDFLAGS+=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 

F77CPPFLAGS+=$(shell echo $(CPPFLAGS) $(CPPFLAGS2) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py)

CXXFLAGSSHFLAGS=-Wall -v
STATICFLAGS=-static

SHFLAGS=$(LDFLAGS) -ldl 

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
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(BINEXT)     $(BINDIR)

vpath %$(F77_HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(F77_SOURCEEXT)  $(SRCDIR)
vpath %$(F77_OBJEXT)     $(OBJDIR)
vpath %$(F77_PROGEXT)    $(PROGDIR)
vpath %$(F77_BINEXT)     $(BINDIR)

vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)

headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT)))
sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT)))
objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
programs:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT)))
binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))

f77_headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(F77_HEADEREXT)))
f77_sources := $(notdir $(shell ls $(SRCDIR)/*$(F77_SOURCEEXT)))
f77_objects := $(f77_sources:%$(F77_SOURCEEXT)=%$(F77_OBJEXT))
f77_objectsWithPath := $(f77_sources:%$(F77_SOURCEEXT)=$(OBJDIR)/%$(F77_OBJEXT))
f77_programs:= $(notdir $(shell ls $(PROGDIR)/*$(F77_PROGEXT)))
f77_binaries:= $(f77_programs:%$(F77_PROGEXT)=%$(F77_BINEXT))

.PHONY : test
test:
	@echo "PACKAGE_DIR='$(PACKAGE_DIR)'"
	@echo "PACKAGENAME='$(PACKAGENAME)'"
	@echo "LIBNAME='$(LIBNAME)'"
	@echo "SYSTEM='$(SYSTEM)'"
	@echo "INCDIR='$(INCDIR)'"
	@echo "SRCDIR='$(SRCDIR)'"
	@echo "C++ headers = $(headers)"
	@echo "C++ sources = $(sources)"
	@echo "C++ objects = $(objects)"
        @echo "F77 headers = $(f77_headers)"
        @echo "F77 sources = $(f77_sources)"
        @echo "F77 objects = $(f77_objects)"
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
	@cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
	@ln -s -f $(PACKAGE_ROOT)/scripts/genbb $(BINDIR)/genbb

##########################################################

.PHONY : clean_special
clean_special :
	@echo "Target 'clean_special'..."

.PHONY : uninstall_special
uninstall : 
	@echo "Uninstalling..."
	test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""
	test -L $(BINDIR)/genbb && rm -f $(BINDIR)/genbb || echo -n ""


##########################################################

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

.PHONY : distclean
distclean : uninstall clean
	@echo "Total cleaning..."

.PHONY : bin
bin : bin_shared

.PHONY : bins
bins : bin_shared bin_static 

.PHONY : bin_static
bin_static : $(binaries_static)

.PHONY : bin_shared
bin_shared : $(binaries) $(f77_binaries)

$(LIBFILE) : $(objectsWithPath)  $(f77_objectsWithPath) 
	@echo "Building static lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	for file_object in $(objects); do \
	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
	ranlib $(LIBDIR)/$(notdir $(@))

$(SHLIBFILE) : $(objectsWithPath) $(f77_objectsWithPath) 
	@echo "Building shared lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	$(LD) $(CXXFLAGS) $(SOFLAGS) $(^) -o $(LIBDIR)/$(notdir $(@))

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) %$(HEADEREXT)
	@echo "Compiling module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

$(OBJDIR)/%$(F77_OBJEXT) : %$(F77_SOURCEEXT) #%$(F77_HEADEREXT)
	@echo "Compiling F77 module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(F77) -c $(F77CPPFLAGS) $(F77FLAGS) $< -o $(OBJDIR)/$(notdir $(@))


%.static$(BINEXT) : %$(PROGEXT) | lib_static
	@echo "Compiling statically linked program '$(@)'..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared 
	@echo "Compiling program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(F77_BINEXT) : %$(F77_PROGEXT) | lib_shared 
	@echo "Compiling F77 program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(F77) -I. $(F77CPPFLAGS) $(F77FLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

# end of GNUmakefile (language C++/F77)
