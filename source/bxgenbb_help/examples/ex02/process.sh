#!/usr/bin/env bash
do_clean=1

while [ -n "$1" ]; do
    token="$1"
    if [ "x${token}" == "x--no-clean" -o "x${token}" == "x-C" ]; then
	do_clean=0
    elif [ "x${token}" == "x--clean" -o "x${token}" == "x-c" ]; then
	do_clean=1
    fi
    shift 1
done

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
tree -l ${install_dir} config

echo "Running ex02..." 1>&2
${install_dir}/bin/ex02
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex02 failed !" 1>&2
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

echo "Shoot events..." 1>&2
LD_LIBRARY_PATH=${install_dir}/lib:${LD_LIBRARY_PATH} \
    ${genbb_inspector_bin} \
    --load-dll "genbb_help_ex02" \
    --configuration "config/manager.conf" \
    --action "shoot" \
    --generator "Fool" \
    --prng-seed 314159 \
    --number-of-events 10000 \
    --modulo 1000 \
    --prompt \
    --delayed \
    --prompt-time-limit 1 \
    --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
    --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf" \
    --output-file "histos_Fool.root"
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot events failed !" 1>&2
    my_exit 1
fi

if [ $do_clean -eq 1 ]; then
    rm -f ./histos_Fool.root
    rm -fr ${build_dir}
    rm -fr ${install_dir}
fi

cd ${opwd}

my_exit 0

# end
