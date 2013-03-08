README.txt
----------

These  sample files and  are used  during the  testing step  of emfield.
They are  provided as  is as  examples of some  possible use  of emfield
classes.

----------------------------------------------------------------------

genbb_help: testing programs and scripts


Cd in the genbb_help base source directory :
 
bb0nu and bb2nu plots :
-----------------------

  bash$ ./testing/test_bb0nu_channel.sh

  It takes a while before BB energy plot is displayed.


Another test program :
----------------------

  bash$  ./testing/test_bb0nu_channel_2.sh 


Run the particle generator manager :
------------------------------------

  bash$ GENBB_HELP_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
    --particle-generator "bb0nu_Se82"

  bash$ GENBB_HELP_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
    --particle-generator "bkg_Bi214_Po214"

  bash$ GENBB_HELP_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
    --particle-generator "bkg_Bi214_Po214_process"

  bash$ GENBB_HELP_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
    --particle-generator "bkg_Bi212_Po212_from_files"

  bash$ GENBB_HELP_DATA_DIR=$(pwd) \
    __build-Linux-x86_64/testing/test_manager \
    --particle-generator "bkgd_model_0"


--
Updated: 2013-02-26
