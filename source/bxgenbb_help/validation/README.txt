Install and setup the genbb_help library.

cd in the validation source tree.

Build the validation executable :

./proc.bash

Run the validation protocol for the "Co60" generator :

Generate particles and associated distributions for Fortran and C++ :
a) ./run.bash --only Co60 --nevents 10000 --prng-seed 314159 --prng-trunc 6

Kolmogorov test between Fortran and C++ output histograms :
b) ./validate Co60 /transcend/work/tmp | grep ^\#


Co60 0 OK
K40  0 OK
Bi214_Po214 0 OK RO and SF
Bi212_Po212 0 OK RO and SF
Cs137 0 OK

./run.bash --only Bi214_Po214 --nevents 10000 --prng-seed 314159 --prng-trunc 6
./validate Bi214_Po214 /transcend/work/tmp | grep ^\#

Tl208 1  only stat
Cs137 0 OK
Bi212_Po212 bad...
