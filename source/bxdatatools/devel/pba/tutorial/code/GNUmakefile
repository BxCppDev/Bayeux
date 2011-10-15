# GNUmakefile

CPPFLAGS=-I. $(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : bin run html_code

.PHONY : bin
bin : \
 tutorial_pba_0 \
 tutorial_pba_1 \
 tutorial_pba_2 \
 tutorial_pba_3 \
 tutorial_pba_4 \
 tutorial_pba_5 \
 tutorial_pba_6 \
 tutorial_pba_7 \
 tutorial_pba_8 \
 tutorial_pba_9 \
 tutorial_pba_10 \
 tutorial_pba_11

.PHONY : run 
run : run_tutorial_pba_0 \
      run_tutorial_pba_1 \
      run_tutorial_pba_2 \
      run_tutorial_pba_3 \
      run_tutorial_pba_4 \
      run_tutorial_pba_5 \
      run_tutorial_pba_6 \
      run_tutorial_pba_7 \
      run_tutorial_pba_8 \
      run_tutorial_pba_9 \
      run_tutorial_pba_10 \
      run_tutorial_pba_11

.PHONY : clean 
clean : clean_html_code
	$(RM) *~
	$(RM) tutorial_pba_0
	$(RM) tutorial_pba_1
	$(RM) tutorial_pba_2
	$(RM) tutorial_pba_3
	$(RM) tutorial_pba_4
	$(RM) tutorial_pba_5
	$(RM) tutorial_pba_6
	$(RM) tutorial_pba_7
	$(RM) tutorial_pba_8
	$(RM) tutorial_pba_9
	$(RM) tutorial_pba_10
	$(RM) tutorial_pba_11
	$(RM) *.data
	$(RM) *.bin
	$(RM) *.out
	$(RM) *.log
	$(RM) pba_*.*

run_tutorial_pba_0 : tutorial_pba_0
	@ ./$< 
	@ od -t u1 pba_0.data | head -3 | cut -d' ' -f2- > pba_0.bin
	@ cat pba_0.bin

run_tutorial_pba_1 : tutorial_pba_1
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_1.out
	@ cat tutorial_pba_1.out

run_tutorial_pba_2 : tutorial_pba_2
	@ echo "Running $<..."
	@ ./$< 
	@ od -t u1 pba_2.data | head -2 | cut -d' ' -f2- > pba_2.bin
	@ cat pba_2.bin

run_tutorial_pba_3 : tutorial_pba_3
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_3.out 2>&1 || echo -n
	@ cat tutorial_pba_3.out

run_tutorial_pba_4 : tutorial_pba_4
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_4.out 2>&1 || echo -n
	@ od -t u1 pba_4.data | head -2 | cut -d' ' -f2- > pba_4.bin
	@ cat pba_4.bin
	@ cat tutorial_pba_4.out

run_tutorial_pba_5 : tutorial_pba_5
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_5.out 2>&1 || echo -n
	@ cat tutorial_pba_5.out

run_tutorial_pba_6 : tutorial_pba_6
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_6.out 2>&1 || echo -n
	@ cat tutorial_pba_6.out

run_tutorial_pba_7 : tutorial_pba_7
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_7.out 2>&1 || echo -n
	@ cat tutorial_pba_7.out

run_tutorial_pba_8 : tutorial_pba_8
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_8.out 2>&1 || echo -n
	@# cat tutorial_pba_8.out
	@ cat pba_8.xml | head -10 | recode --diacritics  u8..latin1 | enscript --encoding=latin1 --line-numbers=1 --highlight=html --color --language=html --output=- | recode --diacritics latin1..html | sed -e 's@<pre>@<PRE>@g' -e 's@</pre>@</PRE>@g'  -e 's@<PRE>@\n<PRE>@g' -e 's@</PRE>@\n</PRE>@g' | awk -f ./extract_pre.awk > pba_8.xml.html

run_tutorial_pba_9 : tutorial_pba_9
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_9.out 2>&1 || echo -n
	@ cat tutorial_pba_9.out

run_tutorial_pba_10 : tutorial_pba_10
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_10.out 2>&1 || echo -n
	@ cat tutorial_pba_10.out

run_tutorial_pba_11 : tutorial_pba_11
	@ echo "Running $<..."
	@ ./$< > tutorial_pba_11.tmp 2>&1 || echo -n
	@ cat tutorial_pba_11.tmp | tail -4 > tutorial_pba_11.out
	@ $(RM) tutorial_pba_11.tmp

% : %.cpp
	$(CXX) $(CPPFLAGS) $< -o $@ ${LDFLAGS}

.PHONY : clean_html_code
clean_html_code : 	
	$(RM) tutorial_pba_0.cpp.html
	$(RM) tutorial_pba_1.cpp.html
	$(RM) tutorial_pba_2.cpp.html
	$(RM) tutorial_pba_3.cpp.html
	$(RM) tutorial_pba_4.cpp.html
	$(RM) tutorial_pba_5.cpp.html
	$(RM) tutorial_pba_6.cpp.html
	$(RM) tutorial_pba_7.cpp.html
	$(RM) tutorial_pba_8.cpp.html
	$(RM) tutorial_pba_9.cpp.html
	$(RM) tutorial_pba_10.cpp.html
	$(RM) tutorial_pba_11.cpp.html

.PHONY : html_code
html_code : \
 tutorial_pba_0.cpp.html \
 tutorial_pba_1.cpp.html \
 tutorial_pba_2.cpp.html \
 tutorial_pba_3.cpp.html \
 tutorial_pba_4.cpp.html \
 tutorial_pba_5.cpp.html \
 tutorial_pba_6.cpp.html \
 tutorial_pba_7.cpp.html \
 tutorial_pba_8.cpp.html \
 tutorial_pba_9.cpp.html \
 tutorial_pba_10.cpp.html \
 tutorial_pba_11.cpp.html

%.cpp.html : %.cpp
	./code2html $< > $@


# end of GNUmakefile
