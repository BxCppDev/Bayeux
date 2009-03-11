# -*- mode: make; -*-

FPU_CAT="Math%20-%20Numerics"
FPU_AR="floating_point_utilities_v3.zip"

FPU_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=$(FPU_AR)&directory=$(FPU_CAT)&"

PBA_CAT="serialization"
PBA_AR="eos_portable_archive.zip"
PBA_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=$(PBA_AR)&directory=$(PBA_CAT)&"

ENDIAN_CAT="Integer"
ENDIAN_AR="endian-0.8.zip"
ENDIAN_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=$(ENDIAN_AR)&directory=$(ENDIAN_CAT)&"

.PHONY : all
all : install

.PHONY : install
install : install_fpu install_eos_pba install_endian

.PHONY : clean
clean : clean_fpu clean_eos_pba clean_endian
	find . -name "*~" -exec $(RM) \{\} \;

.PHONY : install_fpu
install_fpu :
	unzip floating_point_utilities_v3.zip
	ln -s $(shell pwd)/floating_point_utilities_v3 ./fpu

.PHONY : install_eos_pba
install_eos_pba :
	unzip eos_portable_archive.zip
	ln -s $(shell pwd)/eos ./eos_pba

.PHONY : install_endian
install_endian :
	unzip endian-0.8.zip
	ln -s $(shell pwd)/endian-0.8 ./endian

.PHONY : clean_fpu
clean_fpu :
	test -L ./fpu && $(RM) -f ./fpu || echo ""
	test -d floating_point_utilities_v3/ && $(RM) -fr floating_point_utilities_v3/ || echo ""

.PHONY : clean_eos_pba
clean_eos_pba :
	test -L ./eos_pba && $(RM) -f ./eos_pba || echo ""
	test -d eos/ && $(RM) -fr eos/ || echo ""

.PHONY : clean_endian
clean_endian :
	test -L ./endian && $(RM) -f ./endian || echo ""
	test -d endian-0.8/ && $(RM) -fr endian-0.8/ || echo ""

.PHONY : download_fpu
download_fpu :
	wget $(FPU_URL) --output-document=floating_point_utilities_v3.zip

.PHONY : download_eos_pba
download_eos_pba :
	wget $(PBA_URL) --output-document=eos_portable_archive.zip

.PHONY : download_endian
download_endian :
	wget $(ENDIAN_URL) --output-document=endian-0.8.zip


# end