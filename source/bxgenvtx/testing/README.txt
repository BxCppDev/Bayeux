README.txt
----------

These  sample files and  are used  during the  testing step  of genvtx.
They are  provided as  is as  examples of some  possible use  of genvtx
classes.

----------------------------------------------------------------------

Run test scripts from the '__build-Linux-x86_64' build directory :

bash$ GENVTX_BIN_DIR=__build-Linux-x86_64/testing/ \
        gnuplot testing/test_box_vg.gp 

bash$ GENVTX_BIN_DIR=__build-Linux-x86_64/testing/ \
        gnuplot testing/test_cylinder_vg.gp 

bash$ GENVTX_BIN_DIR=__build-Linux-x86_64/testing/ \
        gnuplot testing/test_tube_vg.gp 

bash$ GENVTX_BIN_DIR=__build-Linux-x86_64/testing/ \
        gnuplot testing/test_placement_vg.gp 

bash$ GENVTX_DATA_DIR=$(pwd) GENVTX_BIN_DIR=__build-Linux-x86_64/testing/ \
        gnuplot testing/test_from_file_vg.gp 

bash$ __build-Linux-x86_64/testing/test_quadrangle_random_tools --draw

bash$ __build-Linux-x86_64/testing/test_triangle_random_tools --draw


-----------------------------------------------------------------

Check the setup of the test_manager program:

  bash$ GENVTX_DATA_DIR=$(pwd) \
    geomtools_check_setup \
    --manager-config ${GENVTX_DATA_DIR}/testing/config/test-1.0/geometry/manager.conf \
    --with-visu --visu-view-3d  \
    --with-mapping \
    --with-gdml 

List of available vertex generators: 

  'all_capacitors_side.vg'
  'all_mezzanine_pcb_bulk.vg'
  'board_pcb_bulk.vg'
  'hall_roof.vg' 
  'some_mezzanines_pcb_bulk.vg' 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --vertex-generator "some_mezzanine_pcb_bulk.vg" \
      --many

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --vertex-generator "hall_roof.vg" \
      --many --many --many --visu 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --vertex-generator "all_capacitors_side.vg" \
      --many --many --many \
      --dump-gids \
      --visu --tiny --tiny 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --vertex-generator "all_capacitors_side.vg" \
      --many --many --many \
      --dump-gids \
      --visu --visu-object "[1000:6.0]" --tiny --tiny 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --vertex-generator "some_mezzanines_pcb_bulk.vg" \
      --many --many --many \
      --dump-gids \
      --visu --tiny 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --test-2 \
      --vertex-generator "all_source_bulk.vg" \
      --many --many \
      --dump-gids \
      --visu --tiny 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --test-2 \
      --vertex-generator "source_0_bulk.vg" \
      --many --many \
      --dump-gids \
      --visu --tiny 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --test-2 \
      --vertex-generator "all_scin_surface.vg" \
      --many --many --many --many \
      --dump-gids \
      --visu --tiny 

  bash$ GENVTX_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
      --test-2 \
      --vertex-generator "special_scin_surface.vg" \
      --many --many --many --many \
      --dump-gids \
      --visu --tiny 


--
