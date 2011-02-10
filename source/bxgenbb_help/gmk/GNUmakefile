# -*- mode: makefile; -*- 
# GNUmakefile (language C++)

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

# Choices:
# F77_COMP=g77
# F77_COMP=gfortran
ifneq ($(F77_COMP),)
F77=$(F77_COMP)
$(warning Force $(F77_COMP) compiler!)
endif

COMPILER=$(CC)$(shell $(CC) --version | head -1 | cut -d ' ' -f 3)
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

ifeq ($(F77),g77)
F77FLAGS+=-fno-automatic -finit-local-zero -Wno-globals -fno-globals
F77FLAGS+=-fno-second-underscore
endif

ifeq ($(F77),gfortran)
F77FLAGS+= 
endif

processor=$(shell uname -p)
ifeq ($(strip $(processor)),x86_64)
dummy=$(shell echo "Processor: 64bits")
else
CXXFLAGS+= -Df2cFortran
#F77FLAGS+= #-malign-double
endif


CPPFLAGS=-I. -I$(INCDIR)
LDFLAGS=-O -L$(LIBDIR) -l$(LIBNAME)

DEPCPPFLAGS=
DEPLDFLAGS=
### Dependencies:
#DEP_HOOK

# geomtools dependency:
ifeq ($(shell which geomtools-config),)
$(error Sorry geomtools-config not found!)
endif
DEPCPPFLAGS+=$(shell geomtools-config --cflags)
DEPLDFLAGS+=$(shell geomtools-config --ldflags)

# mygsl dependency:
# ifeq ($(shell which mygsl-config),)
# $(error Sorry mygsl-config not found!)
# endif
# DEPCPPFLAGS+=$(shell mygsl-config --cflags)
# DEPLDFLAGS+=$(shell mygsl-config --ldflags)

#Here you should add stuff for 'CERNLIB':
#DEPCPPFLAGS+=
#DEPLDFLAGS+=

# cernlib
ifeq ($(strip $(CERN_ROOT)),)
$(error Sorry CERNLIB (env. CERN_ROOT) is not configured!)
endif
ifeq ($(strip $(GENBB_HELP_CERNLIB_INC)),)
DEPCPPFLAGS+=-I$(CERN_ROOT)/include
else
DEPCPPFLAGS+=-I$(GENBB_HELP_CERNLIB_INC)
fi
ifeq ($(strip $(GENBB_HELP_CERNLIB_LIBS)),)
DEPLDFLAGS+=$(shell cernlib packlib mathlib kernlib)
else
DEPLDFLAGS+=$(GENBB_HELP_CERNLIB_LIBS)
fi

# f2c dependency:
ifeq ($(F77),g77)
DEPLDFLAGS+=-lg2c
endif

ifeq ($(F77),gfortran)
DEPLDFLAGS+=-lgfortran
endif

CPPFLAGS1=
CPPFLAGS2=
CPPFLAGS3=

# Force only one underscore for extern Fortran77/C+ names:
underscore_mode=one
$(warning Force only one underscore for extern Fortran77/C+ names)

#ifeq ($(F77),gfortran)
#underscore_mode=one
#endif

#ifeq ($(F77),g77)
#underscore_mode=two
#endif

#ifneq ($(F77_TWO_UNDERSCORES),)
#underscore_mode=two
#endif

#ifneq ($(F77_ONE_UNDERSCORE),)
#underscore_mode=one
#endif

ifeq ($(underscore_mode),two)
CPPFLAGS1+=-Dfermi_func_shape_only=fermi_func_shape_only__
CPPFLAGS1+=-Dfermi_wrap=fermi_wrap__
CPPFLAGS1+=-Dfermi_func=fermi_func__
CPPFLAGS1+=-Dfermi_func_nr_approx=fermi_func_nr_approx__
CPPFLAGS1+=-Drng_shoot_reset=rng_shoot_reset__
CPPFLAGS1+=-Drng_shoot_flat=rng_shoot_flat__

CPPFLAGS3+=-Dgenbbsub=genbbsub__
CPPFLAGS3+=-Dset_random_seed=set_random_seed__
CPPFLAGS3+=-Dgrndm=grndm__
CPPFLAGS3+=-Dgenevent=genevent__
CPPFLAGS3+=-Dgenbb_init_enrange=genbb_init_enrange__
CPPFLAGS3+=-Denrange=enrange__
endif

ifeq ($(underscore_mode),one)
CPPFLAGS1+=-Dfermi_func_shape_only=fermi_func_shape_only_
CPPFLAGS1+=-Dfermi_wrap=fermi_wrap_
CPPFLAGS1+=-Dfermi_func=fermi_func_
CPPFLAGS1+=-Dfermi_func_nr_approx=fermi_func_nr_approx_
CPPFLAGS1+=-Drng_shoot_reset=rng_shoot_reset_
CPPFLAGS1+=-Drng_shoot_flat=rng_shoot_flat_

CPPFLAGS3+=-Dgenbbsub=genbbsub_
CPPFLAGS3+=-Dset_random_seed=set_random_seed_
CPPFLAGS3+=-Dgrndm=grndm_
CPPFLAGS3+=-Dgenbb_init_enrange=genbb_init_enrange_
CPPFLAGS3+=-Dgenevent=genevent_
CPPFLAGS3+=-Denrange=enrange_
endif

### End of dependencies.
CPPFLAGS1+=$(DEPCPPFLAGS) 
CPPFLAGS2+=$(DEPCPPFLAGS) 
LDFLAGS1+=$(DEPLDFLAGS) 
LDFLAGS1+=-lm 

CPPFLAGS+=$(shell echo $(CPPFLAGS1) $(CPPFLAGS3) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py) 
LDFLAGS+=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 

DUMMY=-I. -I$(INCDIR)
F77CPPFLAGS+=$(shell echo $(CPPFLAGS1) $(CPPFLAGS2) $(DUMMY) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py)

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
programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT) 2> /dev/null))
programs_test:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^test_)
programs:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep -v ^test_)
#$(notdir $(shell ls $(PROGDIR)/*$(PROGEXT)))
binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))
binaries_test:= $(programs_test:%$(PROGEXT)=%$(BINEXT))

f77_headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(F77_HEADEREXT)))
f77_sources := $(notdir $(shell ls $(SRCDIR)/*$(F77_SOURCEEXT)))
f77_objects := $(f77_sources:%$(F77_SOURCEEXT)=%$(F77_OBJEXT))
f77_objectsWithPath := $(f77_sources:%$(F77_SOURCEEXT)=$(OBJDIR)/%$(F77_OBJEXT))
f77_programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(F77_PROGEXT) 2> /dev/null))
f77_programs_test:= $(shell echo $(f77_programs_all) | tr ' ' '\n' | grep .F$ | grep ^test_)
f77_programs:= $(shell echo $(f77_programs_all) | tr ' ' '\n' | grep .F$ | grep -v ^test_)
f77_binaries:= $(f77_programs:%$(F77_PROGEXT)=%$(F77_BINEXT))
f77_binaries_test:= $(f77_programs_test:%$(F77_PROGEXT)=%$(F77_BINEXT))


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
	#@echo "make bin_test  : build the shared test executables (not mandatory)"
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
	@echo "C++ headers = $(headers)"
	@echo "C++ sources = $(sources)"
	@echo "C++ objects = $(objects)"
	@echo "C++ programs = $(programs)"
	@echo "C++ binaries = $(binaries)"
	@echo "C++ programs_test = $(programs_test)"
	@echo "C++ binaries_test = $(binaries_test)"
	@echo "F77 headers = $(f77_headers)"
	@echo "F77 sources = $(f77_sources)"
	@echo "F77 objects = $(f77_objects)"
	@echo "F77 programs = $(f77_programs)"
	@echo "F77 binaries = $(f77_binaries)"
	@echo "F77 programs_test = $(f77_programs_test)"
	@echo "F77 binaries_test = $(f77_binaries_test)"
	@echo "arch dir = $(ARCHDIR)"
	@echo "CPPFLAGS='$(CPPFLAGS)'"
	@echo "LDFLAGS='$(LDFLAGS)'"

##########################################################

.PHONY : build_special
build_special :
	@echo "Target 'build_special'..."

.PHONY : install
install : 
	@#echo "Installing..."
	@#cat $(BASEDIR)/pkgtools.d/package.csh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).csh
	@#cat $(BASEDIR)/pkgtools.d/package.sh.skel | sed -e "s@__PACKAGE_ROOT__@$(PACKAGE_ROOT)@g" -e "s@__PACKAGE_NAME__@$(UPACKAGENAME)@g" -e "s@__Package_name__@$(PACKAGENAME)@g" > $(BASEDIR)/$(LIBNAME).sh
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@#ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config
	@#ln -s -f $(PACKAGE_ROOT)/scripts/genbb $(BINDIR)/genbb

##########################################################

.PHONY : clean_special
clean_special :
	@echo "Target 'clean_special'..."

.PHONY : uninstall_special
uninstall : 
	@echo "Uninstalling..."
	#test -L $(BINDIR)/$(LIBNAME)-config && $(RM) $(BINDIR)/$(LIBNAME)-config || echo -n ""
	#test -f $(BASEDIR)/$(LIBNAME).sh && $(RM) $(BASEDIR)/$(LIBNAME).sh || echo -n ""
	#test -f $(BASEDIR)/$(LIBNAME).csh && $(RM) $(BASEDIR)/$(LIBNAME).csh || echo -n ""
	#test -L $(BINDIR)/genbb && rm -f $(BINDIR)/genbb || echo -n ""


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
	find $(BASEDIR)/ -name "*~" -exec $(RM) \{\} \; || echo -n "" 
	test -d $(ARCHDIR)/ && $(RM) -r $(ARCHDIR) || echo -n ""

.PHONY : clean_bin_test
clean_bin_test : 
	find $(ARCHDIR)/bin -name "test_*" -a -executable -exec $(RM) \{\} \; || echo -n 

.PHONY : distclean
distclean : uninstall clean
	@echo "Total cleaning..."

.PHONY : bin
bin : bin_shared

.PHONY : bin_test
bin_test : bin_test_shared

.PHONY : bins
bins : bin_shared bin_static 

.PHONY : bin_static
bin_static : $(binaries_static)

.PHONY : bin_shared
bin_shared : $(binaries) $(f77_binaries)

.PHONY : bin_test_shared
bin_test_shared : $(binaries_test) $(f77_binaries_test)

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
