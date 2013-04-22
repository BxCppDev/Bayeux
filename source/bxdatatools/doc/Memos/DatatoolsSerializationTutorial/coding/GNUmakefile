# GNUmakefile

CPPFLAGS=$(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : \
  data_1 \
  data_2 \
  data_3 \
  data_3X \
  data_4 \
  data_5 \
  data_6

.PHONY : clean
clean :
	$(RM) *~
	$(RM) *.log
	$(RM) data.o
	$(RM) sdata.o
	$(RM) sdata2.o
	$(RM) data_1
	$(RM) data_2
	$(RM) data_3
	$(RM) data_3X
	$(RM) data_4
	$(RM) data_5
	$(RM) data_6
	$(RM) *.out
	$(RM) stored_data.txt
	$(RM) stored_data.xml
	$(RM) data_3.txt
	$(RM) data_3.xml
	$(RM) data_4.txt
	$(RM) data_6.txt

.PHONY : run
run : \
  run_1 \
  run_2 \
  run_3 \
  run_3X \
  run_4 \
  run_5 \
  run_6


.PHONY : run_1
run_1 : data_1
	./data_1 > data_1.out 2>&1

.PHONY : run_2
run_2 : data_2
	./data_2 > data_2.out 2>&1

.PHONY : run_3
run_3 : data_3
	./data_3 > data_3.out 2>&1

.PHONY : run_3X
run_3X : data_3X
	./data_3X > data_3.out 2>&1

.PHONY : run_4
run_4 : data_4
	./data_4 > data_4.out 2>&1

.PHONY : run_5
run_5 : data_5
	./data_5 > data_5.out 2>&1

.PHONY : run_6
run_6 : data_6
	./data_6 > data_6.out 2>&1

data_1 : data_1.cxx data.o
	$(CXX) $(CPPFLAGS) $< data.o $(LDFLAGS) -o $@

data_2 : data_2.cxx data.o
	$(CXX) $(CPPFLAGS) $< data.o $(LDFLAGS) -o $@

data_3 : data_3.cxx data.o
	$(CXX) $(CPPFLAGS) $< data.o $(LDFLAGS) -o $@

data_3X : data_3X.cxx data.o
	$(CXX) $(CPPFLAGS) $< data.o $(LDFLAGS) -o $@

data_4 : data_4.cxx sdata.o
	$(CXX) $(CPPFLAGS) $< sdata.o $(LDFLAGS) -o $@

data_5 : data_5.cxx sdata2.o
	$(CXX) $(CPPFLAGS) $< sdata2.o $(LDFLAGS) -o $@

data_6 : data_6.cxx
	$(CXX) $(CPPFLAGS) $< $(LDFLAGS) -o $@

data.o : data.cc
	$(CXX) -c $(CPPFLAGS) $< -o $@

sdata.o : sdata.cc
	$(CXX) -c $(CPPFLAGS) $< -o $@

sdata2.o : sdata2.cc
	$(CXX) -c $(CPPFLAGS) $< -o $@

# end of GNUmakefile
