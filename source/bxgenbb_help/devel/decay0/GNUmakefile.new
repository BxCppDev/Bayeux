# 

FC=gfortran

CPPFLAGS=-I. 
LDFLAGS=$(shell cernlib packlib kernlib mathlib)

.PHONY : all
all : bin

.PHONY : test
test : 
	@echo "test:"

.PHONY : bin
bin : decay0_fixed test_fermi

decay0_fixed : decay0_fixed.for
	$(FC) $(CPPFLAGS)  $< -o $@ $(LDFLAGS)

test_fermi : test_fermi.for
	$(FC) $(CPPFLAGS)  $< -o $@ $(LDFLAGS)

.PHONY : clean
clean :
	$(RM) *~
	$(RM) decay0_fixed
	$(RM) decay0_reference_plot.ps
	$(RM) genbb_help_validation_plot.ps
	$(RM) Se82_*.his
	$(RM) Se82_*.conf
	$(RM) Se82_*.data
	$(RM) Se82_*.genbb
	$(RM) test_fermi
	$(RM) test_fermi.data
	$(RM) test_fermi_val.data
	$(RM) test_fermi_plot.ps

# end