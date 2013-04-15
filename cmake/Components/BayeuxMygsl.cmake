 

# -------------------------------------------------------------------------
# - mygsl
#

if ( BAYEUX_WITH_MYGSL )
  set ( _mygsl_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/mygsl )
  set (_mygsl_run_post_build_tests OFF)
  if (BAYEUX_WITH_TESTS)
    set (_mygsl_run_post_build_tests ON)
  endif ()
  
  if (_mygsl_run_post_build_tests)
    set (_mygsl_ep_test_options 
      TEST_BEFORE_INSTALL ${_test_before_install} 
      TEST_AFTER_INSTALL ${_test_after_install} 
      TEST_COMMAND make test 
      )
  endif ()
  ExternalProject_Add( mygsl 
    DEPENDS datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/mygsl/${BAYEUX_MYGSL_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
         -DCMAKE_MODULE_PATH:PATH=${CMAKE_MODULE_PATH}
  	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DMYGSL_WITH_DEBUG=0 
	 -DMYGSL_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DMYGSL_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Ddatatools_DIR:PATH=${_datatools_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
    ${_mygsl_ep_test_options}
  )

  if (BAYEUX_WITH_DOCS)
    ExternalProject_Add_Step ( mygsl build_doc
      COMMAND make doc
      COMMENT "Build the mygsl documentation material"
      DEPENDEES build
      DEPENDERS install
      WORKING_DIRECTORY <BINARY_DIR>
      )
  endif (BAYEUX_WITH_DOCS)
endif ( BAYEUX_WITH_MYGSL ) 

