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

# - Depends on mygsl:
if ( DEFINED mygsl_DIR )
    message ( STATUS "${PROJECT_NAME}: mygsl_DIR is : '${mygsl_DIR}' " )
else ()
    if ( NOT "x$ENV{MYGSL_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment MYGSL_CMAKE_CONFIG_DIR exists : '$ENV{MYGSL_CMAKE_CONFIG_DIR}' " )
      set ( mygsl_DIR "$ENV{MYGSL_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: mygsl_DIR is : '${mygsl_DIR}' " )
    endif ()
endif ()

set ( _mygsl_min_version 3.0 )
find_package ( mygsl ${_mygsl_min_version} REQUIRED NO_MODULE )
if ( mygsl_VERSION )
     message ( STATUS "Found mygsl >= ${_mygsl_min_version}" )
     message ( STATUS " o mygsl_CONFIG              : ${mygsl_CONFIG}" )
     message ( STATUS " o mygsl_VERSION             : ${mygsl_VERSION}" )
     message ( STATUS " o mygsl_INCLUDE_DIRS        : ${mygsl_INCLUDE_DIRS}" )
     message ( STATUS " o mygsl_LIBRARIES           : ${mygsl_LIBRARIES}" )
     message ( STATUS " o mygsl_WITH_DEBUG          : ${mygsl_WITH_DEBUG}" )
else ()
     message ( FATAL_ERROR "Cannot find mygsl >= ${_mygsl_min_version} library !" )
endif ()

set ( test_mygsl_src_files ${CMAKE_SOURCE_DIR}/../../test_mygsl.cxx )

include_directories ( ${mygsl_INCLUDE_DIRS} )

add_executable ( test_mygsl
	         ${test_mygsl_src_files}
               )

target_link_libraries ( test_mygsl ${mygsl_LIBRARIES} )

install ( TARGETS test_mygsl DESTINATION bin )

# - end of top level CMakeLists.txt for test_mygsl project.
