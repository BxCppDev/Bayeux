README.txt
----------

These  sample files and  are used  during the testing step of geomtools.
They are  provided as  is as  examples of some possible use of geomtools
classes.


--

List the names of the materials used in the geometry :

  bash$ grep "material." testing/config/test-1.0/*.geom | cut -d= -f2 | sed 's@"@@g' | sort | uniq
 air
 aluminium
 copper
 glass
 iron
 mylar
 plexiglass
 polystyrene
 vacuum

Run the test program for the geomtools geometry manager from the 
"__build-Linux-x86_64" CMake build dir :

  bash$ GEOMTOOLS_DATA_DIR=$(pwd) \
        GEOMTOOLS_TMP_DIR=$(pwd)/__build-Linux-x86_64/testing/testing_output \
        __build-Linux-x86_64/testing/test_manager --visu --gdml


Run the test program for the geomtools geometry service : 

  bash$ GEOMTOOLS_DATA_DIR=$(pwd) __build-Linux-x86_64/testing/test_service_manager --visu

--
