
# -------------------------------------------------------------------------
# - datatools
#
set ( _datatools_DIR ${_install_prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/datatools )
  if (_bayeux_with_explicit_cadfael)
    set (_find_clhep_dir_option "-DCLHEP_ROOT_DIR:PATH=${Cadfael_ROOT_DIR}" )
    set (_find_boost_dir_option "-DBOOST_ROOT:PATH=${Cadfael_ROOT_DIR}" )
  endif ()

ExternalProject_Add( datatools 
   SVN_REPOSITORY ${BAYEUX_COMPONENTS_SVN_BASE_URL}/datatools/${BAYEUX_DATATOOLS_PATH}
   SVN_USERNAME ${_svn_username} 
   SVN_PASSWORD ${_svn_password}
   TIMEOUT 10
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix}
	-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	-DDATATOOLS_WITH_DEBUG=0 
	-DDATATOOLS_WITH_DOC=${BAYEUX_WITH_DOCS} 
	-DDATATOOLS_WITH_TEST=${BAYEUX_WITH_TESTS} 
	-DDATATOOLS_WITH_BIO=${BAYEUX_WITH_BIO} 
	-DDATATOOLS_WITH_PYTHON_WRAPPER=${BAYEUX_WITH_PYTHON_WRAPPER}
        ${_additional_cmake_module_path_option}
        ${_find_clhep_dir_option}
        ${_find_boost_dir_option}
   CMAKE_GENERATOR "Unix Makefiles"
   BUILD_COMMAND make -j${BAYEUX_PARALLEL_JOBS}
   INSTALL_COMMAND make install
)
