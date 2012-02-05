

# -------------------------------------------------------------------------
# - geomtools
#

if ( BAYEUX_WITH_GEOMTOOLS )
  set ( _geomtools_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/geomtools )
  ExternalProject_Add( geomtools 
    DEPENDS mygsl
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/geomtools/${BAYEUX_GEOMTOOLS_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DGEOMTOOLS_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DGEOMTOOLS_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Dmygsl_DIR:PATH=${_mygsl_DIR}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
  )
endif ( BAYEUX_WITH_GEOMTOOLS ) 

