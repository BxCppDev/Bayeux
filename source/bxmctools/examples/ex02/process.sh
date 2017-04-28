#!/usr/bin/env bash

do_clean=1
do_simulation=1
do_pre=1
do_visu=1
only_build=0
while [ -n "$1" ]; do
    token="$1"
    if [ "x${token}" == "x--no-clean" -o "x${token}" == "x-C" ]; then
	do_clean=0
    elif [ "x${token}" == "x--clean" -o "x${token}" == "x-c" ]; then
	do_clean=1
    elif [ "x${token}" == "x--simulation" -o "x${token}" == "x-s" ]; then
	do_simulation=1
    elif [ "x${token}" == "x--no-simulation" -o "x${token}" == "x-S" ]; then
	do_simulation=0
    elif [ "x${token}" == "x--only-build" -o "x${token}" == "x-o" ]; then
	only_build=1
    elif [ "x${token}" == "x--pre" -o "x${token}" == "x-p" ]; then
	do_pre=1
    elif [ "x${token}" == "x--no-pre" -o "x${token}" == "x-P" ]; then
	do_pre=0
    elif [ "x${token}" == "x--visu" ]; then
	do_visu=1
    elif [ "x${token}" == "x--no-visu" ]; then
	do_visu=0
    fi
    shift 1
done

opwd=$(pwd)

install_dir=$(pwd)/_install.d
test -d ${install_dir} && rm -fr ${install_dir}

build_dir=$(pwd)/_build.d
test -d ${build_dir} && rm -fr ${build_dir}

which pandoc > /dev/null 2<&1
if [ $? -eq 0 ]; then
    echo -e "\nBuild the HTML README document..." 1>&2
    pandoc -r rst -w html README.rst -o mctools-ex02_README.html
fi
mkdir ${build_dir}
cd ${build_dir}

echo -e "\nBuild the example programs..." 1>&2
cmake \
    -DCMAKE_INSTALL_PREFIX=${install_dir} \
    -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
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
make install
if [ $? -ne 0 ]; then
    echo "ERROR: make install failed !" 1>&2
    exit 1
fi

if [ ${only_build} -eq 1 ]; then
    exit 0
fi
cd ..
ls -l

echo -e "\nDefine the CONFIG_DIR environment variable..." 1>&2
export CONFIG_DIR="./config"

eg_name="C12_ion_2plus_monokinetic_1keV"
vg_name="gun_mouth_point.vg"

if [ ${do_pre} -eq 1 ]; then
    echo -e "\nCheck the geometry..." 1>&2
    echo "q" | bxgeomtools_inspector \
	--manager-config ${CONFIG_DIR}/geometry/manager.conf \
	--without-visu
    if [ $? -ne 0 ]; then
	echo "ERROR: geomtools_inspector failed !" 1>&2
	exit 1
    fi

    echo -e "\nList the event generators..." 1>&2
    bxgenbb_inspector \
	--configuration "${CONFIG_DIR}/event_generator/manager.conf" \
	--action "list"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxgenbb_inspector failed !" 1>&2
	exit 1
    fi

    echo -e "\nShoot some random events..." 1>&2
    bxgenbb_inspector \
	--configuration "${CONFIG_DIR}/event_generator/manager.conf" \
	--action "shoot" \
	--generator "${eg_name}" \
	--prng-seed 314159 \
	--number-of-events 1000 \
	--output-file "histos.root"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxgenbb_inspector failed !" 1>&2
	exit 1
    fi

    echo -e "\nList the vertex generators..." 1>&2
    bxgenvtx_production \
	--geometry-manager "${CONFIG_DIR}/geometry/manager.conf" \
	--vertex-generator-manager "${CONFIG_DIR}/vertex_generator/manager.conf" \
	--list
    if [ $? -ne 0 ]; then
	echo "ERROR: bxgenvtx_production failed !" 1>&2
	exit 1
    fi

    echo -e "\nShoot some random vertexes..." 1>&2
    bxgenvtx_production \
	--geometry-manager "${CONFIG_DIR}/geometry/manager.conf" \
	--vertex-generator-manager "${CONFIG_DIR}/vertex_generator/manager.conf" \
	--shoot \
	--number-of-vertices 10000 \
	--prng-seed 314159 \
	--vertex-generator ${vg_name} \
	--output-file "mctools_ex02_vertices.txt"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxgenvtx_production failed !" 1>&2
	exit 1
    fi

fi

visu_opt=""
if [ ${do_visu} -eq 1 ]; then
    visu_opt="--with-visualization"
fi

#################### SIMULATIONS ####################
if [ $do_simulation -eq 1 ]; then

    eg_name="C12_ion_2plus_monokinetic_1keV"
    vg_name="gun_mouth_point.vg"
    # eg_name="C12_ion_2plus_gaussian_1500eV_cone"

    echo -e "\nRun the Geant4 simulation interactively..." 1>&2
    echo -e "/run/beamOn 1\nexit" | \
	bxg4_production \
	--logging-priority "debug" \
	--number-of-events-modulo 1 \
	--interactive \
	--g4-visu \
	--config "${CONFIG_DIR}/simulation/manager.conf" \
	--vertex-generator-name ${vg_name} \
	--vertex-generator-seed 0 \
	--event-generator-name ${eg_name} \
	--event-generator-seed 0 \
	--shpf-seed 0 \
	--output-profiles " detector + all_details " \
	--g4-manager-seed 0 \
	--output-prng-seeds-file "prng_seeds.save" \
	--output-prng-states-file "prng_states.save" \
	--output-data-file "mctools_ex02_output.xml" \
	--g4-macro "${CONFIG_DIR}/simulation/geant4_visualization.mac"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxg4_production failed !" 1>&2
	exit 1
    fi
    #exit 0

    echo -e "\nSet LD_LIBRARY_PATH..." 1>&2
    export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}

    echo -e "\nBrowse the output plain simulated data file..." 1>&2
    ${install_dir}/ex02_read_plain_simdata \
	--interactive  \
	${visu_opt} \
	--logging-priority "notice" \
	--input-file "mctools_ex02_output.xml"
    if [ $? -ne 0 ]; then
	echo "ERROR: ex02_read_plain_simdata failed !" 1>&2
	exit 1
    fi
    # #exit 0

    # eg_name="C12_ion_2plus_gaussian_1500eV_cone"

    echo -e "\nRun the Geant4 simulation non-interactively..." 1>&2
    bxg4_production \
        --logging-priority "warning" \
	--number-of-events 20 \
        --number-of-events-modulo 10 \
        --batch \
        --config "${CONFIG_DIR}/simulation/manager.conf" \
	--vertex-generator-name ${vg_name} \
	--vertex-generator-seed 0 \
	--event-generator-name ${eg_name} \
	--event-generator-seed 0 \
	--shpf-seed 0 \
	--g4-manager-seed 0 \
	--output-profiles " detector + all_details " \
        --output-prng-seeds-file "prng_seeds.save" \
        --output-prng-states-file "prng_states.save" \
        --output-data-file "mctools_ex02_output.data.gz"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxg4_production failed !" 1>&2
	exit 1
    fi
    echo -e "\nBrowse the output plain simulated data file..." 1>&2
    ${install_dir}/ex02_read_plain_simdata \
	--interactive \
	${visu_opt} \
	--logging-priority "notice" \
	--input-file "mctools_ex02_output.data.gz"
    if [ $? -ne 0 ]; then
	echo "ERROR: ex02_read_plain_simdata failed !" 1>&2
	exit 1
    fi

fi

if [ ${do_clean} -eq 1 ]; then
    rm -f geomtools_inspector.C
    rm -f mctools_ex02-1.0.gdml
    rm -f mctools_ex02_output.data.gz
    rm -f mctools_ex02_output.xml
    echo -e "\nDelete the HTML README document..." 1>&2
    rm -f mctools-ex02_README.html
    rm -f mctools_ex02_vertices.txt
    rm -f histos.root
    rm -f prng_seeds.save
    rm -f prng_states.save
    rm -fr ${build_dir}
    rm -fr ${install_dir}
    find . -name "*~" -exec rm -f \{\} \;
fi

cd ${opwd}

exit 0

# end
