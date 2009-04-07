#!/bin/bash

base_dir=./io_factory
opwd=$(pwd)
cd ../..
datatools_dir=$(pwd)
cd ${opwd}

echo "NOTICE: datatools_dir=${datatools_dir}" 1>&2
inc_dir=${datatools_dir}/include/datatools/serialization
src_dir=${datatools_dir}/src

if [ -d ${base_dir} ]; then
    rm -fr ${base_dir}
fi
mkdir ${base_dir}
mkdir ${base_dir}/include
mkdir ${base_dir}/src
mkdir ${base_dir}/doc
mkdir ${base_dir}/examples

hfiles="io_factory i_serializable safe_serial"
for hfile in ${hfiles}; do
    cat ${inc_dir}/${hfile}.h | \
	sed -e "s/__datatools__serialization//g" \
	-e "s@datatools/serialization/@@g" \
	-e "s@datatools::serialization::@@g" \
	-e "s/namespace datatools/namespace io_factory/g" \
	-e "s/namespace serialization {//g" \
	-e "s@} // end of namespace serialization@@g" \
	-e "s@#ifdef DATATOOLS_USE_TREE_DUMP@@g" \
	-e "s@#endif // DATATOOLS_USE_TREE_DUMP@@g" \
	-e "s@#include <datatools/utils/i_tree_dump.h>@@g" \
	-e "s@: public datatools::utils::i_tree_dumpable@@g" \
	-e "s@virtual // trick@@g" \
	> ${base_dir}/include/${hfile}.h
done

##    -e 's@^[[:space:]*]$@@g' \

cat ${src_dir}/io_factory.cc | \
    sed -e "s/__datatools__serialization//g" \
    -e "s@datatools/serialization/@@g" \
    -e "s/namespace datatools/namespace io_factory/g" \
    -e "s/namespace serialization {//g" \
    -e "s@} // end of namespace serialization@@g" \
    -e "s@.*// tree_trick@@g" \
    > ${base_dir}/src/io_factory.cc

cp foo.h foo2.h test_rw_0.cxx ${base_dir}/examples/
cp GNUmakefile_1 ${base_dir}/examples/GNUmakefile
cp -a boost/  ${base_dir}/include/
cp -a eos_pba/  ${base_dir}/include/

find ${base_dir}/ -type d -a -name ".svn" -exec rm -fr \{\} \; 
tree -a ${base_dir}/

#i_serializable.h
#safe_serial.h
#serialization.h

function aaa ()
{
    #mkdir -p ${base_dir}/boost/vault
    
    FPU_CAT="Math%20-%20Numerics"
    FPU_AR="floating_point_utilities_v3.zip"
    FPU_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=${FPU_AR}&directory=${FPU_CAT}&"
    
    PBA_CAT="serialization"
    PBA_AR="eos_portable_archive.zip"
    PBA_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=${PBA_AR}&directory=${PBA_CAT}&"
    
    ENDIAN_CAT="Integer"
    ENDIAN_AR="endian-0.8.zip"
    ENDIAN_URL="http://www.boostpro.com/vault/index.php?action=downloadfile&filename=${ENDIAN_AR}&directory=${ENDIAN_CAT}&"
    
    wget $(FPU_URL) --output-document=${base_dir}/${FPU_AR}
    wget $(PBA_URL) --output-document=${base_dir}/${PBA_AR}
    wget $(ENDIAN_URL) --output-document=${base_dir}/${ENDIAN_AR}

    cd ${base_dir}
    unzip ${FPU_AR}
    unzip ${PBA_AR}
    unzip ${ENDIAN_AR}   
    cd ${opwd}
}


exit 0

# end
