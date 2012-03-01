cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_genvtx )

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

# - Depends on genvtx:
if ( DEFINED genvtx_DIR )
    message ( STATUS "${PROJECT_NAME}: genvtx_DIR is : '${genvtx_DIR}' " )
else ()
    if ( NOT "x$ENV{GENVTX_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment GENVTX_CMAKE_CONFIG_DIR exists : '$ENV{GENVTX_CMAKE_CONFIG_DIR}' " )
      set ( genvtx_DIR "$ENV{GENVTX_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: genvtx_DIR is : '${genvtx_DIR}' " )
    endif ()
endif ()

set ( _genvtx_min_version 3.0 )
find_package ( genvtx ${_genvtx_min_version} REQUIRED NO_MODULE )
if ( genvtx_VERSION )
     message ( STATUS "Found genvtx >= ${_genvtx_min_version}" )
     message ( STATUS " o genvtx_CONFIG              : ${genvtx_CONFIG}" )
     message ( STATUS " o genvtx_VERSION             : ${genvtx_VERSION}" )
     message ( STATUS " o genvtx_INCLUDE_DIRS        : ${genvtx_INCLUDE_DIRS}" )
     message ( STATUS " o genvtx_LIBRARIES           : ${genvtx_LIBRARIES}" )
     message ( STATUS " o genvtx_WITH_DEBUG          : ${genvtx_WITH_DEBUG}" )
     message ( STATUS " o genvtx_WITH_PYTHON_WRAPPER : ${genvtx_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find genvtx >= ${_genvtx_min_version} library !" )
endif ()

set ( test_genvtx_src_files ${CMAKE_SOURCE_DIR}/../../test_genvtx.cxx )

include_directories ( ../.. )
include_directories ( ${genvtx_INCLUDE_DIRS} )

add_executable ( test_genvtx
	         ${test_genvtx_src_files}
               )

target_link_libraries ( test_genvtx ${genvtx_LIBRARIES} )

install ( TARGETS test_genvtx DESTINATION bin )

# - end of top level CMakeLists.txt for test_genvtx project.
