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

set ( _cuts_min_version 3.0 )
find_package ( cuts ${_cuts_min_version} REQUIRED NO_MODULE )
if ( cuts_VERSION )
     message ( STATUS "Found cuts >= ${_cuts_min_version}" )
     message ( STATUS "  cuts_CONFIG              : ${cuts_CONFIG}" )
     message ( STATUS "  cuts_VERSION             : ${cuts_VERSION}" )
     message ( STATUS "  cuts_INCLUDE_DIRS        : ${cuts_INCLUDE_DIRS}" )
     message ( STATUS "  cuts_LIBRARIES           : ${cuts_LIBRARIES}" )
     message ( STATUS "  cuts_WITH_DEBUG          : ${cuts_WITH_DEBUG}" )
     message ( STATUS "  cuts_WITH_PYTHON_WRAPPER : ${cuts_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find cuts >= ${_cuts_min_version} library !" )
endif ()

set ( test_cuts_src_files ${CMAKE_SOURCE_DIR}/../test_cuts.cxx )

include_directories ( ${cuts_INCLUDE_DIRS} )

add_executable ( test_cuts
	         ${test_cuts_src_files}
               )

target_link_libraries ( test_cuts ${cuts_LIBRARIES} )

install ( TARGETS test_cuts DESTINATION bin )

# - end of top level CMakeLists.txt for test_cuts project.
