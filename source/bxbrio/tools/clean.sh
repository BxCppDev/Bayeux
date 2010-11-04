#!/usr/bin/env bash

find . -name "*~" -exec rm -f \{\} \;

test -f file.root && rm -f file.root
test -f test_io.root && rm -f test_io.root
test -f test_io_2.root && rm -f test_io_2.root
test -f test_data_io.root && rm -f test_data_io.root


exit 0

# end
