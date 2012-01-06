# - Top level CMakeLists.txt for mygsl project
# -*- mode: cmake; -*-
#
#----------------------------------------------------------------------------
# Copyright (c) 2012, François Mauger <mauger@lpccaen.in2p3.fr>
#                     Ben Morgan <Ben.Morgan@warwick.ac.uk>
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "mygsl requires an out-of-source build." )
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
project ( mygsl )

set ( mygsl_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${mygsl_PACKAGE_NAME}'" )

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

### Depends on GSL:
set ( _gsl_min_version 1.14 ) 
set ( GSL_MIN_VERSION ${_gsl_min_version} )
if ( NOT "$ENV{GSL_ROOT_DIR}" STREQUAL "")  
  message ( STATUS "Environment GSL_GSL_DIR exists : '$ENV{GSL_ROOT_DIR}' ")
  set ( GSL_ROOT_DIR $ENV{GSL_ROOT_DIR} )
else ()
  message ( WARNING  "Environment GSL_ROOT_DIR is not set !" )
endif ()

find_package ( GSL ${GSL_MIN_VERSION} )
if ( GSL_FOUND )
     message ( STATUS "Found GSL")
     message ( STATUS "  GSL_VERSION           : ${GSL_VERSION}" )
     #message ( STATUS "  GSL_CONFIG_EXECUTABLE : ${GSL_CONFIG_EXECUTABLE}" )
     message ( STATUS "  GSL_INCLUDE_DIR       : ${GSL_INCLUDE_DIR}" )
     message ( STATUS "  GSL_LIBRARIES         : ${GSL_LIBRARIES}" )
     include_directories ( ${GSL_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find GSL >= ${GSL_MIN_VERSION} library !" )
endif ()

### Depends on datatools:
set ( _datatools_min_version 4.0 )
find_package ( Datatools ${_datatools_min_version} )
if ( DATATOOLS_FOUND )
     message ( STATUS "Found datatools >= ${_datatools_min_version}" )
     message ( STATUS "  DATATOOLS_VERSION      : ${DATATOOLS_VERSION}" )
     message ( STATUS "  DATATOOLS_LIBRARIES    : ${DATATOOLS_LIBRARIES}" )
     message ( STATUS "  DATATOOLS_INCLUDE_DIRS : ${DATATOOLS_INCLUDE_DIRS}" )
     message ( STATUS "  DATATOOLS_INCLUDE_DIRS_DEPEND : ${DATATOOLS_INCLUDE_DIRS_DEPEND}" )
     message ( STATUS "  DATATOOLS_LIBRARIES_DEPEND   : ${DATATOOLS_LIBRARIES_DEPEND}" )
     include_directories ( ${DATATOOLS_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
endif ()

# ----------------------------------------------------------------------
# Options :
#

option ( MYGSL_WITH_DEBUG
         "Build the mygsl project using debugging code"
         OFF )

option ( MYGSL_WITH_DOC 
         "Build the mygsl documentation" 
 	 ON )

option ( MYGSL_WITH_TEST 
         "Build the mygsl test material" 
 	 ON )

if ( UNIX OR APPLE )
  option ( MYGSL_WITH_CONFIG_SCRIPT 
           "Build the mygsl-config script" 
     	   ON )
  option ( MYGSL_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( MYGSL_WITH_CMAKE_FIND_MODULE 
         "Build the mygsl' CMake find module"
 	 ON )

if ( UNIX OR APPLE )
  option ( MYGSL_WITH_INSTALL_HERE
           "Installation path is stored in the mygsl source tree" 
     	   OFF )

  message ( STATUS "Default install prefix : " "${CMAKE_INSTALL_PREFIX}" )

  if ( MYGSL_WITH_INSTALL_HERE )
    message ( STATUS "DEBUG: MYGSL_WITH_INSTALL_HERE" )
    set ( mygsl_here_install_prefix 
          "${PROJECT_SOURCE_DIR}/.install_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" 
         )
    set ( CMAKE_INSTALL_PREFIX 
          "${mygsl_here_install_prefix}" 
  	  CACHE PATH "datatools installation prefix" FORCE
        )

    message ( STATUS "Please run 'cmake uninstall' to uninstall this build" )
    add_custom_target (uninstall cmake -E remove_directory ${mygsl_here_install_prefix} )
  endif ( MYGSL_WITH_INSTALL_HERE )
endif ()

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
message ( STATUS "_find_module_install_dir = '${_find_module_install_dir}'" )

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
if ( MYGSL_WITH_CMAKE_FIND_MODULE )
  if ( DEFINED MYGSL_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${MYGSL_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "MYGSL_FIND_MODULE_INSTALL_DIR : " "${MYGSL_FIND_MODULE_INSTALL_DIR}" )
  endif ()
endif ( MYGSL_WITH_CMAKE_FIND_MODULE )

message ( STATUS "MYGSL_WITH_DEBUG           : " "${MYGSL_WITH_DEBUG}" )
if (UNIX OR APPLE)
  message ( STATUS "MYGSL_WITH_CONFIG_SCRIPT   : " "${MYGSL_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "MYGSL_WITH_PKGCONFIG       : " "${MYGSL_WITH_PKGCONFIG}" )
  message ( STATUS "MYGSL_WITH_INSTALL_HERE    : " "${MYGSL_WITH_INSTALL_HERE}" )
endif (UNIX OR APPLE)
message ( STATUS "MYGSL_WITH_DOC             : " "${MYGSL_WITH_DOC}" )
message ( STATUS "MYGSL_WITH_TEST            : " "${MYGSL_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX      : " "${CMAKE_INSTALL_PREFIX}" )


# ----------------------------------------------------------------------
# mygsl API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#


configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/mygsl_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/mygsl/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/mygsl_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/mygsl_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/mygsl_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _mygsl_pkgconfig_Requires 
     "Requires: datatools >= ${_datatools_min_version},GSL >= ${_gsl_min_version}" 
    )

set ( _mygsl_pkgconfig_Libs "-lmygsl" )

set ( _mygsl_pkgconfig_Libs 
      ${_mygsl_pkgconfig_Libs} 
      ${GSL_LIBRARIES} 
      ${DATATOOLS_LIBRARIES} ) 

if ( MYGSL_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindMygsl.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindMygsl.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindMygsl.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

  set ( MYGSL_CMAKE_MODULE_INSTALL_DIR ${_find_module_install_dir} 
        CACHE PATH "The CMake Module subdirectory" FORCE )

  message ( STATUS "MYGSL_CMAKE_MODULE_INSTALL_DIR = '${MYGSL_CMAKE_MODULE_INSTALL_DIR}' " )

endif ()

if ( UNIX OR APPLE )
  if ( MYGSL_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/mygsl-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/mygsl-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/mygsl-config
              DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
                          GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
             )

    install ( FILES ${PROJECT_SOURCE_DIR}/pkgtools.d/mkuniqueflags.py
              DESTINATION ${INSTALL_MISC_DIR}/pkgtools
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
	                  GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
            )

  endif ()
endif ()  

install ( FILES README.txt COPYING.txt LICENSE.GPL3.txt
          DESTINATION ${INSTALL_MISC_DIR}
         )

#----------------------------------------------------------------------------
# Add documentation.
#
if ( MYGSL_WITH_DOC )
  add_subdirectory ( doc )
endif ()

#----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( MYGSL_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

###############################
# packaging
###############################

include ( ${MYGSL_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for mygsl project.
