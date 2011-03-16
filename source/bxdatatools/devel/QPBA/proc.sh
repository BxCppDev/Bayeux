#!/usr/bin/env bash

which boost-config >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Boost is not setup (boost_help) ! Abort !" >&2
    exit 1
fi

CPPFLAGS="-I./boost/qpba -I../../boost/vault/fpu -I../../boost/vault/endian -I. $(boost-config --cflags)"
LDFLAGS="$(boost-config --ldflags serialization)"

echo "CPPFLAGS = '${CPPFLAGS}'" >&2
echo "LDFLAGS  = '${LDFLAGS}'" >&2

g++ -c ${CPPFLAGS} libs/src/quasi_portable_binary_oarchive.cpp -o quasi_portable_binary_oarchive.o

g++ -c ${CPPFLAGS} libs/src/quasi_portable_binary_iarchive.cpp -o quasi_portable_binary_iarchive.o

g++ -c ${CPPFLAGS} libs/src/quasi_portable_binary_archive_common.cpp -o quasi_portable_binary_archive_common.o

g++ ${CPPFLAGS} prg0.cxx ${LDFLAGS} -o prg0
g++ ${CPPFLAGS} prg1.cxx ${LDFLAGS} quasi_portable_binary_oarchive.o quasi_portable_binary_iarchive.o quasi_portable_binary_archive_common.o -o prg1

mkdir ../virtual_serialization/lib/
cp *.o ../virtual_serialization/lib/

exit 0

# end