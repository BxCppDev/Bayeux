#!/usr/bin/env bash
# Author: François Mauger, LPC Caen <mauger@lpccaen.in2p3.fr>
# Date:   2015-10-16
#
# Copyright (c) 2015 by Arnaud Chapon <achapon@cerap.fr>
#                       and François Mauger <mauger@lpccaen.in2p3.fr>
#
# This file is part of Bayeux/mctools.
#
# Bayeux/mctools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Bayeux/mctools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Bayeux/mctools. If not, see <http://www.gnu.org/licenses/>.
#

# Script parameters:
only_build=0
do_build=1
do_simulation=1
do_analysis=1
do_check=1
do_clean=1

# Parse command line options:
while [ -n "$1" ]; do
    token="$1"
    if [ "x${token}" == "x--no-clean" -o "x${token}" == "x-C" ]; then
	do_clean=0
    elif [ "x${token}" == "x--clean" -o "x${token}" == "x-c" ]; then
	do_clean=1
    elif [ "x${token}" == "x--build" -o "x${token}" == "x-b" ]; then
	do_build=1
    elif [ "x${token}" == "x--no-build" -o "x${token}" == "x-B" ]; then
	do_build=0
    elif [ "x${token}" == "x--check" -o "x${token}" == "x-k" ]; then
	do_check=1
    elif [ "x${token}" == "x--no-check" -o "x${token}" == "x-K" ]; then
	do_check=0
    elif [ "x${token}" == "x--simulation" -o "x${token}" == "x-s" ]; then
	do_simulation=1
    elif [ "x${token}" == "x--no-simulation" -o "x${token}" == "x-S" ]; then
	do_simulation=0
    elif [ "x${token}" == "x--analysis" -o "x${token}" == "x-a" ]; then
	do_analysis=1
    elif [ "x${token}" == "x--no-analysis" -o "x${token}" == "x-A" ]; then
	do_analysis=0
    elif [ "x${token}" == "x--only-build" -o "x${token}" == "x-o" ]; then
	only_build=1
    fi
    shift 1
done

# Save the current directory:
opwd=$(pwd)

# Custom exit:
function my_exit()
{
    cd ${opwd}
    exit $1
}

# Check Bayeux:
which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Bayeux is not setup !" 1>&2
    my_exit 1
fi

# Prepare environment:
export HPGE_BUILD_DIR=$(pwd)/_build.d
export HPGE_INSTALL_DIR=$(pwd)/_install.d
test -d ${HPGE_BUILD_DIR} && rm -fr ${HPGE_BUILD_DIR}

# Generate HTML doc
rst2html README.rst > mctools-ex03_README.html

# Build library and application:
mkdir ${HPGE_BUILD_DIR}

echo -e "\nDefine the HPGE_RESOURCES_DIR environment variables..." 1>&2
export HPGE_RESOURCES_DIR="$(pwd)/resources"

echo -e "\nCreate the HPGE_RESOURCES_DIR working directory..." 1>&2
export HPGE_WORK_DIR="/tmp/${USER}/mctools.ex03.d/HPGe"
if [ -d ${HPGE_WORK_DIR} ]; then
    rm -fr ${HPGE_WORK_DIR}
fi
mkdir -p ${HPGE_WORK_DIR}
mkdir -p ${HPGE_WORK_DIR}/data
mkdir -p ${HPGE_WORK_DIR}/management

#################### BUILD ####################
if [ ${do_build} -eq 1 ]; then
    cd ${HPGE_BUILD_DIR}

    echo -e "\nBuild the example library and application..." 1>&2
    cmake \
	-DCMAKE_INSTALL_PREFIX=${HPGE_INSTALL_DIR} \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
	..
    if [ $? -ne 0 ]; then
	echo "ERROR: cmake failed !" 1>&2
	my_exit 1
    fi
    make
    if [ $? -ne 0 ]; then
	echo "ERROR: make failed !" 1>&2
	my_exit 1
    fi
    make install
    if [ $? -ne 0 ]; then
	echo "ERROR: make install failed !" 1>&2
	my_exit 1
    fi

    if [ ${only_build} -eq 1 ]; then
	my_exit 0
    fi
    cd ${opwd}
fi

#################### CHECK ####################
if [ ${do_check} -eq 1 ]; then
echo -e "\nCheck the geometry..." 1>&2
echo "q" | bxgeomtools_inspector \
    --manager-config "${HPGE_RESOURCES_DIR}/config/HPGe/geometry/0.1/manager.conf" \
    --without-visu
if [ $? -ne 0 ]; then
    echo "ERROR: bxgeomtools_inspector failed !" 1>&2
    my_exit 1
fi

echo -e "\nList the event generators..." 1>&2
bxgenbb_inspector \
    --configuration "${HPGE_RESOURCES_DIR}/config/HPGe/simulation/primaries/0.1/manager.conf" \
    --action "list"
if [ $? -ne 0 ]; then
    echo "ERROR: bxgenbb_inspector failed !" 1>&2
    my_exit 1
fi

echo -e "\nShoot some random primary events..." 1>&2
bxgenbb_inspector \
    --configuration "${HPGE_RESOURCES_DIR}/config/HPGe/simulation/primaries/0.1/manager.conf" \
    --action "shoot" \
    --generator "Co60" \
    --prng-seed 314159 \
    --number-of-events 10000 \
    --modulo 1000 \
    --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
    --prompt \
    --title-prefix "Co60" \
    --output-file "${HPGE_WORK_DIR}/data/histos_Co60.root"
if [ $? -ne 0 ]; then
    echo "ERROR: bxgenbb_inspector failed !" 1>&2
    my_exit 1
fi

echo -e "\nList the available vertex generators..." 1>&2
bxgenvtx_production \
    --geometry-manager "${HPGE_RESOURCES_DIR}/config/HPGe/geometry/0.1/manager.conf" \
    --vertex-generator-manager "${HPGE_RESOURCES_DIR}/config/HPGe/simulation/vertexes/0.1/manager.conf" \
    --list
if [ $? -ne 0 ]; then
    echo "ERROR: bxgenvtx_production failed !" 1>&2
    my_exit 1
fi

echo -e "\nShoot some random vertexes..." 1>&2
echo -e "\n" | \bxgenvtx_production \
    --geometry-manager "${HPGE_RESOURCES_DIR}/config/HPGe/geometry/0.1/manager.conf" \
    --vertex-generator-manager "${HPGE_RESOURCES_DIR}/config/HPGe/simulation/vertexes/0.1/manager.conf" \
    --shoot --prng-seed 314159 \
    --number-of-vertices 1000 \
    --vertex-generator "sample_bulk.vg" \
    --output-file "${HPGE_WORK_DIR}/data/vertices.data" \
    --visu \
    --visu-spot-zoom 0.25
if [ $? -ne 0 ]; then
    echo "ERROR: bxgenvtx_production failed !" 1>&2
    my_exit 1
fi
fi

#################### SIMULATIONS ####################
if [ ${do_simulation} -eq 1 ]; then

    echo -e "\nRun the Geant4 simulation interactively..." 1>&2
    echo -e "/run/beamOn 5\nexit" | \
 	bxg4_production \
	--interactive \
	--g4-visu \
	--g4-macro "${HPGE_RESOURCES_DIR}/config/HPGe/simulation/geant4_control/0.1/geant4_visualization.macro" \
	--config "${HPGE_RESOURCES_DIR}/config/HPGe/simulation/geant4_control/0.1/manager.conf" \
	--vertex-generator-name "sample_bulk.vg" \
	--vertex-generator-seed 314159 \
	--event-generator-name "Co60" \
	--event-generator-seed 314159 \
	--g4-manager-seed 314159 \
	--shpf-seed 314159 \
	--number-of-events-modulo 1000 \
	--output-prng-seeds-file  "${HPGE_WORK_DIR}/management/mc_g4_production.seeds" \
	--output-prng-states-file "${HPGE_WORK_DIR}/management/mc_g4_production.states" \
	--output-data-file "${HPGE_WORK_DIR}/data/mc_g4_sample.xml"
    if [ $? -ne 0 ]; then
	echo "ERROR: bxg4_production failed !" 1>&2
	my_exit 1
    fi

    echo -e "\nRun the Geant4 simulation non-interactively..." 1>&2
    bxg4_production \
	  --config "${HPGE_RESOURCES_DIR}/config/HPGe/simulation/geant4_control/0.1/manager.conf" \
	  --vertex-generator-name "sample_bulk.vg" \
	  --vertex-generator-seed 314159 \
	  --event-generator-name "Co60" \
	  --event-generator-seed 314159 \
	  --g4-manager-seed 314159 \
	  --shpf-seed 314159 \
	  --number-of-events 100000 \
	  --number-of-events-modulo 1000 \
	  --forbid-private-hits --dont-save-no-sensitive-hit-events \
	  --output-prng-seeds-file  "${HPGE_WORK_DIR}/management/mc_g4_production.seeds" \
	  --output-prng-states-file "${HPGE_WORK_DIR}/management/mc_g4_production.states" \
	  --output-data-file "${HPGE_WORK_DIR}/data/mc_g4_sample.data.gz" \
	  > ${HPGE_WORK_DIR}/management/mc_g4_production.log

fi

#################### ANALYSIS ####################
if [ ${do_analysis} -eq 1 ]; then

    echo -e "\nSet LD_LIBRARY_PATH..." 1>&2
    export LD_LIBRARY_PATH=${HPGE_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}

    echo -e "\nAnalyze the output simulated data file..." 1>&2
    ${HPGE_INSTALL_DIR}/bin/hpge_analysis  \
    	--logging-priority "notice" \
    	--input-file "${HPGE_WORK_DIR}/data/mc_g4_sample.data.gz" \
	--input-nevents 100000 \
	--prng-seed=12345 \
	--histo-output-file "${HPGE_WORK_DIR}/data/histo_spectro.data" \
    	--histo-draw
    if [ $? -ne 0 ]; then
    	echo "ERROR: hpge_analysis failed !" 1>&2
    	my_exit 1
    fi

fi

#################### CLEAN ####################
if [ ${do_clean} -eq 1 ]; then
    rm -fr ${HPGE_WORK_DIR}
    rm -fr ${HPGE_INSTALL_DIR}
    rm -fr ${HPGE_BUILD_DIR}
    rm -f mctools-ex03_README.html
    find . -name "*~" -exec rm -f \{\} \;
fi

my_exit 0

# end of process.sh
