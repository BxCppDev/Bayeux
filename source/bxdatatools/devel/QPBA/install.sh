#!/usr/bin/env bash

datatools_include=../../include
datatools_src=../../src


test -d ${datatools_include}/boost/qpba || mkdir -p ${datatools_include}/boost/qpba

cp -f boost/qpba/*.hpp ${datatools_include}/boost/qpba/
cp -f libs/src/quasi_portable_binary_iarchive.cpp  ${datatools_src}/quasi_portable_binary_iarchive.cc
cp -f libs/src/quasi_portable_binary_oarchive.cpp  ${datatools_src}/quasi_portable_binary_oarchive.cc
cp -f libs/src/quasi_portable_binary_archive_common.cpp  ${datatools_src}/quasi_portable_binary_archive_common.cc

exit 0

# end