#!/bin/bash

system=$(mygsl-config --system)
prefix=$(mygsl-config --prefix)

bin_path=${prefix}/${system}/bin


function test_histo ()
{
    ${bin_path}/test_histogram
    gnuplot ${prefix}/tests/test_histogram.gnuplot


    rm -f test_histogram.store
    rm -f test_histogram.data
    find . -maxdepth 1 -name "test_histogram[[:digit:]].data" \
	-exec rm -f \{\} \;
}

function test_ode ()
{
    ${prefix}/tests/test_ode.bash
}

function test_multimin ()
{
    ${prefix}/tests/test_multimin.bash --run
    ${prefix}/tests/test_multimin.bash --a
    ${prefix}/tests/test_multimin.bash --b
    ${prefix}/tests/test_multimin.bash --r
    ${prefix}/tests/test_multimin.bash --p1
    ${prefix}/tests/test_multimin.bash --p2
    ${prefix}/tests/test_multimin.bash --p3
    ${prefix}/tests/test_multimin.bash --p4
    #${prefix}/tests/test_multimin.bash --f
    #${prefix}/tests/test_multimin.bash --af
    #${prefix}/tests/test_multimin.bash --clean
    
}

test_histo
test_ode
test_multimin

rm -f *~
rm -f __test_ode.data
rm -f __test_ode.gnuplot

# end