cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_cuts )

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

# - Depends on cuts:
if ( DEFINED cuts_DIR )
    message ( STATUS "${PROJECT_NAME}: cuts_DIR is : '${cuts_DIR}' " )
else ()
    if ( NOT "x$ENV{CUTS_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment CUTS_CMAKE_CONFIG_DIR exists : '$ENV{CUTS_CMAKE_CONFIG_DIR}' " )
      set ( cuts_DIR "$ENV{CUTS_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: cuts_DIR is : '${cuts_DIR}' " )
    endif ()
endif ()

set ( _cuts_min_version 3.1 )
find_package ( cuts ${_cuts_min_version} REQUIRED NO_MODULE )
if ( cuts_VERSION )
     message ( STATUS "Found cuts >= ${_cuts_min_version}" )
     message ( STATUS " o cuts_CONFIG              : ${cuts_CONFIG}" )
     message ( STATUS " o cuts_VERSION             : ${cuts_VERSION}" )
     message ( STATUS " o cuts_INCLUDE_DIRS        : ${cuts_INCLUDE_DIRS}" )
     message ( STATUS " o cuts_LIBRARIES           : ${cuts_LIBRARIES}" )
     message ( STATUS " o cuts_WITH_DEBUG          : ${cuts_WITH_DEBUG}" )
     message ( STATUS " o cuts_WITH_PYTHON_WRAPPER : ${cuts_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find cuts >= ${_cuts_min_version} library !" )
endif ()

set ( test_cuts_src_files ${CMAKE_SOURCE_DIR}/../../test_manager.cxx )

include_directories ( ../.. )
include_directories ( ${cuts_INCLUDE_DIRS} )

add_executable ( test_manager
	         ${test_cuts_src_files}
               )

#target_link_libraries ( test_manager ${cuts_LIBRARIES} )
target_link_libraries ( test_manager ${cuts_LIBRARY} )

install ( TARGETS test_manager DESTINATION bin )

# - end of top level CMakeLists.txt for test_cuts project.
