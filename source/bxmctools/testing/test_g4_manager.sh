#!/usr/bin/env bash

if [ ! -d /tmp/${USER}/sng4.d ]; then
    mkdir -p /tmp/${USER}/sng4.d
fi

if [ ! -x ${SNG4_BIN_DIR}/test_manager ]; then
    echo "ERROR: test_manager.sh: No binary ! Abort !" >&2
    exit 1
fi

#${SNG4_BIN_DIR}/test_manager --debug --visu ${SNG4_ROOT}/resources/test_g4vis.mac

${SNG4_BIN_DIR}/test_manager -100  

#valgrind --leak-check=yes ${SNG4_BIN_DIR}/test_manager -10 
#--num-callers

# Count the number of stored events:
# shell> cat sng4_0.xml.gz | gunzip | grep "<boost_seriali" | wc -l


exit 0

# end