# 

FC=gfortran

CPPFLAGS=-I. $(shell clhep-config --include) $(shell mygsl-config --cflags)
CPPFLAGS+=-Dfermi_wrap=fermi_wrap_
CPPFLAGS+=-Drng_shoot_flat=rng_shoot_flat_
CPPFLAGS+=-Drng_shoot_reset=rng_shoot_reset_
LDFLAGS=$(shell clhep-config --ldflags)
LDFLAGS+=$(shell mygsl-config --ldflags)

sources := $(notdir $(shell ls ./*.f))
ccsources := $(notdir $(shell ls ./*.cc))
ccobjects := $(ccsources:%.cc=%.o)
objects := $(sources:%.f=%.o)
objectsWithPath := $(sources:%.f=./%.o)
ccobjectsWithPath := $(ccsources:%.cc=./%.o)

vpath %.f  .
vpath %.o  .

.PHONY : all
all : bin

.PHONY : prepare
prepare : 
	@touch trick_nevents.f.inc
	@touch trick_write.f.inc

.PHONY : test
test : 
	@echo "test:"
	@echo "-- sources   = $(sources)"
	@echo "-- objects   = $(objects)"
	@echo "-- ccsources = $(ccsources)"
	@echo "-- ccobjects = $(ccobjects)"

.PHONY : bin
bin : prog_DECAY0

prog_DECAY0 : prog_DECAY0.F libdecay0.a
	$(FC) $(CPPFLAGS)  $< libdecay0.a -o $@ $(LDFLAGS)

.PHONY : lib
lib : lib_static

.PHONY : lib_static
lib_static : libdecay0.a

%.o : %.f 
	@echo "Compiling file '$(<)'..."
	$(FC) -c $(CPPFLAGS) $< -o $@

%.o : %.cc 
	@echo "Compiling file '$(<)'..."
	$(CXX) -c $(CPPFLAGS) $< -o $@

libdecay0.a : $(objectsWithPath) $(ccobjectsWithPath)
	@echo "Building archive file '$(@)'..."
	for file_object in "$(objects) $(ccobjects)"; do \
	  $(AR) -rusv ./$@ ./$(notdir $${file_object}) ; done
	@ranlib libdecay0.a

.PHONY : clean
clean :
	$(RM) *~
	$(RM) *.o
	$(RM) prog_DECAY0
	$(RM) libdecay0.a

.PHONY : atom
atom : clean 
	$(RM) *.f
	$(RM) ./genbb_help/*.h
	test -d ./genbb_help && rmdir ./genbb_help || echo -n
	$(RM) *.cc
	$(RM) *.f.orig
	$(RM) *.f.tmp*
	$(RM) *.f.replaced
	$(RM) *.f.inc
	$(RM) *.F
	$(RM) GNUmakefile

# end