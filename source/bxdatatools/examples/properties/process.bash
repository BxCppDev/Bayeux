#!/usr/bin/env bash

which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Missing Bayeux's bxquery !" >&2
    exit 1
fi

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=.. \
    -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
    ..
make
make install

cd ${opwd}

./ex_properties --debug

cat foo.conf

rm -f foo.conf
rm -f ex_properties
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;

exit 0

# end
