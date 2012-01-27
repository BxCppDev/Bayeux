cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_mygsl )

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "${PROJECT_NAME} requires an out-of-source build." )
  message ( STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:" )
  message ( STATUS "  CMakeCache.txt" )
  message ( STATUS "  CMakeFiles" )
  message ( STATUS "Once these files are removed, create a separate directory" )
  message ( STATUS "and run CMake from there, pointing it to:" )
  message ( STATUS "  ${CMAKE_SOURCE_DIR}" )
  message ( FATAL_ERROR "in-source build detected" )
endif ()

# set ( _datatools_min_version 4.0 )
# find_package ( datatools ${_datatools_min_version} REQUIRED NO_MODULE )
# if ( datatools_VERSION )
#      message ( STATUS "Found datatools >= ${_datatools_min_version}" )
#      message ( STATUS "  datatools_VERSION             : ${datatools_VERSION}" )
#      message ( STATUS "  datatools_INCLUDE_DIRS        : ${datatools_INCLUDE_DIRS}" )
#      message ( STATUS "  datatools_LIBRARIES           : ${datatools_LIBRARIES}" )
# else ()
#      message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
# endif ()

set ( _mygsl_min_version 3.0 )
find_package ( mygsl ${_mygsl_min_version} REQUIRED NO_MODULE )
if ( mygsl_VERSION )
     message ( STATUS "Found mygsl >= ${_mygsl_min_version}" )
     message ( STATUS "  mygsl_CONFIG              : ${mygsl_CONFIG}" )
     message ( STATUS "  mygsl_VERSION             : ${mygsl_VERSION}" )
     message ( STATUS "  mygsl_INCLUDE_DIRS        : ${mygsl_INCLUDE_DIRS}" )
     message ( STATUS "  mygsl_LIBRARIES           : ${mygsl_LIBRARIES}" )
     message ( STATUS "  mygsl_WITH_DEBUG          : ${mygsl_WITH_DEBUG}" )
     #message ( STATUS "  mygsl_WITH_BIO            : ${mygsl_WITH_BIO}" )
     #message ( STATUS "  mygsl_WITH_PYTHON_WRAPPER : ${mygsl_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find mygsl >= ${_mygsl_min_version} library !" )
endif ()

set ( test_mygsl_src_files ${CMAKE_SOURCE_DIR}/../test_mygsl.cxx )

include_directories ( ${mygsl_INCLUDE_DIRS} )

add_executable ( test_mygsl
	         ${test_mygsl_src_files}
               )

target_link_libraries ( test_mygsl ${mygsl_LIBRARIES} )

install ( TARGETS test_mygsl DESTINATION bin )

# - end of top level CMakeLists.txt for test_mygsl project.
