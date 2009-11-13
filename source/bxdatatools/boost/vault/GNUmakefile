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

SER_BITSET_CAT="serialization"
SER_BITSET_AR="ser_bitset1.zip"
SER_BITSET_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=$(SER_BITSET_AR)&directory=$(SER_BITSET_CAT)&"

BITSET_AND_MORE_CAT="serialization"
BITSET_AND_MORE_AR="bitset_and_more.zip"
BITSET_AND_MORE_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=$(BITSET_AND_MORE_AR)&directory=$(BITSET_AND_MORE_CAT)&"

.PHONY : all
all : install

.PHONY : install
install : install__fpu install__eos_pba install__endian #install__ser_bitset

.PHONY : clean
clean : clean__fpu clean__eos_pba clean__endian #clean__ser_bitset
	find . -name "*~" -exec $(RM) \{\} \;

.PHONY : install__fpu
install__fpu :
	unzip $(FPU_AR)
	ln -s $(shell pwd)/floating_point_utilities_v3 ./fpu

.PHONY : install__eos_pba
install__eos_pba :
	unzip $(PBA_AR)
	ln -s $(shell pwd)/eos ./eos_pba

# .PHONY : install__ser_bitset
# install__ser_bitset :
# 	mkdir ./ser_bitset1
# 	cd ./ser_bitset1; unzip ../$(SER_BITSET_AR); cd ..
# 	ln -s $(shell pwd)/ser_bitset1 ./ser_bitset

#.PHONY : install__bitset_and_more
#install__bitset_and_more :
#	mkdir ./bitset_and_more
#	cd ./bitset_and_more; unzip ../$(BITSET_AND_MORE_AR); cd ..
#	#ln -s $(shell pwd)/bitset_and_more ./bitset_and_more

.PHONY : install__endian
install__endian :
	unzip $(ENDIAN_AR)
	ln -s $(shell pwd)/endian-0.8 ./endian

.PHONY : clean__fpu
clean__fpu :
	test -L ./fpu && $(RM) -f ./fpu || echo ""
	test -d floating_point_utilities_v3/ && $(RM) -fr floating_point_utilities_v3/ || echo ""

.PHONY : clean__eos_pba
clean__eos_pba :
	test -L ./eos_pba && $(RM) -f ./eos_pba || echo ""
	test -d eos/ && $(RM) -fr eos/ || echo ""

.PHONY : clean__endian
clean__endian :
	test -L ./endian && $(RM) -f ./endian || echo ""
	test -d endian-0.8/ && $(RM) -fr endian-0.8/ || echo ""

# .PHONY : clean__ser_bitset
# clean__ser_bitset :
# 	test -L ./ser_bitset && $(RM) -f ./ser_bitset || echo ""
# 	test -d ser_bitset1/ && $(RM) -fr ser_bitset1/ || echo ""

#.PHONY : clean__bitset_and_more
#clean__bitset_and_more :
#	test -d bitset_and_more/ && $(RM) -fr bitset_and_more/ || echo ""

.PHONY : download__fpu
download__fpu :
	wget $(FPU_URL) --output-document=floating_point_utilities_v3.zip

.PHONY : download__eos_pba
download__eos_pba :
	wget $(PBA_URL) --output-document=eos_portable_archive.zip

.PHONY : download__endian
download__endian :
	wget $(ENDIAN_URL) --output-document=endian-0.8.zip

.PHONY : download__ser_bitset
download__ser_bitset :
	wget $(SER_BITSET_URL) --output-document=ser_bitset1.zip

#.PHONY : download__bitset_and_more
#download__bitset_and_more :
#	wget $(BITSET_AND_MORE_URL) --output-document=bitset_and_more.zip


# end