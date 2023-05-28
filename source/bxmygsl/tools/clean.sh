#!/bin/bash

find . -name '*~' -exec rm -f \{\} \;

find . -name "test_multimin_f_init_*.data"  -exec rm -f \{\} \;
find . -name "test_multimin_f_sol_*.data"  -exec rm -f \{\} \;

rm -f ./test_multimin.his ./test_multimin.ini ./test_multimin.lop ./test_multimin.pars ./test_multimin.ps ./test_multimin.sol 

rm -f ./tnd.data

exit 0

# end
