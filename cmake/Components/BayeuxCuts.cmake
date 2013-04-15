
# -------------------------------------------------------------------------
# - cuts
#

if ( BAYEUX_WITH_CUTS )
  set ( _cuts_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/cuts )
  set (_cuts_run_post_build_tests OFF)
  if (BAYEUX_WITH_TESTS)
    set (_cuts_run_post_build_tests ON)
  endif ()
  
  if (_cuts_run_post_build_tests)
   set (_cuts_ep_test_options 
      TEST_BEFORE_INSTALL ${_test_before_install} 
      TEST_AFTER_INSTALL ${_test_after_install} 
      TEST_COMMAND make test 
      )
  endif ()
  ExternalProject_Add( cuts 
    DEPENDS datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/cuts/${BAYEUX_CUTS_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
         -DCMAKE_MODULE_PATH:PATH=${CMAKE_MODULE_PATH}
    	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DCUTS_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DCUTS_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Ddatatools_DIR:PATH=${_datatools_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
    ${_cuts_ep_test_options}
  )

  if (BAYEUX_WITH_DOCS)
    ExternalProject_Add_Step ( cuts build_doc
      COMMAND make doc
      COMMENT "Build the cuts documentation material"
      DEPENDEES build
      DEPENDERS install
      WORKING_DIRECTORY <BINARY_DIR>
      )
  endif (BAYEUX_WITH_DOCS)

endif ( BAYEUX_WITH_CUTS ) 
