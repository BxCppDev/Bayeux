# GNUmakefile

CPPFLAGS=$(shell geomtools-config --cflags)
LDFLAGS=$(shell geomtools-config --ldflags)

.PHONY : all
all : domestic_1 domestic_2 domestic_3 gmanager

.PHONY : clean
clean :
	$(RM) *~
	$(RM) domestic_1
	$(RM) domestic_2
	$(RM) domestic_3
	$(RM) gmanager
	$(RM) domestic_1.log  
	$(RM) domestic_2.log  
	$(RM) domestic_3.log  
	$(RM) gmanager.log  

.PHONY : run
run : run_1 run_2 run_3 run_gm

.PHONY : run_1
run_1 : domestic_1
	./domestic_1 2> domestic_1.log

.PHONY : run_2
run_2 : domestic_2
	./domestic_2 2> domestic_2.log

.PHONY : run_3
run_3 : domestic_3
	./domestic_3 2> domestic_3.log

.PHONY : run_gm
run_gm : gmanager
	echo "" | ./gmanager domestic_models.geom domestic_categories.lis 1> gmanager.log 2>&1

domestic_1 : domestic_1.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

domestic_2 : domestic_2.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

domestic_3 : domestic_3.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

gmanager : gmanager.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end of GNUmakefile
