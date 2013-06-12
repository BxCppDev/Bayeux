#!/usr/bin/env bash

opwd=$(pwd)

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

pandoc -r rst -w html README.rst -o mctools_ex01_README.html

mkdir ${build_dir}
cd ${build_dir}

echo -e "\nBuild the example programs..." 1>&2
cmake \
  -DCMAKE_INSTALL_PREFIX=.. \
  -Dmctools_DIR=$(mctools-config --prefix) \
  ..
make
make install

#exit 0
cd ..
ls -l

echo -e "\nCheck the geometry..." 1>&2
geomtools_check_setup \
                  --manager-config config/geometry/manager.conf \
                  --without-visu --with-gdml

echo -e "\nList the event generators..." 1>&2
genbb_inspector \
    --configuration "config/event_generator/manager.conf" \
    --action "list"

echo -e "\nShoot some random events..." 1>&2
genbb_inspector \
    --configuration "config/event_generator/manager.conf" \
    --action "shoot" \
    --generator "Co60" \
    --prng-seed 314159 \
    --number-of-events 1000 \
    --output-file "histos_Co60.root"

echo -e "\nList the vertex generators..." 1>&2
genvtx_production \
    --geometry-manager "config/geometry/manager.conf" \
    --vertex-generator-manager "config/vertex_generator/manager.conf" \
    --list

echo -e "\nShoot some random vertexes..." 1>&2
genvtx_production \
    --geometry-manager "config/geometry/manager.conf" \
    --vertex-generator-manager "config/vertex_generator/manager.conf" \
    --shoot \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-generator "vessel_inner_surface.vg" \
    --output-file "mctools_ex01_vertices.txt"


echo -e "\nRun the Geant4 simulation interactively..." 1>&2
echo -e "/run/beamOn 5\nexit" | \
    g4_production \
    --logging-priority "warning" \
    --number-of-events-modulo 1 \
    --interactive \
    --g4-visu \
    --config "config/g4_manager.conf" \
    --vertex-generator-name "source_0_bulk.vg" \
    --vertex-generator-seed 0 \
    --event-generator-name "Co60" \
    --event-generator-seed 0 \
    --shpf-seed 0 \
    --g4-manager-seed 0 \
    --output-prng-seeds-file "prng_seeds.save" \
    --output-prng-states-file "prng_states.save" \
    --output-data-file "mctools_ex01_Co60_source_0_bulk.xml" \
    --g4-macro "config/g4vis.mac"

echo -e "\nBrowse the output plain simulated data file..." 1>&2
./ex01_read_plain_simdata \
    --interactive  \
    --with-visualization \
    --logging-priority "notice" \
    --input-file "mctools_ex01_Co60_source_0_bulk.xml"

echo -e "\nRun the Geant4 simulation non-interactively..." 1>&2
g4_production \
                --logging-priority "warning" \
		--number-of-events 10 \
            	--number-of-events-modulo 0 \
            	--batch \
            	--config "config/g4_manager.conf" \
		--vertex-generator-name "source_0_bulk.vg" \
		--vertex-generator-seed 0 \
		--event-generator-name "Co60" \
		--event-generator-seed 0 \
		--shpf-seed 0 \
		--g4-manager-seed 0 \
                --output-prng-seeds-file "prng_seeds.save" \
                --output-prng-states-file "prng_states.save" \
                --output-data-file "mctools_ex01_Co60_source_0_bulk.data.gz"

echo -e "\nBrowse the output plain simulated data file..." 1>&2
./ex01_read_plain_simdata \
    --interactive \
    --with-visualization \
    --logging-priority "notice" \
    --input-file "mctools_ex01_Co60_source_0_bulk.data.gz"

echo -e "\nRun the Geant4 simulation through a non-interactive data processing pipeline..." 1>&2

dpp_processing \
    --logging-priority "debug" \
    --dlls-config "config/pipeline/dlls.conf" \
    --module-manager-config "config/pipeline/module_manager.conf" \
    --max-records 7 \
    --modulo 5 \
    --module "Co60@source_0_bulk" \
    --output-file "mctools_ex01_Co60_source_0_bulk.dpp.xml"

rm -f ex01_read_plain_simdata
rm -f histos_Co60.root
rm -f mctools_ex01-1.0.gdml
rm -f geomtools_check_setup.C
rm -f mctools_ex01_Co60_source_0_bulk.xml
rm -f mctools_ex01_vertices.txt
rm -f prng_seeds.save
rm -f prng_seeds.save.~backup~
rm -f prng_states.save
rm -f prng_states.save.~backup~
rm -f mctools_ex01_Co60_source_0_bulk.data.gz

rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;


cd ${opwd}

exit 0

# end