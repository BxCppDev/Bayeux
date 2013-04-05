README.txt
----------

These  sample files and  are used  during the  testing step  of mctools.
They are  provided as  is as  examples of some  possible use  of mctools
classes.

----------------------------------------------------------------------

Run test scripts from the '__build-Linux-x86_64' build directory :

bash$ MCTOOLS_DATA_DIR=$(pwd) \
        __build-Linux-x86_64/testing/test_base_step_hit 

bash$ MCTOOLS_DATA_DIR=$(pwd) \
        __build-Linux-x86_64/testing/test_simulated_data_1

bash$ MCTOOLS_DATA_DIR=$(pwd) \
        __build-Linux-x86_64/testing/test_g4_prng

bash$ MCTOOLS_DATA_DIR=$(pwd) \
        __build-Linux-x86_64/testing/test_g4_detector_construction

bash$ MCTOOLS_DATA_DIR=$(pwd) \
        __build-Linux-x86_64/testing/test_step_hit_processor_factory

--
