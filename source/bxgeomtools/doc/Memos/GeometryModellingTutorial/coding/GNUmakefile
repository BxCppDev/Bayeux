# GNUmakefile

CPPFLAGS=$(shell geomtools-config --cflags)
LDFLAGS=$(shell geomtools-config --ldflags)

.PHONY : all
all : simple_world_1 simple_world_2 setup_construct_and_view

.PHONY : clean
clean :
	$(RM) *~
	$(RM) simple_world_1
	$(RM) simple_world_2
	$(RM) setup_construct_and_view
	$(RM) simple_world_1.log  
	$(RM) simple_world_2.log  
	$(RM) simple_world_2.gdml  

.PHONY : run
run : run_1 run_2 

.PHONY : run_1
run_1 : simple_world_1
	./simple_world_1 2> simple_world_1.log

.PHONY : run_2
run_2 : simple_world_2
	./simple_world_2 2> simple_world_2.log

simple_world_1 : simple_world_1.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

simple_world_2 : simple_world_2.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

setup_construct_and_view : setup_construct_and_view.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end of GNUmakefile
