cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_datatools )

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

set ( _datatools_min_version 4.0 )
find_package ( datatools ${_datatools_min_version} REQUIRED NO_MODULE )
if ( datatools_VERSION )
     message ( STATUS "Found datatools >= ${_datatools_min_version}" )
     message ( STATUS "  datatools_CONFIG       : ${datatools_CONFIG}" )
     message ( STATUS "  datatools_VERSION      : ${datatools_VERSION}" )
     message ( STATUS "  datatools_INCLUDE_DIRS : ${datatools_INCLUDE_DIRS}" )
     message ( STATUS "  datatools_LIBRARIES    : ${datatools_LIBRARIES}" )
else ()
     message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
endif ()

set ( test_datatools_src_files ${CMAKE_SOURCE_DIR}/../test_datatools.cxx )

include_directories ( ${datatools_INCLUDE_DIRS} )

add_executable ( test_datatools ${test_datatools_src_files} )

target_link_libraries ( test_datatools ${datatools_LIBRARIES} )

install ( TARGETS test_datatools DESTINATION bin )

# - end of top level CMakeLists.txt for test_datatools project.
