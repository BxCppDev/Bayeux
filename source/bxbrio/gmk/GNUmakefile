# -*- mode: makefile; -*- 
# GNUmakefile (language C++)

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

###COMPILER=-$(CC)$(shell $(CC) --version | head -1 | cut -d ' ' -f 3)
COMPILER=
SYSTEM=$(OS)-$(ARCH)$(COMPILER)

.PHONY : all
all : lib

PACKAGENAME=brio
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
ABSLIBDIR=$(PACKAGE_DIR)/$(SYSTEM)/lib

CXXFLAGS=-fPIC
CPPFLAGS=-I. -I$(INCDIR)
#LDFLAGS=-O -L$(LIBDIR) -l$(LIBNAME)
LDFLAGS=-O $(ABSLIBDIR)/lib$(LIBNAME)${SHLIBEXT}

DEPCPPFLAGS=
DEPLDFLAGS=
### Dependencies:
#DEP_HOOK

#Here one sets the stuff for 'ROOT':
#ifeq ($(shell which root-config),)
#$(error Sorry root-config not found!)
#endif
DEPCPPFLAGS+=$(shell root-config --cflags)
DEPLDFLAGS+=$(shell root-config --libs)

#Here one sets the stuff for 'datatools':
#ifeq ($(shell which datatools-config),)
#$(error Sorry datatools-config not found!)
#endif
DEPCPPFLAGS+=$(shell datatools-config --cflags)
DEPLDFLAGS+=$(shell datatools-config --ldflags)

DEPLDFLAGS+=-pthread -lm -ldl -rdynamic

#>>> Examples of dependencies stuff:

# 'some_package':
#ifeq ($(shell which XXX-config),)
#$(error Sorry XXX-config not found!)
#endif
#DEPCPPFLAGS+=$(shell XXX-config --cflags)
#DEPLDFLAGS+=$(shell XXX-config --ldflags)

### End of dependencies.
CPPFLAGS1+=$(DEPCPPFLAGS) 
LDFLAGS1+=$(DEPLDFLAGS) 

# default with libmath dependency:
LDFLAGS1+=-lm 

CPPFLAGS+=$(shell echo $(CPPFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py) 
#LDFLAGS+=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 

DEPLDFLAGS_SORTED=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r)
LDFLAGS+=$(DEPLDFLAGS_SORTED)

CXXFLAGSSHFLAGS=-Wall -v
STATICFLAGS=-static

SHFLAGS=$(LDFLAGS) -ldl 

HEADEREXT=.h
SOURCEEXT=.cc
PROGEXT=.cxx
OBJEXT=.o
LIBEXT=.a
BINEXT=

CHEADEREXT=.h
CSOURCEEXT=.c
CPROGEXT=.c
COBJEXT=.o

.SUFFIXES : $(HEADEREXT) $(SOURCEEXT) $(PROGEXT) $(OBJEXT) $(LIBEXT) $(SHLIBEXT) $(BINEXT)

vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)/detail
vpath %$(HEADEREXT)  $(INCDIR)/$(PACKAGENAME)/test
vpath %$(SOURCEEXT)  $(SRCDIR)
vpath %$(PROGEXT)    $(PROGDIR)
vpath %$(OBJEXT)     $(OBJDIR)
vpath %$(LIBEXT)     $(LIBDIR)
vpath %$(SHLIBEXT)   $(LIBDIR)
vpath %$(BINEXT)     $(BINDIR)

vpath %$(CHEADEREXT)  $(INCDIR)/$(PACKAGENAME)
vpath %$(CSOURCEEXT)  $(SRCDIR)
vpath %$(COBJEXT)     $(OBJDIR)
vpath %$(CPROGEXT)    $(PROGDIR)

headers := $(notdir $(shell ls $(INCDIR)/$(PACKAGENAME)/*$(HEADEREXT) 2> /dev/null))

### Basic sources list in arbitrary order:
#sources_check := $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null)
sources := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null))

### If uncommented, this code force XXX.cc to be the last in library 
# due to initialization of a static scoped pointer or similar stuff:
#sources1 := $(notdir $(shell ls $(SRCDIR)/*$(SOURCEEXT) 2> /dev/null | sed -e "s@/brio_record.cc@/@g"))

csources := $(notdir $(shell ls $(SRCDIR)/*$(CSOURCEEXT) 2> /dev/null))
objects := $(sources:%$(SOURCEEXT)=%$(OBJEXT))
objectsWithPath := $(sources:%$(SOURCEEXT)=$(OBJDIR)/%$(OBJEXT))
cobjects := $(csources:%$(CSOURCEEXT)=%$(COBJEXT))
cobjectsWithPath := $(csources:%$(CSOURCEEXT)=$(OBJDIR)/%$(COBJEXT))
programs_all:= $(notdir $(shell ls $(PROGDIR)/*$(PROGEXT) 2> /dev/null))

programs_test:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep ^test_)
programs:= $(shell echo $(programs_all) | tr ' ' '\n' | grep .cxx$ | grep -v ^test_)

binaries:= $(programs:%$(PROGEXT)=%$(BINEXT))
binaries.static:= $(programs:%$(PROGEXT)=%.static$(BINEXT))

binaries_test:= $(programs_test:%$(PROGEXT)=%$(BINEXT))

# ROOT dictionary
dictionary_name := brio_dict
dictionary_obj  := $(dictionary_name)$(OBJEXT)
dictionary_src  := $(SRCDIR)/$(dictionary_name)$(SOURCEEXT)

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
	@echo "make dump      : print some infos"
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
	@echo "headers = $(headers)"
	@echo "sources = $(sources)"
	@echo "sources_check = $(sources_check)"
	@echo "objects = $(objects)"
	@echo "objectsWithPath = $(objectsWithPath)"
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
	@echo "Target 'build_special'..." >&2

.PHONY : install
install : 
	@echo "Installing..." >&2
	@test -d $(BINDIR) || mkdir -p $(BINDIR)

#########################################################################

.PHONY : clean_special
clean_special : clean_dict
	@echo "Target 'clean_special'..." >&2

.PHONY : uninstall
uninstall : 
	@echo "Uninstalling..." >&2

#.PHONY : build_dict
#build_dict : brio_dict.cc
#	@echo "Generating ROOT dictionary for TArrayCMod and brio_record classes..." >&2

#brio_dict.cc :	
#	cd ..; ./tools/make_dict.sh

.PHONY : clean_dict
clean_dict : 
	@echo "Removing ROOT dictionary for TArrayCMod and brio_record classes..." >&2	
	$(RM) $(INCDIR)/$(PACKAGENAME)/detail/brio_dict.h || echo -n ""
	$(RM) $(SRCDIR)/brio_dict.cc || echo -n ""


#########################################################################

.PHONY : system
system :
	@echo "$(SYSTEM)"

# .PHONY : setup
# setup :
# 	@echo "export PATH=$(PACKAGE_DIR)/$(SYSTEM)/bin:$(PATH)"
# 	@echo "export LD_LIBRARY_PATH=$(PACKAGE_DIR)/$(SYSTEM)/lib:$(LD_LIBRARY_PATH)"

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

.PHONY : clean_bin_test
clean_bin_test : 
	find $(ARCHDIR)/bin -name "test_*" -a -executable -exec $(RM) \{\} \;  || echo -n ""

.PHONY : distclean
distclean : uninstall clean
	@echo "NOTICE: Total cleaning..." >&2

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
bin_test_shared : $(binaries_test)

$(LIBFILE) : $(objectsWithPath) 
	@echo "NOTICE: Building static lib '$(@)'..." >&2
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	for file_object in $(objects); do \
	  $(AR) -rusv $(LIBDIR)/$(LIBFILE) $(OBJDIR)/$(notdir $${file_object}) ; done
	ranlib $(LIBDIR)/$(notdir $(@))

$(SHLIBFILE) : $(objectsWithPath) $(OBJDIR)/$(dictionary_obj) 
	@echo "NOTICE: Building shared lib '$(@)'..." >&2
	@test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	@test -n "$(objectsWithPath)" && $(LD) $(CXXFLAGS) $(SOFLAGS) $(^) $(DEPLDFLAGS_SORTED) -o $(LIBDIR)/$(notdir $(@)) || echo ""

$(OBJDIR)/%$(OBJEXT) : %$(SOURCEEXT) %$(HEADEREXT)
	@echo "NOTICE: Compiling module '$(@)'..." >&2
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$(notdir $(@))

%.static$(BINEXT) : %$(PROGEXT) | lib_static
	@echo "NOTICE: Compiling statically linked program '$(@)'..." >&2
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(STATICFLAGS) $(LDFLAGS) -o $(BINDIR)/$(notdir $(@))

%$(BINEXT) : %$(PROGEXT) | lib_shared 
	@echo "NOTICE: Compiling program '$(@)' with shared library..." >&2
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $< $(SHFLAGS) -o $(BINDIR)/$(notdir $(@))

$(OBJDIR)/$(dictionary_obj) : TArrayCMod.h brio_record.h
	@echo "Generating dictionary '$(dictionary_src)'..."
	cd .. ; ./tools/make_dict.sh 
	pwd
	@#rootcint -f $(dictionary_src) -c -I$(INCDIR) $^
	@echo "Compiling module '$(@)'..."
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(dictionary_src) -o $(OBJDIR)/$(notdir $(@))

.PHONY : dist
dist : distclean
	echo "NOTICE: Making the distribution tarball '$(LIBNAME)-$(LIBVER).tar.gz'" >&2
	make clean
	tar cvzf "../../$(LIBNAME)-$(LIBVER).tar.gz" --exclude ".svn" ../../$(PACKAGENAME)/

.PHONY : bzdist
bzdist : distclean
	echo "NOTICE: Making the distribution tarball '$(LIBNAME)-$(LIBVER).tar.bz2'" >&2
	make clean
	tar cvjf "../../$(LIBNAME)-$(LIBVER).tar.bz2" --exclude ".svn" ../../$(PACKAGENAME)/

# end of GNUmakefile (language C++)
