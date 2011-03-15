#!/usr/bin/env bash

DEFHERE=
#DEFHERE=-DHERE

CPPFLAGS_A="${DEFHERE} -I../../include/ -I. $(boost-config --cflags)"
LDFLAGS_A="-L./lib -lA $(boost-config --ldflags serialization)"
CPPFLAGS_B="-I. ${CPPFLAGS_A}"
LDFLAGS_B="-L./lib -lB ${LDFLAGS_A}"

echo "CPPFLAGS_A = '${CPPFLAGS_A}'" >&2
echo "LDFLAGS_A  = '${LDFLAGS_A}'" >&2
echo "CPPFLAGS_B = '${CPPFLAGS_B}'" >&2
echo "LDFLAGS_B  = '${LDFLAGS_B}'" >&2

test -d lib/ || mkdir ./lib
rm -f Ac1.o
rm -f Ac2.o
rm -f libA.a
rm -f libA.so
g++ -c ${CPPFLAGS_A} Ac1.cpp -o Ac1.o
g++ -c ${CPPFLAGS_A} Ac2.cpp -o Ac2.o
g++ -shared -fPIC Ac1.o Ac2.o -o ./lib/libA.so
g++ ${CPPFLAGS_A} Aprg0.cxx ${LDFLAGS_A} -o Aprg0
g++ ${CPPFLAGS_A} Aprg1.cxx ${LDFLAGS_A} -o Aprg1

g++ -c ${CPPFLAGS_B} Bc3.cpp -o Bc3.o
g++ -shared -fPIC Bc3.o -o ./lib/libB.so
g++ ${CPPFLAGS_B} Bprg0.cxx ${LDFLAGS_B} -o Bprg0
g++ ${CPPFLAGS_B} Bprg1.cxx ${LDFLAGS_B} -o Bprg1

exit 0

# end