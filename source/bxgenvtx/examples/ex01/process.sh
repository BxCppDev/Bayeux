#!/usr/bin/env bash

which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Missing Bayeux's bxquery !" >&2
    exit 1
fi

opwd=$(pwd)

clean=1
help=0
debug=0
html=0
html_file=genvtx-ex01_README.html

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

install_dir=$(pwd)/_install.d
test -d ${install_dir} && rm -fr ${install_dir}

build_dir=$(pwd)/_build.d
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

if [ $html -eq 1 ]; then
    pandoc -r rst -w html README.rst -o ${html_file}
fi

mkdir ${build_dir}
cd ${build_dir}

echo "NOTICE: Using Bayeux..." >&2
cmake \
    -DCMAKE_INSTALL_PREFIX=${install_dir} \
    -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
    ..
if [ $? -ne 0 ]; then
    echo "ERROR: Configuration failed !" 1>&2
    my_exit 1
fi
genvtx_production_bin="bxgenvtx_production"
geomtools_inspector_bin="bxgeomtools_inspector"
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

cd ..
ls -l

which ${genvtx_production_bin} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: No '${genvtx_inspector_bin}' executable !" 1>&2
    my_exit 1
fi

which ${geomtools_inspector_bin} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: No '${geomtools_inspector_bin}' executable !" 1>&2
    my_exit 1
fi

echo "Check the  geometry setup : " 1>&2
${geomtools_inspector_bin} \
   --manager-config config/geometry/manager.conf \
   --with-visu --visu-view-3d
if [ $? -ne 0 ]; then
    echo "ERROR: Geometry check failed !" 1>&2
    my_exit 1
fi

echo "Run the example program ex01 : " 1>&2
${install_dir}/ex01 > ex01_vertices.data
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex01 failed !" 1>&2
    my_exit 1
fi
gnuplot ./ex01.gp
if [ $? -ne 0 ]; then
    echo "ERROR: Gnuplot display failed !" 1>&2
    my_exit 1
fi

echo "Run the genvtx_production program : " 1>&2

echo "NOTICE: Print usage : " 1>&2
${genvtx_production_bin} --help
if [ $? -ne 0 ]; then
    echo "ERROR: Print help failed !" 1>&2
    my_exit 1
fi

echo "NOTICE: List vertex generators : " 1>&2
${genvtx_production_bin} \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --list
if [ $? -ne 0 ]; then
    echo "ERROR: List vertex generators failed !" 1>&2
    my_exit 1
fi

echo "NOTICE: Shoot vertexes (all PCBs bulk): " 1>&2
${genvtx_production_bin} \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "from_every_pcb_bulk.vg" \
    --output-file "genvtx_ex01_vertices.txt" \
    --visu \
    --visu-spot-zoom 0.5
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot vertexes failed !" 1>&2
    my_exit 1
fi

echo "NOTICE: Shoot vertexes (all coils surf) : " 1>&2
${genvtx_production_bin} \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "all_coil_body_surf.vg" \
    --output-file "genvtx_ex01_vertices_1.txt" \
    --visu \
    --visu-spot-zoom 0.5
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot vertexes failed !" 1>&2
    my_exit 1
fi

echo "NOTICE: Shoot vertexes (all coils bulk): " 1>&2
${genvtx_production_bin} \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "all_coil_body_bulk.vg" \
    --output-file "genvtx_ex01_vertices_2.txt" \
    --visu \
    --visu-spot-zoom 0.5
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot vertexes failed !" 1>&2
    my_exit 1
fi

echo "NOTICE: Shoot vertexes (dummy shape bulk) : " 1>&2
${genvtx_production_bin} \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "dummy_bulk.vg" \
    --output-file "genvtx_ex01_vertices_3.txt" \
    --visu \
    --visu-spot-zoom 0.1
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot vertexes failed !" 1>&2
    my_exit 1
fi

echo "NOTICE: Shoot vertexes (dummy shape inner surface) : " 1>&2
${genvtx_production_bin} \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "dummy_inner_surface.vg" \
    --output-file "genvtx_ex01_vertices_4.txt" \
    --visu \
    --visu-spot-zoom 0.1
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot vertexes failed !" 1>&2
    my_exit 1
fi

echo "NOTICE: Shoot vertexes (dummy shape other surfaces) : " 1>&2
${genvtx_production_bin} \
    --geometry-manager config/geometry/manager.conf \
    --vertex-generator-manager config/vertex/manager.conf \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "dummy_some_surfaces.vg" \
    --output-file "genvtx_ex01_vertices_4.txt" \
    --visu \
    --visu-spot-zoom 0.1
if [ $? -ne 0 ]; then
    echo "ERROR: Shoot vertexes failed !" 1>&2
    my_exit 1
fi

if [ $clean -eq 1 ]; then
    echo "NOTICE: Clean..." 1>&2
    rm -f ./ex01_vertices.data
    rm -f ./genvtx_ex01_vertices.txt
    rm -f ./genvtx_ex01_vertices_1.txt
    rm -f ./genvtx_ex01_vertices_2.txt
    rm -f ./genvtx_ex01_vertices_3.txt
    rm -f ./genvtx_ex01_vertices_4.txt
    rm -f ./genvtx-example-ex01.gdml
    rm -fr ${build_dir}
    rm -fr ${install_dir}
    find . -name "*~" -exec rm -f \{\} \;
fi

my_exit 0

# end
