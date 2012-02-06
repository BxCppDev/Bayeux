

# -------------------------------------------------------------------------
# - genvtx
#

if ( BAYEUX_WITH_GENVTX )
  set ( _genvtx_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/genvtx )
  ExternalProject_Add( genvtx 
    DEPENDS geomtools mygsl
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/genvtx/${BAYEUX_GENVTX_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DGENVTX_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DGENVTX_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Dgeomtools_DIR:PATH=${_geomtools_DIR}
         ${_additional_cmake_module_path_option}
     CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
  )
endif ( BAYEUX_WITH_GENVTX ) 

