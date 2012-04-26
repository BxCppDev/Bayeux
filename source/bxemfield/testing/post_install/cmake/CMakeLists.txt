# -*- mode: cmake; -*-
# - Top level CMakeLists.txt for the test_emfield project.

cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_emfield )

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "${PROJECT_NAME}: ${PROJECT_NAME} requires an out-of-source build." )
  message ( STATUS "${PROJECT_NAME}: Please remove these files from ${CMAKE_BINARY_DIR} first:" )
  message ( STATUS "${PROJECT_NAME}:   CMakeCache.txt" )
  message ( STATUS "${PROJECT_NAME}:   CMakeFiles" )
  message ( STATUS "${PROJECT_NAME}: Once these files are removed, create a separate directory" )
  message ( STATUS "${PROJECT_NAME}: and run CMake from there, pointing it to:" )
  message ( STATUS "${PROJECT_NAME}:   ${CMAKE_SOURCE_DIR}" )
  message ( FATAL_ERROR "${PROJECT_NAME}: in-source build detected" )
endif ()

set ( _emfield_min_version 0.1.0 )
find_package ( emfield ${_emfield_min_version} REQUIRED NO_MODULE )
if ( emfield_CONFIG )
  message ( STATUS "${PROJECT_NAME}: Found emfield >= ${_emfield_min_version}" )
  message ( STATUS "${PROJECT_NAME}:  o emfield_CONFIG              : ${emfield_CONFIG}" )
  message ( STATUS "${PROJECT_NAME}:  o emfield_VERSION             : ${emfield_VERSION}" )
  message ( STATUS "${PROJECT_NAME}:  o emfield_INCLUDE_DIRS        : ${emfield_INCLUDE_DIRS}" )
  message ( STATUS "${PROJECT_NAME}:  o emfield_LIBRARIES           : ${emfield_LIBRARIES}" )
  message ( STATUS "${PROJECT_NAME}:  o emfield_WITH_DEBUG          : ${emfield_WITH_DEBUG}" )
  if (0)
    message ( STATUS "${PROJECT_NAME}:  o emfield_WITH_BIO : ${emfield_WITH_BIO}" )
  endif ()
  if (0)
    message ( STATUS "${PROJECT_NAME}:  o emfield_WITH_PYTHON_WRAPPER : ${emfield_WITH_PYTHON_WRAPPER}" )
  endif ()
else ()
  message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find emfield >= ${_emfield_min_version} library !" )
endif ()

set ( test_emfield_src_files ${CMAKE_SOURCE_DIR}/../../test_emfield.cxx )

include_directories ( ../.. )
include_directories ( ${emfield_INCLUDE_DIRS} )

add_executable ( test_emfield
  ${test_emfield_src_files}
  )

target_link_libraries ( test_emfield ${emfield_LIBRARIES} )

install ( TARGETS test_emfield DESTINATION bin )

# - end of top level CMakeLists.txt for the test_emfield project.
