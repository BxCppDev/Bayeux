cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_genbb_help )

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

# - Depends on genbb_help:
if ( DEFINED genbb_help_DIR )
    message ( STATUS "${PROJECT_NAME}: genbb_help_DIR is : '${genbb_help_DIR}' " )
else ()
    if ( NOT "x$ENV{GENBB_HELP_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment GENBB_HELP_CMAKE_CONFIG_DIR exists : '$ENV{GENBB_HELP_CMAKE_CONFIG_DIR}' " )
      set ( genbb_help_DIR "$ENV{GENBB_HELP_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: genbb_help_DIR is : '${genbb_help_DIR}' " )
    endif ()
endif ()

set ( _genbb_help_min_version 3.0 )
find_package ( genbb_help ${_genbb_help_min_version} REQUIRED NO_MODULE )
if ( genbb_help_VERSION )
     message ( STATUS "${PROJECT_NAME}: Found genbb_help >= ${_genbb_help_min_version}" )
     message ( STATUS "${PROJECT_NAME}:  o genbb_help_CONFIG              : ${genbb_help_CONFIG}" )
     message ( STATUS "${PROJECT_NAME}:  o genbb_help_VERSION             : ${genbb_help_VERSION}" )
     message ( STATUS "${PROJECT_NAME}:  o genbb_help_INCLUDE_DIRS        : ${genbb_help_INCLUDE_DIRS}" )
     message ( STATUS "${PROJECT_NAME}:  o genbb_help_LIBRARIES           : ${genbb_help_LIBRARIES}" )
     message ( STATUS "${PROJECT_NAME}:  o genbb_help_WITH_DEBUG          : ${genbb_help_WITH_DEBUG}" )
     message ( STATUS "${PROJECT_NAME}:  o genbb_help_WITH_PYTHON_WRAPPER : ${genbb_help_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find genbb_help >= ${_genbb_help_min_version} library !" )
endif ()

# --------------------------------------------------------------------------
set ( test_genbb_help_src_files ${CMAKE_SOURCE_DIR}/../../test_genbb_help.cxx )

include_directories ( ${genbb_help_INCLUDE_DIRS} )

add_executable ( test_genbb_help
	         ${test_genbb_help_src_files}
               )

target_link_libraries ( test_genbb_help ${genbb_help_LIBRARIES} )

install ( TARGETS test_genbb_help DESTINATION bin )

# --------------------------------------------------------------------------
set ( test_genbb_mgr_src_files ${CMAKE_SOURCE_DIR}/../../test_genbb_mgr.cxx )

add_executable ( test_genbb_mgr
	         ${test_genbb_mgr_src_files}
               )

target_link_libraries ( test_genbb_mgr ${genbb_help_LIBRARIES} )

install ( TARGETS test_genbb_mgr DESTINATION bin )

# - end of top level CMakeLists.txt for test_genbb_help project.
