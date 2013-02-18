#!/usr/bin/env bash

make gen_sample_data

DPP_DLL_PATH=../../__build-Linux-x86_64/src/
LD_LIBRARY_PATH=${DPP_DLL_PATH}:${LD_LIBRARY_PATH}

./gen_sample_data  data_0.xml

sleep 1

./gen_sample_data -10 data_0.brio

sleep 1

./gen_sample_data -10 data_0.txt.gz
sleep 1
./gen_sample_data -10 data_1.txt.gz
sleep 1
./gen_sample_data -10 data_2.txt.gz
sleep 1
./gen_sample_data -10 data_3.txt.gz
sleep 1


make clean

exit 0

# end
