
# -------------------------------------------------------------------------
# - genbb_help
#

if ( BAYEUX_WITH_GENBB_HELP )
  set ( _genbb_help_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/genbb_help )
  set (_genbb_help_run_post_build_tests OFF)
  if (BAYEUX_WITH_TESTS)
    set (_genbb_help_run_post_build_tests ON)
  endif ()
  
  if (_genbb_help_run_post_build_tests)
    set (_genbb_help_ep_test_options 
      TEST_BEFORE_INSTALL ${_test_before_install} 
      TEST_AFTER_INSTALL ${_test_after_install} 
      TEST_COMMAND make test 
      )
  endif ()
   ExternalProject_Add( genbb_help 
    DEPENDS geomtools mygsl datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/genbb_help/${BAYEUX_GENBB_HELP_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DGENBB_HELP_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DGENBB_HELP_WITH_TEST=${BAYEUX_WITH_TESTS} 
	 -DGENBB_HELP_WITH_BIO=${BAYEUX_WITH_BIO} 
	 -DGENBB_HELP_WITH_PYTHON_WRAPPER=${BAYEUX_WITH_PYTHON_WRAPPER}  
         -Dgeomtools_DIR:PATH=${_geomtools_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
    ${_genbb_help_ep_test_options}
  )

  if (BAYEUX_WITH_DOCS)
    ExternalProject_Add_Step ( genbb_help build_doc
      COMMAND make doc
      COMMENT "Build the genbb_help documentation material"
      DEPENDEES build
      DEPENDERS install
      WORKING_DIRECTORY <BINARY_DIR>
      )
  endif (BAYEUX_WITH_DOCS)

endif ( BAYEUX_WITH_GENBB_HELP ) 
