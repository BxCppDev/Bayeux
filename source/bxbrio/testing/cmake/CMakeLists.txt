cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_brio )

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

set ( _brio_min_version 3.0 )
find_package ( brio ${_brio_min_version} REQUIRED NO_MODULE )
if ( brio_VERSION )
     message ( STATUS "Found brio >= ${_brio_min_version}" )
     message ( STATUS "  brio_CONFIG              : ${brio_CONFIG}" )
     message ( STATUS "  brio_VERSION             : ${brio_VERSION}" )
     message ( STATUS "  brio_INCLUDE_DIRS        : ${brio_INCLUDE_DIRS}" )
     message ( STATUS "  brio_LIBRARIES           : ${brio_LIBRARIES}" )
     message ( STATUS "  brio_WITH_DEBUG          : ${brio_WITH_DEBUG}" )
     #message ( STATUS "  brio_WITH_BIO            : ${brio_WITH_BIO}" )
     #message ( STATUS "  brio_WITH_PYTHON_WRAPPER : ${brio_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find brio >= ${_brio_min_version} library !" )
endif ()

set ( test_brio_src_files ${CMAKE_SOURCE_DIR}/../test_brio.cxx )

include_directories ( ${brio_INCLUDE_DIRS} )

add_executable ( test_brio
	         ${test_brio_src_files}
               )

target_link_libraries ( test_brio ${brio_LIBRARIES} )

install ( TARGETS test_brio DESTINATION bin )

# - end of top level CMakeLists.txt for test_brio project.
