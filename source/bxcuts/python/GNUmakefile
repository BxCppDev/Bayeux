# -*- mode: makefile; -*-
# GNUmakefile

OS=$(shell uname -s)
ifneq ($(OS),Linux)
ifneq ($(OS),Darwin)
$(error Sorry only 'Linux' and 'Darwin' are supported!)
endif
endif
ARCH=$(shell uname -m | sed 's/ //g')
SYSTEM=$(OS)-$(ARCH)
MAKE=make	
CXX=g++
LD=g++
LDFLAGS=-O
CXXFLAGS=-fPIC
PYMODEXT=.so
ifeq ($(OS),Linux)
  SOFLAGS=-shared
  SHLIBEXT=.so
endif
ifeq ($(OS),Darwin)
  SOFLAGS=-bundle
  SHLIBEXT=.dylib
endif

HEADEREXT=.h
SOURCEEXT=.cc

PACKAGENAME=cuts
UPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "a-z" "A-Z")
LPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
LIBNAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}
PYMODULENAME=$(LIBNAME)
PYSHLIBFILE=$(PYMODULENAME)${PYMODEXT}
PYMODSRCFILE=$(PYMODULENAME)${SOURCEEXT}

BASEDIR=..
PACKAGE_ROOT=$(shell cd .. ; pwd)
PACKAGE_DIR=$(shell cd ${BASEDIR} ; pwd)

# boost
ifeq ($(shell which boost-config),)
$(error Sorry boost-config not found!)
endif

PYVERSION=$(shell boost-config --python-version)

# python
ifeq ($(shell which python${PYVERSION}),)
$(error Sorry Python ${PYVERSION} not found!)
endif
python_bin=python${PYVERSION}
python_version=$(shell $(python_bin) -V 2>&1 | tr -s ' ' | cut -d' ' -f2 | cut -d'.' -f1,2)
python_config=python$(python_version)-config
python_cflags=$(shell $(python_config) --cflags)
python_ldflags=$(shell $(python_config) --ldflags)

boost_python_cflags=$(shell boost-config --cflags python)
boost_python_ldflags=$(shell boost-config --ldflags python)

# datatools
ifeq ($(shell which datatools-config),)
$(error Sorry datatools-config not found!)
endif
datatools_cflags=$(shell datatools-config --cflags)
datatools_ldflags=$(shell datatools-config --ldflags)

# cuts
cuts_cflags=$(shell $(PACKAGE_DIR)/pkgtools.d/$(LPACKAGENAME)-config --cflags)
cuts_ldflags=$(shell $(PACKAGE_DIR)/pkgtools.d/$(LPACKAGENAME)-config --ldflags)

DEPCPPFLAGS=
DEPLDFLAGS=

### Dependencies:
#DEP_HOOK

DEPCPPFLAGS+=$(datatools_cflags)
DEPLDFLAGS+=$(datatools_ldflags)

DEPCPPFLAGS+=$(boost_python_cflags)
DEPLDFLAGS+=$(boost_python_ldflags)

#DEPCPPFLAGS+=$(python_cflags)
#DEPLDFLAGS+=$(python_ldflags)

### End of dependencies.
CPPFLAGS1+=$(DEPCPPFLAGS) 
CPPFLAGS1+=-I$(PACKAGE_DIR)/include
CPPFLAGS1+=-I.
LDFLAGS1+=-DBOOST_PYTHON_DYNAMIC_LIB 
LDFLAGS1+=-L../$(SYSTEM)/lib -l$(LIBNAME)
LDFLAGS1+=$(DEPLDFLAGS) 
LDFLAGS1+=-lm 


CPPFLAGS+=$(shell echo $(CPPFLAGS1) | sed -e "s/-Wstrict-prototypes//g" | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py ) 
LDFLAGS+=$(shell echo $(LDFLAGS1) | python $(PACKAGE_ROOT)/pkgtools.d/mkuniqueflags.py -r) 

.PHONY : all clean lib shlib test

all : lib

test:
	@echo "SYSTEM=$(SYSTEM)"
	@echo "PYMODULENAME=$(PYMODULENAME)"
	@echo "PYMODSRCFILE=$(PYMODSRCFILE)"
	@echo "PYSHLIBFILE=$(PYSHLIBFILE)"
	@echo "CXXFLAGS=$(CXXFLAGS)"
	@echo "CPPFLAGS=$(CPPFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "python_version=$(python_version)"
	@echo "python_config=$(python_config)"

clean :
	$(RM) *~
	$(RM) *.pyc
	$(RM) $(PYSHLIBFILE)

lib : shlib

shlib : $(PYSHLIBFILE)

$(PYSHLIBFILE) : $(PYMODSRCFILE) 
	$(CXX) $< $(CPPFLAGS) $(SOFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ 

install :

uninstall :

# end of GNUmakefile
