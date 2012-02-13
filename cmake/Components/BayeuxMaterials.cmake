
# -------------------------------------------------------------------------
# - materials
#

if ( BAYEUX_WITH_MATERIALS )
  set ( _materials_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/materials )
  set (_materials_run_post_build_tests OFF)
  if (BAYEUX_WITH_TESTS)
    set (_materials_run_post_build_tests ON)
  endif ()
  
  if (_materials_run_post_build_tests)
    set (_materials_ep_test_options TEST_BEFORE_INSTALL 1 TEST_COMMAND make test )
  endif ()
  ExternalProject_Add( materials 
    DEPENDS geomtools datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/materials/${BAYEUX_MATERIALS_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DMATERIALS_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DMATERIALS_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Dgeomtools_DIR:PATH=${_geomtools_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
    ${_materials_ep_test_options}
  )
endif ( BAYEUX_WITH_MATERIALS ) 
