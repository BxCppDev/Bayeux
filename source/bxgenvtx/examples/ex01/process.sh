#!/usr/bin/env bash


clean=1
help=0
html=0
html_file=genvtx-ex01_README.html

opwd=$(pwd)

while getopts "hCH:" "$@" ;  do
    case $opt in
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
	    cd ${opwd}
	    exit 1
	    ;;
	:)
	    echo "ERROR: Missing argument for option: -$OPTARG" >&2

    esac
done
shift $(( OPTIND - 1 ));

if [ $help -eq 1 ]; then
    cat<<EOF
genvtx -- Example ex01

Usage:
  process.sh [OPTIONS...]

EOF
fi

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

if [ $html -eq 1 ]; then
    pandoc -r rst -w html README.rst -o ${html_file}
fi

mkdir ${build_dir}
cd ${build_dir}

cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Dgenvtx_DIR=$(genvtx-config --prefix) \
  ..
make
make install

cd ..
ls -l

echo "Check the  geometry setup : " 1>&2
geomtools_inspector \
   --manager-config config/geometry/manager.conf \
   --with-visu --visu-view-3d


echo "Run the example program : " 1>&2
./ex01 > ex01_vertices.data
gnuplot ./ex01.gp

echo "Run the genvtx_production program : " 1>&2
genvtx_production \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --list

genvtx_production \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "from_every_pcb_bulk.vg" \
    --output-file "genvtx_ex01_vertices.txt" \
    --visu \
    --visu-spot-zoom 0.5

genvtx_production \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "all_coil_body_bulk.vg" \
    --output-file "genvtx_ex01_vertices_2.txt" \
    --visu \
    --visu-spot-zoom 0.5

if [ $clean -eq 1 ]; then
    rm -f ./ex01
    rm -f ./ex01_vertices.data
    rm -f ./genvtx_ex01_vertices.txt
    rm -f ./genvtx_ex01_vertices_2.txt
    rm -f ./genvtx-example-ex01.gdml
    rm -fr ${build_dir}
    find . -name "*~" -exec rm -f \{\} \;
fi

cd ${opwd}

exit 0

# end
