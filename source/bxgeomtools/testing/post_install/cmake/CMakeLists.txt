cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_geomtools )

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

# - Depends on geomtools:
if ( DEFINED geomtools_DIR )
    message ( STATUS "${PROJECT_NAME}: geomtools_DIR is : '${geomtools_DIR}' " )
else ()
    if ( NOT "x$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment GEOMTOOLS_CMAKE_CONFIG_DIR exists : '$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}' " )
      set ( geomtools_DIR "$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: geomtools_DIR is : '${geomtools_DIR}' " )
    endif ()
endif ()

set ( _geomtools_min_version 3.0 )
find_package ( geomtools ${_geomtools_min_version} REQUIRED NO_MODULE )
if ( geomtools_VERSION )
     message ( STATUS "${PROJECT_NAME}: Found geomtools >= ${_geomtools_min_version}" )
     message ( STATUS "${PROJECT_NAME}:  o geomtools_CONFIG              : ${geomtools_CONFIG}" )
     message ( STATUS "${PROJECT_NAME}:  o geomtools_VERSION             : ${geomtools_VERSION}" )
     message ( STATUS "${PROJECT_NAME}:  o geomtools_INCLUDE_DIRS        : ${geomtools_INCLUDE_DIRS}" )
     message ( STATUS "${PROJECT_NAME}:  o geomtools_LIBRARIES           : ${geomtools_LIBRARIES}" )
     message ( STATUS "${PROJECT_NAME}:  o geomtools_WITH_DEBUG          : ${geomtools_WITH_DEBUG}" )
     message ( STATUS "${PROJECT_NAME}:  o geomtools_WITH_BIO            : ${geomtools_WITH_BIO}" )
     #message ( STATUS "${PROJECT_NAME}:  o geomtools_WITH_PYTHON_WRAPPER : ${geomtools_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find geomtools >= ${_geomtools_min_version} library !" )
endif ()

include_directories ( ../.. )
include_directories ( ${geomtools_INCLUDE_DIRS} )

add_executable ( test_geomtools    ${CMAKE_SOURCE_DIR}/../../test_geomtools.cxx    )
add_executable ( test_serializable ${CMAKE_SOURCE_DIR}/../../test_serializable.cxx )

target_link_libraries ( test_geomtools    ${geomtools_LIBRARIES} )
target_link_libraries ( test_serializable ${geomtools_LIBRARIES} )

install ( TARGETS test_geomtools    DESTINATION bin )
install ( TARGETS test_serializable DESTINATION bin )

# - end of top level CMakeLists.txt for test_geomtools project.
