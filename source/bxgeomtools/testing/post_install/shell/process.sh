

src=../../test_serializable_2.cxx
exe=test_serializable_2

CPPFLAGS="-I../.. $(geomtools-config --cflags)"
LDFLAGS="$(geomtools-config --ldflags)"

echo "CPPFLAGS=${CPPFLAGS}"
echo "LDFLAGS=${LDFLAGS}"

c++ ${CPPFLAGS} ${src} -o ${exe} ${LDFLAGS}



exit 0
#