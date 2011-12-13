#

CPPFLAGS=$(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : bin

.PHONY : bin
bin : \
  test_properties

.PHONY : clean
clean :
	$(RM) *~
	$(RM) test_properties test_properties.conf test_properties.xml

.PHONY : run
run : test_properties
	echo "my" "favorite" "color" "is" "blue" | ./$< 

test_properties : ../test_properties.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end

