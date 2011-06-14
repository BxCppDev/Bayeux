#!/usr/bin/env bash

which boost-config >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Boost is not setup (boost_help) ! Abort !" >&2
    exit 1
fi

# My BOOST_CFLAGS is : 
#  `-I/scratch/sw/boost/install-1_44_0-Linux-i686-gcc45/include'
# and BOOST_LDFLAGS is :
# `-fPIC -L/scratch/sw/boost/install-1_44_0-Linux-i686-gcc45/lib -lboost_serialization'
# It is obtained for a home-made script, please change it
# to suit your own Boost setup :
TRUNK=
if [ "x$1" = "x--trunk" ]; then
    TRUNK=--trunk
fi
BOOST_CFLAGS=$(boost-config --cflags ${TRUNK})
BOOST_LDFLAGS=$(boost-config --ldflags ${TRUNK} serialization)

CPPFLAGS_A="-I../../include/ -I. ${BOOST_CFLAGS}"
LDFLAGS_A="-L./lib -lA ${objs} ${BOOST_LDFLAGS}"
CPPFLAGS_B="-I. ${CPPFLAGS_A}"
LDFLAGS_B="-L./lib -lB ${LDFLAGS_A}"

echo "CPPFLAGS_A = '${CPPFLAGS_A}'" >&2
echo "LDFLAGS_A  = '${LDFLAGS_A}'" >&2
echo "CPPFLAGS_B = '${CPPFLAGS_B}'" >&2
echo "LDFLAGS_B  = '${LDFLAGS_B}'" >&2

test -d lib/ || mkdir ./lib
rm -f Ac1.o
rm -f Ac2.o
rm -f Ad1.o
rm -f Aio.o
rm -f libA.a
rm -f libA.so
g++ -c ${CPPFLAGS_A} Ac1.cpp -o Ac1.o
g++ -c ${CPPFLAGS_A} Ac2.cpp -o Ac2.o
g++ -c ${CPPFLAGS_A} Ad1.cpp -o Ad1.o
g++ -c ${CPPFLAGS_A} Aio.cpp -o Aio.o
g++ -shared -fPIC Ad1.o Ac1.o Ac2.o Aio.o -o ./lib/libA.so
g++ ${CPPFLAGS_A} Aprg0.cxx ${LDFLAGS_A} -o Aprg0
g++ ${CPPFLAGS_A} Aprg1.cxx ${LDFLAGS_A} -o Aprg1
g++ ${CPPFLAGS_A} Aprg2.cxx ${LDFLAGS_A} -o Aprg2

g++ -c ${CPPFLAGS_B} Bc3.cpp -o Bc3.o
g++ -c ${CPPFLAGS_A} Bio.cpp -o Bio.o
g++ -shared -fPIC Bc3.o Bio.o -o ./lib/libB.so
g++ ${CPPFLAGS_B} Bprg0.cxx ${LDFLAGS_B} -o Bprg0
g++ ${CPPFLAGS_B} Bprg1.cxx ${LDFLAGS_B} -o Bprg1
g++ ${CPPFLAGS_B} Bprg1bis.cxx ${LDFLAGS_B} -o Bprg1bis
g++ ${CPPFLAGS_B} Bprg2.cxx ${LDFLAGS_B} -o Bprg2
g++ ${CPPFLAGS_B} Bprg3.cxx ${LDFLAGS_B} -o Bprg3

exit 0

# end