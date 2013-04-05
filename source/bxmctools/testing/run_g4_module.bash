#!/usr/bin/env bash

tmp_test_dir=${SNG4_ROOT}/tests/tmp
if [ ! -d ${tmp_test_dir} ]; then
    mkdir ${tmp_test_dir}
fi
cat<<EOF

This test  script runs the  sng4 GEANT4-based simulation  program from
the  generic  snemo_processing program  (released  within sncore).   A
'Simulation Module' is plug-ged  along the the event record processing
chain.  This module  is responsible to feed the  'Simulated Data' bank
of the event record with simulated data objects.


EOF

${SNCORE_BIN_DIR}/snemo_processing \
    -m "bb0nu_Se82@source_bulk" \
    --max-events 10 \
    -L ${SNG4_ROOT}/tests/conf/dlls.conf  \
    -c ${SNG4_ROOT}/tests/conf/module_manager.conf \
    -o ${tmp_test_dir}/sng4_SD.txt.gz
if [ $? -ne 0 ]; then
    echo "ERROR: snemo_processing failed !" 1>&2
    exit 1
fi

exit 0

# end
