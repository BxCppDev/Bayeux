#!/usr/bin/env bash

clean=1

if [ "x$1" = "x--no-clean" ]; then
    clean=0
    shift 1
fi

which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Missing Bayeux's bxquery !" >&2
    exit 1
fi

opwd=$(pwd)

function my_exit()
{
    cd ${opwd}
    exit $1
}

install_dir=$(pwd)/_install.d
test -d ${install_dir} && rm -fr ${install_dir}

build_dir=$(pwd)/_build.d
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=${install_dir} \
    -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
    ..
if [ $? -ne 0 ]; then
    echo "ERROR: Configuration failed !" 1>&2
    my_exit 1
fi
make
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed !" 1>&2
    my_exit 1
fi
make install
if [ $? -ne 0 ]; then
    echo "ERROR: Installation failed !" 1>&2
    my_exit 1
fi

cd ${opwd}
ls -l

echo "Running ex01..." 1>&2
${install_dir}/ex01
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex01 failed !" 1>&2
    my_exit 1
fi
genbb_inspector_bin="bxgenbb_inspector"

which ${genbb_inspector_bin} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: No '${genbb_inspector_bin}' executable !" 1>&2
    my_exit 1
fi

echo "List available event generators..." 1>&2
${genbb_inspector_bin} \
    --configuration config/manager.conf \
    --action list
if [ $? -ne 0 ]; then
    echo "ERROR: List event generators failed !" 1>&2
    my_exit 1
fi

echo "Shoot Co60 events..." 1>&2
${genbb_inspector_bin} \
    --configuration config/manager.conf \
    --action shoot \
    --generator "Co60" \
    --prng-seed 314159 \
    --number-of-events 100 \
    --output-mode "histograms" \
    --output-file "histos_Co60.root"
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot events failed !" 1>&2
    my_exit 1
fi

echo "" 1>&2
echo "Shoot only one bb2nu_Se82_HE event..." 1>&2
${genbb_inspector_bin} \
    --configuration config/manager.conf \
    --action shoot \
    --generator "bb2nu_Se82_HE" \
    --prng-seed 314159 \
    --number-of-events 1 \
    --output-mode "plain" \
    --output-file "histos_bb2nu_Se82_HE_one_shot.xml"
if [ $? -ne 0 ]; then
    echo "ERROR: One shoot event failed !" 1>&2
    my_exit 1
fi

echo "" 1>&2
echo "Shoot many bb2nu_Se82_HE events..." 1>&2
${genbb_inspector_bin} \
    --configuration config/manager.conf \
    --action shoot \
    --generator "bb2nu_Se82_HE" \
    --prng-seed 314159 \
    --number-of-events 27 \
    --output-mode "bank" \
    --output-bank-max-records 10 \
    --output-file "histos_bb2nu_Se82_HE_0.xml" \
    --output-file "histos_bb2nu_Se82_HE_1.xml" \
    --output-file "histos_bb2nu_Se82_HE_2.xml"
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot events failed !" 1>&2
    my_exit 1
fi

if [ $clean -eq 1 ]; then
    rm -f ./ex01.xml
    rm -f ./histos_Co60.root
    rm -f ./histos_bb2nu_Se82_HE_one_shot.xml
    rm -f ./histos_bb2nu_Se82_HE_?.xml
    rm -fr ${build_dir}
    rm -fr ${install_dir}
fi

cd ${opwd}

my_exit 0

# end
