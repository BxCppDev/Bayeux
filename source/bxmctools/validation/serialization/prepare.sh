#!/usr/bin/env bash

do_clean=1
only_build=0
do_store=0
serial_downgrade=0
do_doc=0

while [ -n "$1" ]; do
    token="$1"
    if [ "x${token}" == "x--no-clean" -o "x${token}" == "x-C" ]; then
	do_clean=0
    elif [ "x${token}" == "x--clean" -o "x${token}" == "x-c" ]; then
	do_clean=1
    elif [ "x${token}" == "x--only-build" -o "x${token}" == "x-o" ]; then
	only_build=1
    elif [ "x${token}" == "x--downgrade" ]; then
	serial_downgrade=1
    elif [ "x${token}" == "x--store" ]; then
	do_store=1
    elif [ "x${token}" == "x--doc" ]; then
	do_doc=1
    fi
    shift 1
done

opwd=$(pwd)

install_dir=$(pwd)/_install.d
test -d ${install_dir} && rm -fr ${install_dir}

build_dir=$(pwd)/_build.d
test -d ${build_dir} && rm -fr ${build_dir}

if [ ${do_doc} -eq 1 ]; then
    which pandoc > /dev/null 2<&1
    if [ $? -eq 0 ]; then
	echo -e "\nBuild the HTML README document..." 1>&2
	pandoc -r rst -w html README.rst -o mctools-exserialization_README.html
	# which xdg-open > /dev/null 2<&1
	# if [ $? -eq 0 ]; then
	# 	echo -e "\nBrowse the HTML README document..." 1>&2
	# 	xdg-open mctools-exserialization_README.html &
	# fi
    fi
fi

mkdir ${build_dir}
cd ${build_dir}

echo -e "\nBuild the validation programs..." 1>&2
cmake \
    -DCMAKE_INSTALL_PREFIX=${install_dir} \
    -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
    -DMCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE=${serial_downgrade} \
    ..
if [ $? -ne 0 ]; then
    echo "ERROR: cmake failed !" 1>&2
    exit 1
fi
make
if [ $? -ne 0 ]; then
    echo "ERROR: make failed !" 1>&2
    exit 1
fi
# make install
# if [ $? -ne 0 ]; then
#     echo "ERROR: make install failed !" 1>&2
#     exit 1
# fi

if [ ${only_build} -eq 1 ]; then
    exit 0
fi
cd ..
ls -l

if [ ${do_store} -eq 1 ]; then
    echo -e "\nGenerate the output MC hit data file..." 1>&2
    ${build_dir}/exstore
    if [ $? -ne 0 ]; then
	echo "[error] exstore failed !" 1>&2
	exit 1
    fi
fi

downgrade_opt=""
if [ "${serial_downgrade}" -eq 1 ]; then
    downgrade_opt="--downgrade"
fi

$(pwd)/run.sh ${downgrade_opt} --source-dir $(pwd) --work-dir ${build_dir}
if [ $? -ne 0 ]; then
    echo "[error] run failed !" 1>&2    
    exit 1
fi

if [ ${do_clean} -eq 1 ]; then
    rm -f out-1.xml
    rm -f out-2.xml
    rm -f check-out-1.xml
    rm -f check-out-2.xml
    rm -f test-1.txt
    rm -f test-2.txt
    rm -f mctools-exserialization_README.html
    rm -fr ${install_dir}
    rm -fr ${build_dir}
    find . -name "*~" -exec rm -f \{\} \;
fi

cd ${opwd}

exit 0

# end
