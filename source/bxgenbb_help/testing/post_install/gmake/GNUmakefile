#

CPPFLAGS=-I.. $(shell genbb_help-config --cflags)
LDFLAGS=$(shell genbb_help-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : test_genbb_help test_genbb_mgr

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_genbb_help
	$(RM) test_genbb_mgr

.PHONY : run
run : run1 run2

.PHONY : run1
run1 : test_genbb_help
	./$< 

.PHONY : run2
run2 : test_genbb_mgr
	./$< 

test_genbb_help : ../../test_genbb_help.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

test_genbb_mgr : ../../test_genbb_mgr.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

