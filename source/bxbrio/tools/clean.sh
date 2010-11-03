#!/usr/bin/env bash

find . -name "*~" -exec rm -f \{\} \;

test -f test_io.root && rm -f test_io.root


exit 0

# end
