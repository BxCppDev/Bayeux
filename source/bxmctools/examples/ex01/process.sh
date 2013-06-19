#!/usr/bin/env bash

do_clean=1
do_simulation=1
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
    fi
    shift 1
done

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

pandoc -r rst -w html README.rst -o mctools-ex01_README.html

mkdir ${build_dir}
cd ${build_dir}

echo -e "\nBuild the example programs..." 1>&2
cmake \
    -DCMAKE_INSTALL_PREFIX=.. \
    -Dmctools_DIR=$(mctools-config --prefix) \
    ..
make
make install

if [ ${only_build} -eq 1 ]; then
    exit 0
fi
cd ..
ls -l

echo -e "\nCheck the geometry..." 1>&2
echo "q" | geomtools_inspector \
    --manager-config config/geometry/manager.conf \
    --without-visu

echo -e "\nList the event generators..." 1>&2
genbb_inspector \
    --configuration "config/event_generator/manager.conf" \
    --action "list"

eg_name="Co60"

echo -e "\nShoot some random events..." 1>&2
genbb_inspector \
    --configuration "config/event_generator/manager.conf" \
    --action "shoot" \
    --generator "${eg_name}" \
    --prng-seed 314159 \
    --number-of-events 1000 \
    --output-file "histos_${eg_name}.root"

#root "histos_${eg_name}.root"

echo -e "\nList the vertex generators..." 1>&2
genvtx_production \
    --geometry-manager "config/geometry/manager.conf" \
    --vertex-generator-manager "config/vertex_generator/manager.conf" \
    --list

vg_name="vessel_inner_surface.vg"

echo -e "\nShoot some random vertexes..." 1>&2
genvtx_production \
    --geometry-manager "config/geometry/manager.conf" \
    --vertex-generator-manager "config/vertex_generator/manager.conf" \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator ${vg_name} \
    --output-file "mctools_ex01_vertices_${vg_name}.txt"

vg_name="probe_bulk.vg"

echo -e "\nShoot some random vertexes..." 1>&2
genvtx_production \
    --geometry-manager "config/geometry/manager.conf" \
    --vertex-generator-manager "config/vertex_generator/manager.conf" \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator ${vg_name} \
    --output-file "mctools_ex01_vertices_${vg_name}.txt"

#################### SIMULATIONS ####################
if [ $do_simulation -eq 1 ]; then

    eg_name="Co60"
    vg_name="source_0_bulk.vg"
#vg_name="col1_row0_scin_surface.vg"

    echo -e "\nRun the Geant4 simulation interactively..." 1>&2
    echo -e "/run/beamOn 5\nexit" | \
	g4_production \
	--logging-priority "warning" \
	--number-of-events-modulo 1 \
	--interactive \
	--g4-visu \
	--config "config/g4_manager.conf" \
	--vertex-generator-name ${vg_name} \
	--vertex-generator-seed 0 \
	--event-generator-name ${eg_name} \
	--event-generator-seed 0 \
	--shpf-seed 0 \
	--g4-manager-seed 0 \
	--output-prng-seeds-file "prng_seeds.save" \
	--output-prng-states-file "prng_states.save" \
	--output-data-file "mctools_ex01_${eg_name}_${vg_name}.xml" \
	--g4-macro "config/g4vis.mac"


    echo -e "\nSet LD_LIBRARY_PATH..." 1>&2
    export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}

    echo -e "\nBrowse the output plain simulated data file..." 1>&2
    ./ex01_read_plain_simdata \
	--interactive  \
	--with-visualization \
	--logging-priority "notice" \
	--input-file "mctools_ex01_${eg_name}_${vg_name}.xml"

    echo -e "\nRun the Geant4 simulation non-interactively..." 1>&2
    g4_production \
        --logging-priority "warning" \
	--number-of-events 100 \
        --number-of-events-modulo 0 \
        --batch \
        --config "config/g4_manager.conf" \
	--vertex-generator-name ${vg_name} \
	--vertex-generator-seed 0 \
	--event-generator-name ${eg_name} \
	--event-generator-seed 0 \
	--shpf-seed 0 \
	--g4-manager-seed 0 \
        --output-prng-seeds-file "prng_seeds.save" \
        --output-prng-states-file "prng_states.save" \
        --output-data-file "mctools_ex01_${eg_name}_${vg_name}.data.gz"

    echo -e "\nBrowse the output plain simulated data file..." 1>&2
    ./ex01_read_plain_simdata \
	--interactive \
	--with-visualization \
	--logging-priority "notice" \
	--input-file "mctools_ex01_${eg_name}_${vg_name}.data.gz"

    echo -e "\nRun the Geant4 simulation through a non-interactive data processing pipeline..." 1>&2

    sim_module="Co60@source_0_bulk"
    dpp_processing \
	--logging-priority "debug" \
	--dlls-config "config/pipeline/dlls.conf" \
	--module-manager-config "config/pipeline/module_manager.conf" \
	--max-records 7 \
	--modulo 5 \
	--module ${sim_module} \
	--output-file "mctools_ex01_${sim_module}.dpp.xml"

fi

if [ ${do_clean} -eq 1 ]; then
    rm -f ex01_read_plain_simdata
    rm -f histos_Co60.root
    rm -f mctools_ex01-1.0.gdml
    rm -f geomtools_check_setup.C
    rm -f mctools_ex01_Co60_source_0_bulk.xml
    rm -f mctools_ex01_vertices_*.txt
    rm -f prng_seeds.save
    rm -f prng_seeds.save.~backup~
    rm -f prng_states.save
    rm -f prng_states.save.~backup~
    rm -f mctools_ex01_Co60_source_0_bulk.data.gz
    rm -fr ${build_dir}
    rm -fr ./lib
    find . -name "*~" -exec rm -f \{\} \;
fi

cd ${opwd}

exit 0

# end