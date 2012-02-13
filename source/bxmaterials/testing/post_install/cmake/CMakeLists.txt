cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( test_material )

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

# - Depends on materials:
if ( DEFINED materials_DIR )
    message ( STATUS "${PROJECT_NAME}: materials_DIR is : '${materials_DIR}' " )
else ()
    if ( NOT "x$ENV{MATERIALS_CMAKE_CONFIG_DIR}" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: Environment MATERIALS_CMAKE_CONFIG_DIR exists : '$ENV{MATERIALS_CMAKE_CONFIG_DIR}' " )
      set ( materials_DIR "$ENV{MATERIALS_CMAKE_CONFIG_DIR}" )
      message ( STATUS "${PROJECT_NAME}: materials_DIR is : '${materials_DIR}' " )
    endif ()
endif ()

set ( _materials_min_version 3.0 )
find_package ( materials ${_materials_min_version} REQUIRED NO_MODULE )
if ( materials_VERSION )
     message ( STATUS "Found materials >= ${_materials_min_version}" )
     message ( STATUS " o materials_CONFIG              : ${materials_CONFIG}" )
     message ( STATUS " o materials_VERSION             : ${materials_VERSION}" )
     message ( STATUS " o materials_INCLUDE_DIRS        : ${materials_INCLUDE_DIRS}" )
     message ( STATUS " o materials_LIBRARIES           : ${materials_LIBRARIES}" )
     message ( STATUS " o materials_WITH_DEBUG          : ${materials_WITH_DEBUG}" )
     message ( STATUS " o materials_WITH_PYTHON_WRAPPER : ${materials_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find materials >= ${_materials_min_version} library !" )
endif ()

set ( test_material_src_files ${CMAKE_SOURCE_DIR}/../../test_material.cxx )

include_directories ( ../.. )
include_directories ( ${materials_INCLUDE_DIRS} )

add_executable ( test_material
	         ${test_material_src_files}
               )

target_link_libraries ( test_material ${materials_LIBRARIES} )

install ( TARGETS test_material DESTINATION bin )

# - end of top level CMakeLists.txt for test_material project.
