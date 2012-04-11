

src=../../test_serializable_3.cxx
exe=test_serializable_3

CPPFLAGS="-I../.. $(geomtools-config --cflags)"
LDFLAGS="$(geomtools-config --libs) $(geomtools-config --ldflags)"

echo "CPPFLAGS=${CPPFLAGS}"
echo "LDFLAGS=${LDFLAGS}"

c++ ${CPPFLAGS} ${src} -o ${exe} ${LDFLAGS}



exit 0
#