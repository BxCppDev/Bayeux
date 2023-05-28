#!/usr/bin/env bash

make gen_sample_data

DPP_DLL_PATH=../../__build-Linux-x86_64/src/
LD_LIBRARY_PATH=${DPP_DLL_PATH}:${LD_LIBRARY_PATH}

DATA_PATH=../data

datatools_majver=$(datatools-config --version | cut -d. -f1)
if [ ${datatools_majver} -lt 5 ]; then
    echo "build.bash: Generating 'legacy' data..." >&2
    DATA_PATH=../legacy_data  
    if [ ! -d ${DATA_PATH} ]; then
	mkdir ${DATA_PATH}
    fi
fi
./gen_sample_data ${DATA_PATH}/data_0.xml

sleep 1

./gen_sample_data -10 ${DATA_PATH}/data_0.brio

sleep 1

./gen_sample_data -10 ${DATA_PATH}/data_0.txt.gz
sleep 1
./gen_sample_data -10 ${DATA_PATH}/data_1.txt.gz
sleep 1
./gen_sample_data -10 ${DATA_PATH}/data_2.txt.gz
sleep 1
./gen_sample_data -10 ${DATA_PATH}/data_3.txt.gz
sleep 1


make clean

exit 0

# end
