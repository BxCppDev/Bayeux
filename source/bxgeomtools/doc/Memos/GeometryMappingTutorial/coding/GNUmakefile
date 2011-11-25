# GNUmakefile

CPPFLAGS=$(shell geomtools-config --cflags)
LDFLAGS=$(shell geomtools-config --ldflags)

.PHONY : all
all : domestic_1 domestic_2 domestic_3

.PHONY : clean
clean :
	$(RM) *~
	$(RM) domestic_1
	$(RM) domestic_2
	$(RM) domestic_3
	$(RM) domestic_1.log  
	$(RM) domestic_2.log  
	$(RM) domestic_3.log  

.PHONY : run
run : run_1 run_2 run_3

.PHONY : run_1
run_1 : domestic_1
	./domestic_1 2> domestic_1.log

.PHONY : run_2
run_2 : domestic_2
	./domestic_2 2> domestic_2.log

.PHONY : run_3
run_3 : domestic_3
	./domestic_3 2> domestic_3.log

domestic_1 : domestic_1.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

domestic_2 : domestic_2.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

domestic_3 : domestic_3.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end of GNUmakefile
