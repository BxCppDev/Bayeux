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

--
