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

set (CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH} ${CMAKE_MODULE_PATH} )

# - Depends on datatools:
if ( DEFINED datatools_DIR )
    message ( STATUS "${PROJECT_NAME}: datatools_DIR is : '${datatools_DIR}' " )
else ()
    if ( NOT "x$ENV{DATATOOLS_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment DATATOOLS_CMAKE_CONFIG_DIR exists : '$ENV{DATATOOLS_CMAKE_CONFIG_DIR}' " )
      set ( datatools_DIR "$ENV{DATATOOLS_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: datatools_DIR is : '${datatools_DIR}' " )
    endif ()
endif ()

set ( _datatools_min_version 4.0 )
find_package ( datatools ${_datatools_min_version} REQUIRED NO_MODULE )
if ( datatools_CONFIG )
     message ( STATUS "Found datatools >= ${datatools_VERSION}" )
     message ( STATUS " o datatools_CONFIG       : ${datatools_CONFIG}" )
     message ( STATUS " o datatools_VERSION      : ${datatools_VERSION}" )
     message ( STATUS " o datatools_INCLUDE_DIRS : ${datatools_INCLUDE_DIRS}" )
     message ( STATUS " o datatools_LIBRARIES    : ${datatools_LIBRARIES}" )
else ()
     message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
endif ()

include_directories ( ../.. )
include_directories ( ${datatools_INCLUDE_DIRS} )

add_executable ( test_datatools      ${CMAKE_SOURCE_DIR}/../../test_datatools.cxx )
add_executable ( test_serializable_1 ${CMAKE_SOURCE_DIR}/../../test_serializable_1.cxx )

target_link_libraries ( test_datatools      ${datatools_LIBRARIES} )
target_link_libraries ( test_serializable_1 ${datatools_LIBRARIES} )

install ( TARGETS test_datatools      DESTINATION bin )
install ( TARGETS test_serializable_1 DESTINATION bin )

# - end of top level CMakeLists.txt for test_datatools project.
