
# -------------------------------------------------------------------------
# - brio
#

if ( BAYEUX_WITH_BRIO )
  if (_bayeux_with_explicit_cadfael)
    set (_find_root_dir_option "-DROOT_ROOT_DIR:PATH=${Cadfael_ROOT_DIR}" )
  endif ()
  set ( _brio_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/brio )
  ExternalProject_Add( brio 
    DEPENDS datatools
    SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/brio/${BAYEUX_BRIO_PATH}
    SVN_USERNAME ${_svn_username} 
    SVN_PASSWORD ${_svn_password}
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix} 
   	 -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	 -DBRIO_WITH_DEBUG=0 
	 -DBRIO_WITH_DOC=${BAYEUX_WITH_DOCS} 
	 -DBRIO_WITH_TEST=${BAYEUX_WITH_TESTS} 
         -Ddatatools_DIR:PATH=${_datatools_DIR}
         ${_additional_cmake_module_path_option}
         ${_find_root_dir_option}
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
    INSTALL_COMMAND make install
  )
endif ( BAYEUX_WITH_BRIO )
 
