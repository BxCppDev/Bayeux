# -*- mode: make; -*-

.PHONY : all
all : install

.PHONY : install
install : install_fpu

.PHONY : clean
clean : clean_fpu
	find . -name "*~" -exec $(RM) \{\} \;

.PHONY : install_fpu
install_fpu :
	unzip floating_point_utilities_v3.zip
	ln -s $(shell pwd)/floating_point_utilities_v3 ./fpu

.PHONY : clean_fpu
clean_fpu :
	test -L ./fpu && $(RM) -f ./fpu || echo ""
	test -d floating_point_utilities_v3/ && $(RM) -fr floating_point_utilities_v3/ || echo ""

.PHONY : download
download :
	wget "http://www.boostpro.com/vault/index.php?action=downloadfile&filename=floating_point_utilities_v3.zip&directory=Math%20-%20Numerics&" --output-document=floating_point_utilities_v3.zip

# end