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

set (CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH} ${CMAKE_MODULE_PATH} )

# - Depends on brio:
if ( DEFINED brio_DIR )
    message ( STATUS "${PROJECT_NAME}: brio_DIR is : '${brio_DIR}' " )
else ()
    if ( NOT "x$ENV{BRIO_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment BRIO_CMAKE_CONFIG_DIR exists : '$ENV{BRIO_CMAKE_CONFIG_DIR}' " )
      set ( brio_DIR "$ENV{BRIO_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: brio_DIR is : '${brio_DIR}' " )
    endif ()
endif ()

set ( _brio_min_version 3.0 )
find_package ( brio ${_brio_min_version} REQUIRED NO_MODULE )
if ( brio_CONFIG )
     message ( STATUS "Found brio >= ${brio_VERSION}" )
     message ( STATUS " o brio_CONFIG              : ${brio_CONFIG}" )
     message ( STATUS " o brio_VERSION             : ${brio_VERSION}" )
     message ( STATUS " o brio_INCLUDE_DIRS        : ${brio_INCLUDE_DIRS}" )
     message ( STATUS " o brio_LIBRARIES           : ${brio_LIBRARIES}" )
     message ( STATUS " o brio_WITH_DEBUG          : ${brio_WITH_DEBUG}" )
else ()
     message ( FATAL_ERROR "Cannot find brio >= ${_brio_min_version} library !" )
endif ()

include_directories ( ../.. )
include_directories ( ${brio_INCLUDE_DIRS} )

add_executable ( test_brio
	         ${CMAKE_SOURCE_DIR}/../../test_brio.cxx
               )
add_executable ( test_data_io
	         ${CMAKE_SOURCE_DIR}/../../test_data_io.cxx
               )

#target_link_libraries ( test_brio ${brio_LIBRARIES} )
#target_link_libraries ( test_data_io ${brio_LIBRARIES} )
target_link_libraries ( test_brio    ${brio_LIBRARY} )
target_link_libraries ( test_data_io ${brio_LIBRARY} )

install ( TARGETS test_brio    DESTINATION bin )
install ( TARGETS test_data_io DESTINATION bin )

# - end of top level CMakeLists.txt for test_brio project.
