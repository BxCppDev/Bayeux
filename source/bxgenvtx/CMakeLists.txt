# - Top level CMakeLists.txt for genvtx project
# -*- mode: cmake; -*-
#
#----------------------------------------------------------------------------
# Copyright (c) 2012,  <>
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "genvtx requires an out-of-source build." )
  message ( STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:" )
  message ( STATUS "  CMakeCache.txt" )
  message ( STATUS "  CMakeFiles" )
  message ( STATUS "Once these files are removed, create a separate directory" )
  message ( STATUS "and run CMake from there, pointing it to:" )
  message ( STATUS "  ${CMAKE_SOURCE_DIR}" )
  message ( FATAL_ERROR "in-source build detected" )
endif ()

#----------------------------------------------------------------------------
# Setup CMake requirements and project's name
#
cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( genvtx )

set ( genvtx_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${genvtx_PACKAGE_NAME}'" )

# include project configuration
include ( ${CMAKE_SOURCE_DIR}/cmake/Config.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/PackageFilename.cmake )

#----------------------------------------------------------------------------
# - Prepend our own CMake Modules to the search path
# This allows us to customize, add, and factor out functionality
# NB: if our custom modules include others that we don't supply, those in the
# base path will be used, so watch for incompatibilities!!
#
message ( STATUS "Module path: ${CMAKE_MODULE_PATH} ${CMAKE_ROOT}" )

if ( NOT "$ENV{CMAKE_MODULE_PATH}" STREQUAL "" )
  message ( STATUS "Environment CMAKE_MODULE_PATH exists : '$ENV{CMAKE_MODULE_PATH}' " )
  set (__env_cmake_module_path "$ENV{CMAKE_MODULE_PATH}" )
  message ( STATUS "Environment CMAKE_MODULE_PATH : '$ENV{CMAKE_MODULE_PATH}'" )
  string ( REGEX REPLACE ":" ";" __env_cmake_module_path "${__env_cmake_module_path}" )
  set ( CMAKE_MODULE_PATH "${__env_cmake_module_path}" ${CMAKE_MODULE_PATH} )
endif ()
set ( CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake/Modules
    ${CMAKE_MODULE_PATH} )
message ( STATUS "CMAKE_ROOT        : '${CMAKE_ROOT}'" )
message ( STATUS "CMAKE_MODULE_PATH : '${CMAKE_MODULE_PATH}'" )

#
# Dependencies :
#

message ( STATUS "genvtx depends on " ) 

# - Depends on geomtools:
set ( _geomtools_min_version 3.0 )
find_package ( Geomtools ${_geomtools_min_version} )
if ( GEOMTOOLS_FOUND )
     message ( STATUS "Found geomtools >= ${_geomtools_min_version}" )
     message ( STATUS "  GEOMTOOLS_VERSION             : ${GEOMTOOLS_VERSION}" )
     message ( STATUS "  GEOMTOOLS_LIBRARIES           : ${GEOMTOOLS_LIBRARIES}" )
     message ( STATUS "  GEOMTOOLS_INCLUDE_DIRS        : ${GEOMTOOLS_INCLUDE_DIRS}" )
     message ( STATUS "  GEOMTOOLS_INCLUDE_DIRS_DEPEND : ${GEOMTOOLS_INCLUDE_DIRS_DEPEND}" )
     message ( STATUS "  GEOMTOOLS_LIBRARIES_DEPEND    : ${GEOMTOOLS_LIBRARIES_DEPEND}" )
else ()
     message ( FATAL_ERROR "Cannot find geomtools >= ${_geomtools_min_version} library !" )
endif ()

# ----------------------------------------------------------------------
# Options :
#

option ( GENVTX_WITH_DEBUG
         "Build the genvtx project using debugging code"
         OFF )

option ( GENVTX_WITH_DOC 
         "Build the genvtx documentation" 
 	 ON )

option ( GENVTX_WITH_TEST 
         "Build the genvtx test material" 
 	 ON )

if ( UNIX OR APPLE )
  option ( GENVTX_WITH_CONFIG_SCRIPT 
           "Build the genvtx-config script" 
     	   ON )
  option ( GENVTX_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( GENVTX_WITH_CMAKE_FIND_MODULE 
         "Build the genvtx' CMake find module"
 	 ON )

message ( STATUS "GENVTX_WITH_DEBUG           : " "${GENVTX_WITH_DEBUG}" )
if ( UNIX OR APPLE )
  message ( STATUS "GENVTX_WITH_CONFIG_SCRIPT   : " "${GENVTX_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "GENVTX_WITH_PKGCONFIG       : " "${GENVTX_WITH_PKGCONFIG}" )
endif ( UNIX OR APPLE )
message ( STATUS "GENVTX_WITH_DOC             : " "${GENVTX_WITH_DOC}" )
message ( STATUS "GENVTX_WITH_TEST            : " "${GENVTX_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX      : " "${CMAKE_INSTALL_PREFIX}" )


# ----------------------------------------------------------------------
# genvtx API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
if ( DEFINED GENVTX_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${GENVTX_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "GENVTX_FIND_MODULE_INSTALL_DIR : " "${GENVTX_FIND_MODULE_INSTALL_DIR}" )
endif ()
message ( STATUS "_find_module_install_dir : " "${_find_module_install_dir}" )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genvtx_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/genvtx/genvtx_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/genvtx/genvtx_config.h
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/genvtx/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genvtx_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/genvtx_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/genvtx_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# set ( _genvtx_pkgconfig_Requires 
#      "Requires: geomtools >= ${_geomtools_min_version},YYY >= ${_yyy_min_version}" 
#     )

set ( _genvtx_pkgconfig_Libs "-lgenvtx" )

set ( _genvtx_pkgconfig_Libs 
      ${_genvtx_pkgconfig_Libs} 
      ### ${YYY_LIBRARIES} 
      ### ${GEOMTOOLS_LIBRARIES} 
    ) 

if ( GENVTX_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindGenvtx.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindGenvtx.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindGenvtx.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( GENVTX_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genvtx-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/genvtx-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/genvtx-config
              DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
                          GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
             )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/mkuniqueflags.py
              DESTINATION ${INSTALL_MISC_DIR}/pkgtools/
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
	                  GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
            )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/libpkgtools.sh
              DESTINATION ${INSTALL_MISC_DIR}/pkgtools/
              PERMISSIONS OWNER_READ OWNER_WRITE 
                          GROUP_READ
                          WORLD_READ
             )

  endif ()
endif ()  

install ( FILES README.txt COPYING.txt LICENSE.GPL3.txt
          DESTINATION ${INSTALL_MISC_DIR}
         )

#----------------------------------------------------------------------------
# Add documentation.
#
if ( GENVTX_WITH_DOC )
  add_subdirectory ( doc )
endif ()

#----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( GENVTX_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

###############################
# packaging
###############################

include ( ${GENVTX_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for genvtx project.
