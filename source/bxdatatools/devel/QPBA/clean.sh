#!/usr/bin/env bash

rm -f prg0
rm -f prg1
rm -f quasi_portable_binary_oarchive.o
rm -f quasi_portable_binary_iarchive.o
rm -f quasi_portable_binary_archive_common.o
rm -f prg1.data
rm -f prg1.xml
rm -f prg1.txt
find ./ -name "*~"  -exec rm -f \{\} \;


exit 0

# end