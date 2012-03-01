#!/bin/bash

test=1

if [ $test -ne 0 ]; then
    echo -e "314159\n1000000" | ${GENBB_HELP_ROOT}/$(uname -s)-$(uname -m | sed 's/ //g')/bin/test_rnd.f77.exe | tr -d ' ' | gsl-histogram 0 1 100 > test_rnd.his
fi
gnuplot ${GENBB_HELP_ROOT}/tests/test_rnd.gpl

if [ $test -ne 0 ]; then
    rm -f test_rnd.his
fi
