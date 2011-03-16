#!/usr/bin/env bash


rm -f *.o
rm -f lib/lib*.a
rm -f lib/lib*.so
rm -f lib/quasi_portable_binary_*.o
rm -f Aprg0
rm -f Aprg1
rm -f Aprg1.txt
rm -f Bprg0
rm -f Bprg1
rm -f Bprg1bis
rm -f Bprg1.txt
rm -f Bprg1.xml
rm -f Bprg2
rm -f Bprg2.data

find . -name "*~" -exec rm -f \{\} \;

test -d lib && rmdir lib


exit 0

# end