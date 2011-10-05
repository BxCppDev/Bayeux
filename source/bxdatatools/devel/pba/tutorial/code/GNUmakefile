# GNUmakefile

CPPFLAGS=$(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : bin run html_code

.PHONY : bin
bin : tutorial_pba_0 tutorial_pba_1 tutorial_pba_2  tutorial_pba_3

.PHONY : run
run : run_tutorial_pba_0 run_tutorial_pba_1 run_tutorial_pba_2 run_tutorial_pba_3

.PHONY : run 
run : run_tutorial_pba_0

.PHONY : clean 
clean : clean_html_code
	$(RM) *~
	$(RM) tutorial_pba_0
	$(RM) tutorial_pba_1
	$(RM) tutorial_pba_2
	$(RM) tutorial_pba_3
	$(RM) *.data
	$(RM) *.bin

run_tutorial_pba_0 : tutorial_pba_0
	@ ./$< 
	@ od -t u1 pba_0.data | head -3 | cut -d' ' -f2- > pba_0.bin
	@ cat pba_0.bin

run_tutorial_pba_1 : tutorial_pba_1
	@ ./$< > tutorial_pba_1.out
	@ cat tutorial_pba_1.out

run_tutorial_pba_2 : tutorial_pba_2
	@ ./$< 
	@ od -t u1 pba_1.data | head -1 | cut -d' ' -f2- > pba_1.bin
	@ cat pba_1.bin

run_tutorial_pba_3 : tutorial_pba_3
	@ ./$< 
	@ od -t u1 pba_2.data | head -2 | cut -d' ' -f2- > pba_2.bin
	@ cat pba_2.bin

tutorial_pba_0 : tutorial_pba_0.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ ${LDFLAGS}

tutorial_pba_1 : tutorial_pba_1.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ ${LDFLAGS}

tutorial_pba_2 : tutorial_pba_2.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ ${LDFLAGS}

.PHONY : clean_html_code
clean_html_code : 	
	$(RM) tutorial_pba_0.cpp.html
	$(RM) tutorial_pba_1.cpp.html
	$(RM) tutorial_pba_2.cpp.html
	$(RM) tutorial_pba_3.cpp.html

.PHONY : html_code
html_code : tutorial_pba_0.cpp.html tutorial_pba_1.cpp.html tutorial_pba_2.cpp.html tutorial_pba_3.cpp.html

%.cpp.html : %.cpp
	./code2html $< > $@


# end of GNUmakefile
