#

CPPFLAGS=-I../.. $(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : test_properties test_serializable_1

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_properties test_properties.conf test_properties.xml
	$(RM) test_serializable_1 test_serializable_1.txt

.PHONY : run
run : run1 run2

.PHONY : run1
run1 : test_properties
	echo "my" "favorite" "color" "is" "blue" | ./$< 

.PHONY : run2
run2 : test_serializable_1
	echo "my" "favorite" "color" "is" "blue" | ./$< 

test_properties : ../../test_properties.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

test_serializable_1 : ../../test_serializable_1.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

