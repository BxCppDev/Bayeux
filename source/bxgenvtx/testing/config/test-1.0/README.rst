Testing genvtx
===============

Display the geometry setup:

.. raw:: sh

  bash$ export GENVTX_TESTING_DIR=$(pwd)/testing
  bash$ bxgeomtools_inspector \
    --manager-config ${GENVTX_TESTING_DIR}/config/test-1.0/geometry/manager.conf

Print the list of the available vertex generators:

.. raw:: sh

  bash$ bxgenvtx_production \
    --geometry-manager "${GENVTX_TESTING_DIR}/config/test-1.0/geometry/manager.conf" \
    --vertex-generator-manager "${GENVTX_TESTING_DIR}/config/test-1.0/vg_mgr.conf" \
    --list

Generate some vertexes:

.. raw:: sh

  bash$ bxgenvtx_production \
    --geometry-manager "${GENVTX_TESTING_DIR}/config/test-1.0/geometry/manager.conf" \
    --vertex-generator-manager "${GENVTX_TESTING_DIR}/config/test-1.0/vg_mgr.conf" \
    --shoot \
    --prng-seed 314159 \
    --number-of-vertices 1000 \
    --output-file "vertexes.data" \
    --vertex-generator "hall_bulk.vg" \
    --visu

.. raw:: sh

  bash$ bxgenvtx_production \
    --geometry-manager "${GENVTX_TESTING_DIR}/config/test-1.0/geometry/manager.conf" \
    --vertex-generator-manager "${GENVTX_TESTING_DIR}/config/test-1.0/vg_mgr.conf" \
    --shoot \
    --prng-seed 314159 \
    --number-of-vertices 1000 \
    --output-file "vertexes.data" \
    --vertex-generator "hall_roof.vg" \
    --visu

.. raw:: sh

  bash$ bxgenvtx_production \
    --geometry-manager "${GENVTX_TESTING_DIR}/config/test-1.0/geometry/manager.conf" \
    --vertex-generator-manager "${GENVTX_TESTING_DIR}/config/test-1.0/vg_mgr.conf" \
    --shoot \
    --prng-seed 314159 \
    --number-of-vertices 1000 \
    --output-file "vertexes.data" \
    --vertex-generator "ball_bb_bulk.vg" \
    --visu --visu-zoom 0.2
