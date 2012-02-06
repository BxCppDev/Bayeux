
# -------------------------------------------------------------------------
# - cuts
#

if ( BAYEUX_WITH_CUTS )
  set ( _cuts_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/cuts )
  ExternalProject_Add( cuts 
    DEPENDS datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/cuts/${BAYEUX_CUTS_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DCUTS_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DCUTS_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Ddatatools_DIR:PATH=${_datatools_DIR}
         ${_additional_cmake_module_path_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
  )
endif ( BAYEUX_WITH_CUTS ) 
