
# -------------------------------------------------------------------------
# - genbb_help
#

if ( BAYEUX_WITH_GENBB_HELP )
  set ( _genbb_help_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/genbb_help )
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
  )
endif ( BAYEUX_WITH_GENBB_HELP ) 
