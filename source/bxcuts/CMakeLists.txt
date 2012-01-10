# - Top level CMakeLists.txt for cuts project
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
  message ( STATUS "cuts requires an out-of-source build." )
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
project ( cuts )

set ( cuts_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${cuts_PACKAGE_NAME}'" )

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

message ( STATUS "cuts depends on " ) 

# - Depends on datatools:
set ( _datatools_min_version 4.0 )
find_package ( Datatools ${_datatools_min_version} )
if ( DATATOOLS_FOUND )
     message ( STATUS "Found datatools >= ${_datatools_min_version}" )
     message ( STATUS "  DATATOOLS_VERSION             : ${DATATOOLS_VERSION}" )
     message ( STATUS "  DATATOOLS_LIBRARIES           : ${DATATOOLS_LIBRARIES}" )
     message ( STATUS "  DATATOOLS_INCLUDE_DIRS        : ${DATATOOLS_INCLUDE_DIRS}" )
     message ( STATUS "  DATATOOLS_INCLUDE_DIRS_DEPEND : ${DATATOOLS_INCLUDE_DIRS_DEPEND}" )
     message ( STATUS "  DATATOOLS_LIBRARIES_DEPEND    : ${DATATOOLS_LIBRARIES_DEPEND}" )
else ()
     message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
endif ()

# ----------------------------------------------------------------------
# Options :
#

option ( CUTS_WITH_DEBUG
         "Build the cuts project using debugging code"
         OFF )

option ( CUTS_WITH_DOC 
         "Build the cuts documentation" 
 	 ON )

option ( CUTS_WITH_TEST 
         "Build the cuts test material" 
 	 ON )

if ( UNIX OR APPLE )
  option ( CUTS_WITH_CONFIG_SCRIPT 
           "Build the cuts-config script" 
     	   ON )
  option ( CUTS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( CUTS_WITH_CMAKE_FIND_MODULE 
         "Build the cuts' CMake find module"
 	 ON )

message ( STATUS "CUTS_WITH_DEBUG           : " "${CUTS_WITH_DEBUG}" )
if ( UNIX OR APPLE )
  message ( STATUS "CUTS_WITH_CONFIG_SCRIPT   : " "${CUTS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "CUTS_WITH_PKGCONFIG       : " "${CUTS_WITH_PKGCONFIG}" )
endif ( UNIX OR APPLE )
message ( STATUS "CUTS_WITH_DOC             : " "${CUTS_WITH_DOC}" )
message ( STATUS "CUTS_WITH_TEST            : " "${CUTS_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX      : " "${CMAKE_INSTALL_PREFIX}" )


# ----------------------------------------------------------------------
# cuts API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
if ( DEFINED CUTS_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${CUTS_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "CUTS_FIND_MODULE_INSTALL_DIR : " "${CUTS_FIND_MODULE_INSTALL_DIR}" )
endif ()
message ( STATUS "_find_module_install_dir : " "${_find_module_install_dir}" )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cuts_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/cuts/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cuts_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/cuts_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/cuts_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _cuts_pkgconfig_Requires 
      "Requires: datatools >= ${_datatools_min_version} "
     )

set ( _cuts_pkgconfig_Libs "-lcuts" )

set ( _cuts_pkgconfig_Libs 
      ${_cuts_pkgconfig_Libs} 
      ${DATATOOLS_LIBRARIES_DEPENDS} 
    ) 

if ( CUTS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindCuts.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindCuts.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindCuts.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( CUTS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cuts-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/cuts-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/cuts-config
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
if ( CUTS_WITH_DOC )
  add_subdirectory ( doc )
endif ()

#----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( CUTS_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

###############################
# packaging
###############################

include ( ${CUTS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for cuts project.
