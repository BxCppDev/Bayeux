# 
# Note: the 'boost-config' utility is a home made script that prints
# the proper list of switches to compile and link againts Boost/Serialization.
# You should here use your own pathes such as : 
#   BOOST_CFLAGS="-I<boost_include_path>"
#   BOOST_LDFLAGS="-L<boost_lib_path> -lboost_serialization"

BOOST_CFLAGS=$(shell boost-config --cflags)
BOOST_LDFLAGS=$(shell boost-config --ldflags serialization)

CPPFLAG_AR=-DINCLUDE_ARCHIVES_BASE
#CPPFLAG_AR=

CPPFLAGS_A=-I. $(CPPFLAG_AR) $(BOOST_CFLAGS)
LDFLAGS_A=-L. -lA $(BOOST_LDFLAGS)
CPPFLAGS_B=-I. $(CPPFLAGS_A)
LDFLAGS_B=-L. -lB $(LDFLAGS_A) #$(shell root-config --libs)

.PHONY : all
all : runs

.PHONY : runs 
runs : bins
	@echo "\nRunning Aprg0..."
	@./Aprg0 || echo "Failure !"
	@echo "\nRunning Aprg1..."
	@./Aprg1 || echo "Failure !"
	@echo "\nRunning Bprg0..."
	@./Bprg0 || echo "Failure !"
	@echo "\nRunning Bprg1..."
	@./Bprg1 || echo "Failure !"
	@echo "\nRunning Bprg1bis..."
	@./Bprg1bis || echo "Failure !"
	@echo "\nRunning Bprg1ter..."
	@./Bprg1ter || echo "Failure !"
	@echo "\nRunning Bprg2..."
	@./Bprg2 || echo "Failure !"

.PHONY : bins 
bins : Aprg0 Aprg1 Bprg0 Bprg1 Bprg1bis Bprg1ter Bprg2

.PHONY : libs 
libs : libA.so

.PHONY : clean
clean :
	@rm -f Ac1.o 
	@rm -f Ac2.o 
	@rm -f libA.so 
	@rm -f Aprg0 
	@rm -f Aprg1
	@rm -f Aprg1.txt
	@rm -f Bc3.o 
	@rm -f libB.so 
	@rm -f Bprg0 
	@rm -f Bprg1
	@rm -f Bprg1bis
	@rm -f Bprg1ter
	@rm -f Bprg2
	@rm -f Bprg1.txt Bprg1.xml Bprg2.data
	@rm -f *~

libB.so : Bc3.o
	g++ -shared Bc3.o -o libB.so

Bc3.o : Bc3.cpp
	g++ -c ${CPPFLAGS_B} $< -o $@

Bprg0 : Bprg0.cxx libB.so libA.so
	g++ ${CPPFLAGS_B} $< ${LDFLAGS_B} -o $@

Bprg1 : Bprg1.cxx libB.so libA.so
	g++ ${CPPFLAGS_B} $< ${LDFLAGS_B} -o $@

Bprg1bis : Bprg1bis.cxx libB.so libA.so
	g++ ${CPPFLAGS_B} $< ${LDFLAGS_B} -o $@

Bprg1ter : Bprg1ter.cxx libB.so libA.so
	g++ ${CPPFLAGS_B} $< ${LDFLAGS_B} -o $@

Bprg2 : Bprg2.cxx libB.so libA.so
	g++ ${CPPFLAGS_B} $< ${LDFLAGS_B} -o $@

libA.so : Ac2.o Ac1.o
	g++ -shared Ac2.o Ac1.o -o libA.so

Ac2.o : Ac2.cpp
	g++ -c ${CPPFLAGS_A} $< -o $@

Ac1.o : Ac1.cpp
	g++ -c ${CPPFLAGS_A} $< -o $@

Aprg0 : Aprg0.cxx libA.so
	g++ ${CPPFLAGS_A} $< ${LDFLAGS_A} -o $@

Aprg1 : Aprg1.cxx libA.so
	g++ ${CPPFLAGS_A} $< ${LDFLAGS_A} -o $@

# end
