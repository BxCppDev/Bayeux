#

CPPFLAGS=$(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : prg1


.PHONY : clean
clean : 
	$(RM) *~
	$(RM) stored_data.txt
	$(RM) prg1


prg1 : prg1.cxx data.h
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end
