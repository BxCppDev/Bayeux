#!/bin/bash

find . -name "*~" -exec rm -f \{\} \;
rm -f tu*.data
rm -f test_.data

# end
