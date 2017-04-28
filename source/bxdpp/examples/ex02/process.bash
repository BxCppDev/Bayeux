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
html_file=dpp-ex02_README.html

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
genvtx -- Example ex01

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
    -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
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

export LD_LIBRARY_PATH=${install_dir}/lib:${LD_LIBRARY_PATH}

echo "" 1>&2
echo "NOTICE: Run the ex02 executable :" 1>&2
${install_dir}/ex02
ls -l ./ex02*.xml

if [ $clean -eq 1 ]; then
    echo "NOTICE: Clean..." 1>&2
    rm -f dpp-ex02_*.html
    rm -f ex02.xml
    rm -f ex02.brio
    rm -f ex02b.xml
    rm -f ex02b_*.xml
    rm -f ex02c.xml
    rm -f ex02c_*.xml
    rm -fr ${build_dir}
    rm -fr ${install_dir}
    find . -name "*~" -exec rm -f \{\} \;
fi

my_exit 0

# end
