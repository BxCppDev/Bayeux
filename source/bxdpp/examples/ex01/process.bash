#!/usr/bin/env bash

which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Missing Bayeux's bxquery !" >&2
    exit 1
fi

clean=1
help=0
debug=0
html=0
html_file=dpp-ex01_README.html

opwd=$(pwd)

function my_exit()
{
    cd ${opwd}
    exit $1
}
while getopts "dhCH:" opt ;  do
    case $opt in
	d)
	    debug=1
	    ;;
	h)
	    help=1
	    ;;
	C)
	    clean=0
	    ;;
	H)
	    html=1
	    html_file="$OPTARG"
	    ;;
	\?)
	    echo "ERROR: Invalid option: -$OPTARG" >&2
	    my_exit 1
	    ;;
	:)
	    echo "ERROR: Missing argument for option: -$OPTARG" >&2
	    my_exit 1
	    ;;
    esac
done
shift $(($OPTIND -1))
if [ $debug -eq 1 ]; then
    printf "DEBUG: Remaining arguments are: %s\n" "$*" >&2
fi

if [ $help -eq 1 ]; then
    cat<<EOF
dpp -- Example ex01

Usage:
  process.sh [OPTIONS...]

EOF
    my_exit 0
fi

if [ $html -eq 1 ]; then
    which pandoc
    test $? -eq 0 && pandoc -r rst -w html README.rst > ${html_file}
    which firefox
    test $? -eq 0 && firefox file://$(pwd)/${html_file} &
fi

install_dir=$(pwd)/_install.d
test -d ${install_dir} && rm -fr ${install_dir}
build_dir=$(pwd)/_build.d
test -d ${build_dir} && rm -fr ${build_dir}
test -d ${build_dir} || mkdir ${build_dir}
cd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=${install_dir} \
    -DBayeux_DIR=$(bxquery --cmakedir) \
    ..
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed !" 1>&2
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

if [ $debug -eq 1 ]; then
    echo "" 1>&2
    echo "NOTICE: dpp_ex01 DLL linkage is :" 1>&2
    find ${install_dir}/lib -name "libdpp_ex01.*" -exec ldd \{\} \; 1>&2
fi

export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}

echo "" 1>&2
echo "NOTICE: OCD support :" 1>&2
echo "- List of registered class IDs in the dpp_ex01 DLL : " 1>&2
bxocd_manual --load-dll "dpp_ex01@${install_dir}/lib" --action list | grep "dpp_ex01::" 2> /dev/null

echo "- Generate the OCD documentation for class ``dpp_ex01::raw_generator_module`` : " 1>&2
bxocd_manual --load-dll "dpp_ex01@${install_dir}/lib" --action show --class-id dpp_ex01::raw_generator_module > dpp_ex01_RGM.rst
pandoc -r rst -w html dpp_ex01_RGM.rst > dpp_ex01_RGM.html

echo "- Check and validate a sample testing configuration file for class ``dpp_ex01::raw_generator_module`` : " 1>&2
bxocd_manual --load-dll "dpp_ex01@${install_dir}/lib" --class-id dpp_ex01::raw_generator_module --action validate --input-file testing/rgm.conf

echo "" 1>&2
echo "NOTICE: Run the ./test_dpp_ex01 executable :" 1>&2
${install_dir}/test_dpp_ex01
ls -l ./test_dpp_ex01.xml

echo "" 1>&2
echo "NOTICE: Run the bxdpp_processing executable :" 1>&2
bxdpp_processing \
    --load-dll "dpp_ex01@${install_dir}/lib" \
    --modulo 1 \
    --max-records 1000 \
    --module-manager-config "config/module_manager.conf" \
    --module "pipeline" \
    --max-records-per-output-file 250 \
    --output-file dpp_ex01_01.brio \
    --output-file dpp_ex01_02.brio \
    --output-file dpp_ex01_03.brio \
    --output-file dpp_ex01_04.brio
if [ $? -ne 0 ]; then
    echo "ERROR: bxdpp_processing has failed !" 1>&2
    exit 1
fi
ls -l ./dpp_ex01_intermediate_*.brio
ls -l ./dpp_ex01_0?.brio

echo "" 1>&2
echo "NOTICE: Merge and convert data files :" 1>&2
bxdpp_processing \
    --logging "notice" \
    --load-dll "dpp_ex01@${install_dir}/lib" \
    --input-file dpp_ex01_01.brio \
    --input-file dpp_ex01_02.brio \
    --input-file dpp_ex01_03.brio \
    --input-file dpp_ex01_04.brio \
    --output-file dpp_ex01_01-04.data.gz
if [ $? -ne 0 ]; then
    echo "ERROR: bxdpp_processing has failed to merge and convert XML files !" 1>&2
    exit 1
fi

if [ $clean -eq 1 ]; then
    echo "NOTICE: Clean..." 1>&2
    rm -f dpp_ex01_*.rst
    rm -f dpp_ex01_*.html
    rm -f test_dpp_ex01.xml
    rm -f test_dpp_t_ex01.xml
    rm -f test_dpp_t2_ex01.xml
    rm -f dpp_ex01_0?.xml
    rm -f dpp_ex01_0?.brio
    rm -f dpp_ex01_intermediate_*.brio
    rm -f dpp_ex01_01-04.data.gz
    rm -fr ${install_dir}
    rm -fr ${build_dir}
    rm -f README.html
    find . -name "*~" -exec rm -f \{\} \;
fi

my_exit 0

# end
