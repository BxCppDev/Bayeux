README.txt
----------

  bash$ export GENVTX_TESTING_DIR=$(pwd)/testing
  bash$ geomtools_check_setup \
    --load-dll materials \
    -3d \
    --with-mapping \
    --manager-config ${GENVTX_TESTING_DIR}/config/test-1.0/geometry/manager.conf \
    --mapping-categories ${GENVTX_TESTING_DIR}/config/test-1.0/geometry/categories.lis


--
