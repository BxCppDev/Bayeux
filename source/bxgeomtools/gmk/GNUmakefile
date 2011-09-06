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
LPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
LIBNAME=$(LPACKAGENAME)
LIBNAME_BIO=$(LIBNAME)_bio
LIBFILE=lib$(LIBNAME).a
LIBFILE_BIO=lib$(LIBNAME_BIO).a
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}
SHLIBFILE_BIO=lib$(LIBNAME_BIO)${SHLIBEXT}
LIBVER=$(shell cat ../VERSION |tr -d '\n')

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

CXXFLAGS=-fPIC
CPPFLAGS=-I. -I$(INCDIR)
#LDFLAGS=-O -L$(LIBDIR) -l$(LIBNAME)
LDFLAGS=-O $(ABSLIBDIR)/lib$(LIBNAME)${SHLIBEXT}

binlibdep=lib

DEPCPPFLAGS=
DEPLDFLAGS=
### Dependencies:
#DEP_HOOK

# mygsl dependency:
ifeq ($(shell which mygsl-config),)
$(error Sorry mygsl-config not found!)
endif
DEPCPPFLAGS+=$(shell mygsl-config --cflags)
DEPLDFLAGS+=$(shell mygsl-config --ldflags)

# CLHEP dependency:
# ifeq ($(strip $(CLHEP_BASE_DIR)),)
# $(error Sorry CLHEP is not configured!)
# endif
# ifeq ($(shell which clhep-config),)
# $(error Sorry clhep-config not found!)
# endif
# DEPCPPFLAGS+=$(shell clhep-config --include)
# DEPLDFLAGS+=$(shell clhep-config --libs)

# datatools:
ifeq ($(shell which datatools-config),)
$(error Sorry datatools-config not found!)
endif
DEPCPPFLAGS+=$(shell datatools-config --cflags)
DEPLDFLAGS+=$(shell datatools-config --ldflags)

### Use or not Embedded Boost I/O:
EBIO_CFLAGS=
EBIO_LDFLAGS=
ifeq ($(GEOMTOOLS_EBIO),1)
GEOMTOOLS_EBIO=1
EBIO_CFLAGS=-DGEOMTOOLS_EBIO=1
$(warning WARNING: geomtools uses 'Embedded Boost I/O' !)
else
GEOMTOOLS_EBIO=0
EBIO_CFLAGS=-DGEOMTOOLS_EBIO=0
$(warning WARNING: geomtools does not use 'Embedded Boost I/O' !)
endif

### Use or not Embedded Boost I/O:
XBIO_CFLAGS=
XBIO_LDFLAGS=
ifeq ($(GEOMTOOLS_XBIO),1)
GEOMTOOLS_XBIO=1
XBIO_CFLAGS=-DGEOMTOOLS_XBIO=1
XBIO_LDFLAGS=-l$(LIBNAME_BIO)
binlibdep=lib_xbio
$(warning WARNING: geomtools uses 'Auxiliary Boost I/O' !)
else
GEOMTOOLS_XBIO=0
XBIO_CFLAGS=-DGEOMTOOLS_XBIO=0
$(warning WARNING: geomtools does not use 'Auxiliary Boost I/O' !)
endif

$(warning WARNING: geomtools EBIO_FLAGS=$(EBIO_CFLAGS))
$(warning WARNING: geomtools XBIO_FLAGS=$(XBIO_CFLAGS))

### End of dependencies.
CPPFLAGS1+=$(EBIO_CFLAGS) $(XBIO_CFLAGS) $(DEPCPPFLAGS)
LDFLAGS1+=$(DEPLDFLAGS)

# default with libmath dependency:
LDFLAGS1+=-lm

CPPFLAGS+=$(shell echo $(CPPFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py)
DEPLDFLAGS_SORTED=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r)
LDFLAGS+=$(DEPLDFLAGS_SORTED)

# Special additional link flag for BIO DLL and executable :
LIBLDFLAGS_BIO=-L$(LIBDIR) -l$(LIBNAME)
EXELDFLAGS_BIO=-L$(LIBDIR) $(XBIO_LDFLAGS) -l$(LIBNAME)

#CXXFLAGSSHFLAGS=-Wall -v
#STATICFLAGS=-static

HEADEREXT=.h
SOURCEEXT=.cc
PROGEXT=.cxx
OBJEXT=.o
LIBEXT=.a
BINEXT=

.SUFFIXES : $(HEADEREXT) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT)

vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)/test
vpath %$(SOURCEEXT)  $(SRCDIR)
vpath %$(PROGEXT)    $(PROGDIR)
vpath test_%$(PROGEXT) $(TESTDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)
vpath %$(BINEXT)     $(BINDIR)

headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT) 2> /dev/null))

sources1=
sources1+=address_set.cc
sources1+=any_shape_3d.cc
sources1+=dummy_geomtools.cc
sources1+=gdml_export.cc
sources1+=gdml_writer.cc
sources1+=geom_id.cc
sources1+=geom_info.cc
sources1+=geom_map.cc
sources1+=gnuplot_draw.cc
sources1+=gnuplot_drawer.cc
sources1+=gnuplot_i.cc
sources1+=hexagon_box.cc
sources1+=id_mgr.cc
sources1+=id_selector.cc
sources1+=i_locator.cc
sources1+=intersection.cc
sources1+=i_placement.cc
sources1+=i_stackable.cc
sources1+=logical_volume.cc
sources1+=mapping.cc
sources1+=mapping_utils.cc
sources1+=model_factory.cc
sources1+=model_with_internal_items_tools.cc
sources1+=multiple_items_model.cc
sources1+=physical_volume.cc

sources1+=multiple_placement.cc
sources1+=regular_grid_placement.cc
sources1+=regular_linear_placement.cc
sources1+=placement.cc

sources1+=regular_polygon.cc
sources1+=sensitive.cc
sources1+=smart_id_locator.cc

# One-dimensional objects:
sources1+=helix_3d.cc
sources1+=line_3d.cc
sources1+=polyline_3d.cc
sources1+=circle.cc

# Two-dimensional objects:
sources1+=rectangle.cc
sources1+=disk.cc

# Models:
sources1+=cylindric_extrusion_boxed_model.cc
sources1+=replicated_boxed_model.cc
sources1+=rotated_boxed_model.cc
sources1+=simple_boxed_model.cc
sources1+=simple_shaped_model.cc
sources1+=simple_world_model.cc
sources1+=stacked_model.cc
sources1+=i_boxed_model.cc
sources1+=surrounded_boxed_model.cc

# sources1+=test_model_1.cc
# sources1+=test_model_2.cc
# sources1+=test_world_model.cc

# 3D shapes:
sources1+=cylinder.cc
sources1+=box.cc
sources1+=sphere.cc
sources1+=tube.cc
sources1+=tessellation.cc
sources1+=polycone.cc
sources1+=polyhedra.cc

# 3D composite  shapes:
sources1+=subtraction_3d.cc
sources1+=union_3d.cc
sources1+=intersection_3d.cc
sources1+=i_composite_shape_3d.cc

sources1+=i_model.cc

sources1+=i_shape_1d.cc
sources1+=i_shape_2d.cc
sources1+=i_shape_3d.cc
sources1+=i_object_3d.cc

sources1+=visibility.cc
sources1+=color.cc
sources1+=material.cc
sources1+=utils.cc

# standard mode :
ifeq ($(GEOMTOOLS_EBIO),0)
sources := $(sources1)
else
# only for GEOMTOOLS_EBIO==1 mode :
sources := the_serializable.cc $(sources1)
endif

# only for GEOMTOOLS_XBIO==1 mode :
sources_bio :=  $(notdir $(shell ls $(SRCDIR)/the_serializable$(SOURCEEXT) 2> /dev/null))

objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
objects_bio := $(sources_bio:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath_bio := $(sources_bio:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT) 2> /dev/null))
programs_test:= $(notdir $(shell ls $(TESTDIR)/test_*$(PROGEXT) 2> /dev/null))

# programs_test:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^test_)
programs:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep -v ^test_)

binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
#binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))

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

.PHONY : test1
test1:
	@echo "sources = $(sources)"

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
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@#ln -s -f $(PACKAGE_ROOT)/pkgtools.d/$(LIBNAME)-config $(BINDIR)/$(LIBNAME)-config

#########################################################################

.PHONY : clean_special
clean_special :
	@echo "Target 'clean_special'..."

.PHONY : uninstall
uninstall :
	@echo "Uninstalling..."

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
libs : lib_shared

# .PHONY : lib_static
# lib_static : $(LIBFILE)

.PHONY : lib_shared
lib_shared : $(SHLIBFILE)
	@echo "SHLIBFILE=$(SHLIBFILE)"

.PHONY : lib_shared_bio
lib_shared_bio : $(SHLIBFILE_BIO) lib_shared
	@echo "SHLIBFILE_BIO=$(SHLIBFILE_BIO)"

.PHONY : clean
clean : clean_special
	@echo "Cleaning..."
	find $(BASEDIR)/ -name "*~" -exec $(RM) \{\} \;  || echo -n ""
	test -d $(ARCHDIR)/ && $(RM) -r $(ARCHDIR) || echo -n ""
	@echo "Cleaning done."
	@echo

.PHONY : clean_bin_test
clean_bin_test :
	find $(ARCHDIR)/bin -name "test_*" -a -executable -exec $(RM) \{\} \;  || echo -n ""

.PHONY : distclean
distclean : uninstall clean
	@echo "Total cleaning..."
	@echo

.PHONY : bin
bin : bin_shared

.PHONY : bin_test
bin_test : bin_test_shared

.PHONY : bins
bins : bin_shared #bin_static

# .PHONY : bin_static
# bin_static : $(binaries_static)

.PHONY : bin_shared
bin_shared : $(binaries)

.PHONY : bin_test_shared
bin_test_shared : ${binaries_test}

test_%$(BINEXT) : test_%$(PROGEXT) | lib_shared $(binlibdep)
	@echo "Compiling test program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. -I$(TESTDIR) $(CPPFLAGS) $(CXXFLAGS) $< $(EXELDFLAGS_BIO) $(DEPLDFLAGS_SORTED) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared $(binlibdep)
	@echo "Compiling program '$(@)' with shared library..."
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(EXELDFLAGS_BIO) $(DEPLDFLAGS_SORTED) -o $(BINDIR)/$(notdir $(@))

# $(LIBFILE) : $(objectsWithPath)
# 	@echo "Building static lib '$(@)'..."
# 	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
# 	for file_object in $(objects); do \
# 	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
# 	ranlib $(LIBDIR)/$(notdir $(@))
# 	@echo "Building static lib '$(@)' done."
# 	@echo

$(SHLIBFILE_BIO) : $(objectsWithPath_bio)
	@echo "NOTICE: Building shared auxiliary BIO lib '$(@)'..." >&2
	@echo "NOTICE: DEPLDFLAGS_SORTED=$(DEPLDFLAGS_SORTED)" >&2
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	@test -n "$(objectsWithPath_bio)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(LIBLDFLAGS_BIO) $(DEPLDFLAGS_SORTED) -o $(LIBDIR)/$(notdir $(@)) || echo ""
	@echo "NOTICE: Building shared lib '$(@)' done." >&2
	@echo

$(SHLIBFILE) : $(objectsWithPath)
	@echo "Building shared lib '$(@)'..."
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	@test -n "$(objectsWithPath)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(DEPLDFLAGS_SORTED) -o $(LIBDIR)/$(notdir $(@)) || echo ""
	@echo "Building shared lib '$(@)' done."
	@echo

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) %$(HEADEREXT)
	@echo "Compiling module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

# %.static$(BINEXT) : %$(PROGEXT) | lib_static
# 	@echo "Compiling statically linked program '$(@)'..."
# 	@test -d $(BINDIR) || mkdir -p $(BINDIR)
# 	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

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
