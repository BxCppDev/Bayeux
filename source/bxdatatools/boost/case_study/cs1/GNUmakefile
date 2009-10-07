CPPFLAGS+=-I.                    
CPPFLAGS+=$(shell boost-config --cflags)
LDFLAGS=$(shell boost-config --ldflags serialization iostreams filesystem)

.PHONY : all
all : tracking_write_0 \
      tracking_write_1 \
      tracking_write_2 \
      tracking_write_3 \
      tracking_1 tracking_2 multiple_archives_per_file_0 multiple_archives_per_file_1

% : %.cpp | foo.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

.PHONY : clean
clean :
	@find . -name "*~" -exec $(RM) \{\} \;
	@find . -type f -a -perm /u=x -exec $(RM) \{\} \;
	@$(RM) *.txt *.xml *.bin

.PHONY : show
show :
	firefox --new-window file://$(shell pwd)/index.htm &

# end