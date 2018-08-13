#!/usr/bin/env bash

do_clean=1
do_simulation=1
do_pre=1
do_cone=0
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
    elif [ "x${token}" == "x--cone" ]; then
	do_cone=1
    elif [ "x${token}" == "x--no-cone" ]; then
	do_cone=0
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
    pandoc -r rst -w html README.rst -o mctools-ex00_README.html
    # which xdg-open > /dev/null 2<&1
    # if [ $? -eq 0 ]; then
    # 	echo -e "\nBrowse the HTML README document..." 1>&2
    # 	xdg-open mctools-ex00_README.html &
    # fi
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

eg_name="electron_1MeV"
vg_name="source_bulk.vg"

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
	--output-file "histos_${eg_name}.root"
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
	--output-file "mctools_ex00_vertices_${vg_name}.txt"
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

    eg_name="electron_1MeV"
    if [ $do_cone -eq 1 ]; then
	 eg_name="electron_1MeV_cone"
    fi
    vg_name="source_bulk.vg"

    echo -e "\nRun the Geant4 simulation interactively..." 1>&2
    echo -e "/run/beamOn 20\nexit" | \
	bxg4_production \
	--logging-priority "warning" \
	--number-of-events-modulo 1 \
	--interactive \
	--g4-visu \
	--config "${CONFIG_DIR}/simulation/manager.conf" \
	--vertex-generator-name ${vg_name} \
	--vertex-generator-seed 0 \
	--event-generator-name ${eg_name} \
	--event-generator-seed 0 \
	--shpf-seed 0 \
	--output-profiles " calo + all_details " \
	--g4-manager-seed 0 \
	--output-prng-seeds-file "prng_seeds.save" \
	--output-prng-states-file "prng_states.save" \
	--output-data-file "mctools_ex00_${eg_name}_${vg_name}.xml" \
	--g4-macro "${CONFIG_DIR}/simulation/geant4_visualization.mac"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxg4_production failed !" 1>&2
	exit 1
    fi
    echo "----------------------------------" 1>&2
    echo "NOTICE: Seeds: " 1>&2
    cat prng_seeds.save 1>&2
    echo "----------------------------------" 1>&2
    # exit 0

    echo -e "\nSet LD_LIBRARY_PATH..." 1>&2
    export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}

    echo -e "\nBrowse the output plain simulated data file..." 1>&2
    ${install_dir}/ex00_read_plain_simdata \
	--interactive  \
	${visu_opt} \
	--logging-priority "notice" \
	--input-file "mctools_ex00_${eg_name}_${vg_name}.xml"
    if [ $? -ne 0 ]; then
	echo "ERROR: ex00_read_plain_simdata failed !" 1>&2
	exit 1
    fi
    # exit 0

    echo -e "\nRun the Geant4 simulation non-interactively..." 1>&2
    bxg4_production \
        --logging-priority "warning" \
	--number-of-events 35 \
        --number-of-events-modulo 10 \
        --batch \
        --config "${CONFIG_DIR}/simulation/manager.conf" \
	--vertex-generator-name ${vg_name} \
	--vertex-generator-seed 0 \
	--event-generator-name ${eg_name} \
	--event-generator-seed 0 \
	--shpf-seed 0 \
	--g4-manager-seed 0 \
	--output-profiles " calo + all_details " \
        --output-prng-seeds-file "prng_seeds.save" \
        --output-prng-states-file "prng_states.save" \
        --output-data-file "mctools_ex00_${eg_name}_${vg_name}.data.gz"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxg4_production failed !" 1>&2
	exit 1
    fi
    echo "----------------------------------" 1>&2
    echo "NOTICE: Seeds: " 1>&2
    cat prng_seeds.save 1>&2
    echo "----------------------------------" 1>&2
    echo -e "\nBrowse the output plain simulated data file..." 1>&2
    ${install_dir}/ex00_read_plain_simdata \
	--interactive \
	${visu_opt} \
	--logging-priority "notice" \
	--input-file "mctools_ex00_${eg_name}_${vg_name}.data.gz"
    if [ $? -ne 0 ]; then
	echo "ERROR: ex00_read_plain_simdata failed !" 1>&2
	exit 1
    fi

    echo -e "\nRun the Geant4 simulation through a non-interactive data processing pipeline..." 1>&2

    sim_module="electron_1MeV_cone@source_bulk"
    bxdpp_processing \
	--logging "warning" \
	--dlls-config "${CONFIG_DIR}/pipeline/dlls.conf" \
	--module-manager-config "${CONFIG_DIR}/pipeline/module_manager.conf" \
	--max-records 15 \
	--modulo 5 \
	--module ${sim_module} \
	--output-file "mctools_ex00_${sim_module}.dpp.brio"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxdpp_processing failed !" 1>&2
	exit 1
    fi

    echo -e "\nBrowse the output pipeline simulated data file..." 1>&2
    ${install_dir}/ex00_read_pipeline_simdata \
        --logging-priority "notice" \
        --interactive \
	${visu_opt} \
        --input-file "mctools_ex00_${sim_module}.dpp.brio"

    echo -e "\nRun the standalone simulation module..." 1>&2
    ${install_dir}/ex00_run_sim_module \
        --number-of-events=25 \
	--geometry-config "${CONFIG_DIR}/geometry/manager.conf" \
	--simulation-config "${CONFIG_DIR}/simulation/manager.conf" \
	--output-file "mctools_ex00_ssm.brio"
    if [ $? -ne 0 ]; then
	echo "ERROR: ./ex00_run_sim_module failed !" 1>&2
	exit 1
    fi

    echo -e "\nBrowse the output simulated data file..." 1>&2
    ${install_dir}/ex00_read_pipeline_simdata \
        --logging-priority "notice" \
        --interactive \
	${visu_opt} \
        --input-file "mctools_ex00_ssm.brio"

fi

if [ ${do_clean} -eq 1 ]; then
    rm -f bias_stats.data
    rm -f geomtools_inspector.C
    rm -f histos_electron_1MeV_gaussian_100keV.root
    rm -f mctools_ex00-1.0.gdml
    rm -f mctools_ex00_electron_1MeV_cone@source_bulk.dpp.brio
    rm -f mctools_ex00_electron_1MeV_source_bulk.vg.data.gz
    rm -f mctools_ex00_electron_1MeV_source_bulk.vg.xml
    rm -f mctools_ex00_electron_1MeV_cone_source_bulk.vg.data.gz
    rm -f mctools_ex00_electron_1MeV_cone_source_bulk.vg.xml
    rm -f mctools_ex00_electron_1MeV_source_bulk.xml
    echo -e "\nDelete the HTML README document..." 1>&2
    rm -f mctools-ex00_README.html
    rm -f mctools_ex00_vertices2.txt
    rm -f mctools_ex00_vertices_source_bulk.vg.txt
    rm -f mctools_ex00_vertices.txt
    rm -f mctools_ex00_ssm.brio
    rm -f histos_electron_1MeV.root
    rm -f prng_seeds.save
    rm -f prng_states.save
    rm -fr ${install_dir}
    rm -fr ${build_dir}
    find . -name "*~" -exec rm -f \{\} \;
fi

cd ${opwd}

exit 0

# end
