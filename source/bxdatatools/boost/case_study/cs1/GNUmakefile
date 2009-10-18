# (C) Copyright 2009 François Mauger <mauger@lpccaen.in2p3.fr>
# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

CPPFLAGS+=-I.      

# Please change the following variables to take into account
# the proper Boost include and link pathes on your system.
#               
CPPFLAGS+=$(shell boost-config --cflags)
LDFLAGS=$(shell boost-config --ldflags serialization iostreams filesystem)

.PHONY : all
all : \
      tracking_write_0 \
      tracking_write_1 \
      tracking_write_2 \
      tracking_write_3 \
      tracking_write_4 \
      tracking_read_0  \
      tracking_read_2  \
      tracking_read_3  \
      tracking_read_4 

% : %.cpp | foo.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

.PHONY : clean
clean :
	@find . -name "*~" -exec $(RM) \{\} \;
	@find . -name "*.fig.bak" -exec $(RM) \{\} \;
	@$(RM) tracking_write_0
	@$(RM) tracking_write_1
	@$(RM) tracking_write_2
	@$(RM) tracking_write_3
	@$(RM) tracking_write_4
	@$(RM) tracking_read_0
	@$(RM) tracking_read_2
	@$(RM) tracking_read_3
	@$(RM) tracking_read_4
	@$(RM) data_*.xml

.PHONY : clean_images
clean_images :
	cd doc/images; make clean

.PHONY : build_images
build_images :
	cd doc/images; make

.PHONY : show
show :
	firefox --new-window file://$(shell pwd)/index.htm &

# end