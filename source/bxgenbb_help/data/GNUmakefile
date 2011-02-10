# GNUmakefile

LDFLAGS=$(shell cernlib packlib mathlib)

.PHONY : all
all : decay0

.PHONY : clean
clean : 
	$(RM) *~
	$(RM) decay0


decay0 : decay0_2011-01-23.for
	gfortran $< -o $@ $(LDFLAGS)
# end
