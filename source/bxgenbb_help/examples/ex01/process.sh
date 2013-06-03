#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Dgenbb_help_DIR=$(genbb_help-config --prefix) \
  ..
make
make install

cd ..
ls -l

echo "Running ex01..." 1>&2
./ex01

echo "List available event generators..." 1>&2
genbb_inspector \
    --configuration config/manager.conf \
    --action list

echo "Shoot events..." 1>&2
genbb_inspector \
    --configuration config/manager.conf \
    --action shoot \
    --generator "Co60" \
    --prng-seed 314159 \
    --number-of-events 55 \
    --output-file "histos_Co60.root"

rm -f ./ex01
rm -f ./histos_Co60.root
rm -fr ${build_dir}

cd ${opwd}

exit 0

# end