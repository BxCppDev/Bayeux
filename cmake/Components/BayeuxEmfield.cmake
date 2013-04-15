
# -------------------------------------------------------------------------
# - emfield
#

if ( BAYEUX_WITH_EMFIELD )
  set ( _emfield_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/emfield )
  set (_emfield_run_post_build_tests OFF)
  if (BAYEUX_WITH_TESTS)
    set (_emfield_run_post_build_tests ON)
  endif ()
  
  if (_emfield_run_post_build_tests)
   set (_emfield_ep_test_options 
      TEST_BEFORE_INSTALL ${_test_before_install} 
      TEST_AFTER_INSTALL ${_test_after_install} 
      TEST_COMMAND make test 
      )
  endif ()
  ExternalProject_Add( emfield 
    DEPENDS geomtools #datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/emfield/${BAYEUX_EMFIELD_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
         -DCMAKE_MODULE_PATH:PATH=${CMAKE_MODULE_PATH}
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DEMFIELD_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DEMFIELD_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Dgeomtools_DIR:PATH=${_geomtools_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
    ${_emfield_ep_test_options}
  )

  if (BAYEUX_WITH_DOCS)
    ExternalProject_Add_Step ( emfield build_doc
      COMMAND make doc
      COMMENT "Build the emfield documentation material"
      DEPENDEES build
      DEPENDERS install
      WORKING_DIRECTORY <BINARY_DIR>
      )
  endif (BAYEUX_WITH_DOCS)
endif ( BAYEUX_WITH_EMFIELD ) 
