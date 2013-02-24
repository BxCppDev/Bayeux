README.txt
----------

These  sample files and  are used  during the  testing step  of materials.
They are  provided as  is as  examples of some  possible use  of materials
classes.

---------------------------------------------------------------------------

From  the build  directory, run  the  test program  for the  materials
geometry manager with an embeded materials driver plugin :

bash$ MATERIALS_DATA_DIR=$(pwd) __build-Linux-x86_64/testing/test_materials_geom_plugin --visu --gdml

Note : 
  This test programs uses the sample geometry setup stored
  in the '${GEOMTOOLS_DATA_DIR}/testing/config/test-1.0/' directory

  Extract material names from geometry setup files : 
  
  bash$ grep "material.*.ref" ${GEOMTOOLS_DATA_DIR}/testing/config/test-1.0/*.geom | cut -d= -f2

  List of materials:
    "mylar"
    "polystyrene"
    "plexiglass"
    "copper"
    "glass"
    "aluminium"
    "iron"
    "air"
    "vacuum"

--
