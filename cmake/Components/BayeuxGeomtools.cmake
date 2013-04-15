

# -------------------------------------------------------------------------
# - geomtools
#

if ( BAYEUX_WITH_GEOMTOOLS )
  set ( _geomtools_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/geomtools )
  set (_geomtools_run_post_build_tests OFF)
  if (BAYEUX_WITH_TESTS)
    set (_geomtools_run_post_build_tests ON)
  endif ()

  option ( BAYEUX_GEOMTOOLS_WITH_GNUPLOT_DISPLAY 
    "Build the Gnuplot-based display tools"
    1 )
  
  if (_geomtools_run_post_build_tests)
    set (_geomtools_ep_test_options 
      TEST_BEFORE_INSTALL ${_test_before_install} 
      TEST_AFTER_INSTALL ${_test_after_install} 
      TEST_COMMAND make test 
      )
  endif ()
   ExternalProject_Add( geomtools 
    DEPENDS mygsl
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/geomtools/${BAYEUX_GEOMTOOLS_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
         -DCMAKE_MODULE_PATH:PATH=${CMAKE_MODULE_PATH}
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DGEOMTOOLS_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DGEOMTOOLS_WITH_TEST=${BAYEUX_WITH_TESTS} 
	 -DGEOMTOOLS_WITH_BIO=${BAYEUX_WITH_BIO} 
	 -DGEOMTOOLS_WITH_GNUPLOT_DISPLAY=${BAYEUX_GEOMTOOLS_WITH_GNUPLOT_DISPLAY} 
         -Dmygsl_DIR:PATH=${_mygsl_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
    ${_geomtools_ep_test_options}
  )

  if (BAYEUX_WITH_DOCS)
    ExternalProject_Add_Step ( geomtools build_doc
      COMMAND make doc
      COMMENT "Build the geomtools documentation material"
      DEPENDEES build
      DEPENDERS install
      WORKING_DIRECTORY <BINARY_DIR>
      )
  endif (BAYEUX_WITH_DOCS)
endif ( BAYEUX_WITH_GEOMTOOLS ) 

