#!/bin/bash

find . -name "*~" -exec rm -f \{\} \;
rm -f tu*.data
rm -f test_.data
rm -f test_model_factory.gdml
rm -f test_tessellated_solid.data

# end
