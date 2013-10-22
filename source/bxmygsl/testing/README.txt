These  sample files and  are used  during the  testing step  of mygsl.
They are  provided as  is as  examples of some  possible use  of mygsl
classes.



---------------------------------------------------------

Run the histogram service test program :

  bash$ MYGSL_TESTING_DIR=$(pwd)/testing \
        MYGSL_TMP_TEST_DIR=$(pwd) \
     __build-Linux-x86_64/testing/test_histogram_service \
        --export-root

  bash$ root test_histogram_service.root
  root [0]
  Attaching file test_histogram_service.root as _file0...
  root [1] TBrowser b;
  root [2] .q


--
