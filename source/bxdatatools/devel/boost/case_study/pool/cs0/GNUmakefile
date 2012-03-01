CPPFLAGS+=-I.                    
CPPFLAGS+=$(shell boost-config --cflags)
LDFLAGS=$(shell boost-config --ldflags serialization iostreams filesystem)

.PHONY : all
all : prg_mt0


% : %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

.PHONY : clean
clean :
	@find . -name "*~" -exec $(RM) \{\} \;
	@find . -type f -a -perm u=x ###-exec $(RM) \{\} \;
	@$(RM) *.txt *.xml *.bin

# end