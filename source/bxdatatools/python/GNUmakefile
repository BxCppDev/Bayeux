##
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
ifeq ($(OS),Linux)
  SOFLAGS=-shared
  SHLIBEXT=.so
endif
ifeq ($(OS),Darwin)
  SOFLAGS=-dynamiclib -twolevel_namespace -undefined dynamic_lookup -dynamic -single_module
  SHLIBEXT=.dylib
endif

HEADEREXT=.h
SOURCEEXT=.cc

PACKAGENAME=datatools
UPACKAGENAME=$(shell echo "$(PACKAGENAME)" | tr "a-z" "A-Z")
LIBNAME=$(shell echo "$(PACKAGENAME)" | tr "A-Z" "a-z")
SHLIBFILE=lib$(LIBNAME)${SHLIBEXT}
PYMODULENAME=datatools
PYSHLIBFILE=$(PYMODULENAME)${SHLIBEXT}
PYMODSRCFILE=$(PYMODULENAME)${SOURCEEXT}

# python
ifeq ($(shell which python2.5),)
$(error Sorry Python 2.5 not found!)
endif
python_bin=python2.5
python_version=$(shell $(python_bin) -V 2>&1 | tr -s ' ' | cut -d' ' -f2 | cut -d'.' -f1,2)
python_config=python$(python_version)-config
python_cflags=$(shell $(python_config) --cflags)
python_ldflags=$(shell $(python_config) --ldflags)

# boost
ifeq ($(shell which boost-config),)
$(error Sorry boost-config not found!)
endif
boost_cflags=$(shell boost-config --cflags)
boost_ldflags=$(shell boost-config --ldflags python serialization)

# datatools
#ifeq ($(shell which datatools-config),)
#$(warning Sorry datatools-config not found!)
#datatools_cflags=
#datatools_ldflags=
#else
datatools_cflags=$(shell $(PACKAGE_DIR)/pkgtools.d/datatools-config --cflags)
datatools_ldflags=$(shell $(PACKAGE_DIR)/pkgtools.d/datatools-config --ldflags)
#endif


BASEDIR=..
PACKAGE_ROOT=$(shell cd .. ; pwd)
PACKAGE_DIR=$(shell cd ${BASEDIR} ; pwd)

DEPCPPFLAGS=
DEPLDFLAGS=

### Dependencies:
#DEP_HOOK

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
CPPFLAGS1+=$(EOS_PBA_FLAGS) $(FPU_FLAGS) 

DEPCPPFLAGS+=$(datatools_cflags)
DEPLDFLAGS+=$(datatools_ldflags)

DEPCPPFLAGS+=$(boost_cflags)
DEPLDFLAGS+=$(boost_ldflags)

DEPCPPFLAGS+=$(python_cflags)
DEPLDFLAGS+=$(python_ldflags)

### End of dependencies.
CPPFLAGS1+=$(DEPCPPFLAGS) 
CPPFLAGS1+=-I$(PACKAGE_DIR)/include
CPPFLAGS1+=-I.
LDFLAGS1+=-DBOOST_PYTHON_DYNAMIC_LIB 
LDFLAGS1+=-L../$(SYSTEM)/lib -ldatatools
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
	#ln -s $(PACKAGE_DIR)/scripts/n3visu $(PACKAGE_DIR)/$(SYSTEM)/bin/

uninstall :
	#test -L $(PACKAGE_DIR)/$(SYSTEM)/bin/n3visu && rm -f $(PACKAGE_DIR)/$(SYSTEM)/bin/n3visu

# end of GNUmakefile
