
# -------------------------------------------------------------------------
# - trackfit
#

if ( BAYEUX_WITH_TRACKFIT )
  set ( _trackfit_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/trackfit )
  set (_trackfit_run_post_build_tests OFF)
  if (BAYEUX_WITH_TESTS)
    set (_trackfit_run_post_build_tests ON)
  endif ()
  
  if (_trackfit_run_post_build_tests)
   set (_trackfit_ep_test_options 
      TEST_BEFORE_INSTALL ${_test_before_install} 
      TEST_AFTER_INSTALL ${_test_after_install} 
      TEST_COMMAND make test 
      )
  endif ()
  ExternalProject_Add( trackfit 
    DEPENDS geomtools datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/trackfit/${BAYEUX_TRACKFIT_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
         -DCMAKE_MODULE_PATH:PATH=${CMAKE_MODULE_PATH}
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DTRACKFIT_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DTRACKFIT_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Dgeomtools_DIR:PATH=${_geomtools_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
    ${_trackfit_ep_test_options}
  )

  if (BAYEUX_WITH_DOCS)
    ExternalProject_Add_Step ( trackfit build_doc
      COMMAND make doc
      COMMENT "Build the trackfit documentation material"
      DEPENDEES build
      DEPENDERS install
      WORKING_DIRECTORY <BINARY_DIR>
      )
  endif (BAYEUX_WITH_DOCS)
endif ( BAYEUX_WITH_TRACKFIT ) 
