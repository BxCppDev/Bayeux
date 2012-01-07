# - Top level CMakeLists.txt for geomtools project
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
  message ( STATUS "geomtools requires an out-of-source build." )
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
project ( geomtools )

set ( geomtools_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${geomtools_PACKAGE_NAME}'" )

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

### Depends on mygsl:
set ( _mygsl_min_version 3.0 )
find_package ( Mygsl ${_mygsl_min_version} )
if ( MYGSL_FOUND )
     message ( STATUS "Found mygsl >= ${_mygsl_min_version}" )
     message ( STATUS "  MYGSL_VERSION      : ${MYGSL_VERSION}" )
     message ( STATUS "  MYGSL_LIBRARIES    : ${MYGSL_LIBRARIES}" )
     message ( STATUS "  MYGSL_INCLUDE_DIRS : ${MYGSL_INCLUDE_DIRS}" )
     message ( STATUS "  MYGSL_INCLUDE_DIRS_DEPEND : ${MYGSL_INCLUDE_DIRS_DEPEND}" )
     message ( STATUS "  MYGSL_LIBRARIES_DEPEND   : ${MYGSL_LIBRARIES_DEPEND}" )
     include_directories ( ${MYGSL_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find mygsl >= ${_mygsl_min_version} library !" )
endif ()

# ----------------------------------------------------------------------
# Options :
#

option ( GEOMTOOLS_WITH_DEBUG
         "Build the geomtools project using debugging code"
         OFF )

option ( GEOMTOOLS_WITH_BIO 
         "Generate an auxiliary DLL with Boost I/O code" 
 	 ON )

option ( GEOMTOOLS_WITH_PYTHON_WRAPPER 
         "Build the geomtools Python wrapper module" 
 	 OFF )

option ( GEOMTOOLS_WITH_DOC 
         "Build the geomtools documentation" 
 	 ON )

option ( GEOMTOOLS_WITH_TEST 
         "Build the geomtools test material" 
 	 ON )

if ( UNIX OR APPLE )
  option ( GEOMTOOLS_WITH_CONFIG_SCRIPT 
           "Build the geomtools-config script" 
     	   ON )
  option ( GEOMTOOLS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE 
         "Build the geomtools' CMake find module"
 	 ON )

if ( UNIX OR APPLE )
  option ( GEOMTOOLS_WITH_INSTALL_HERE
           "Installation path is stored in the geomtools source tree" 
     	   OFF )

  message ( STATUS "Default install prefix : " "${CMAKE_INSTALL_PREFIX}" )

  if ( GEOMTOOLS_WITH_INSTALL_HERE )
    message ( STATUS "DEBUG: GEOMTOOLS_WITH_INSTALL_HERE" )
    set ( geomtools_here_install_prefix 
          "${PROJECT_SOURCE_DIR}/.install_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" 
         )
    set ( CMAKE_INSTALL_PREFIX 
          "${geomtools_here_install_prefix}" 
  	  CACHE PATH "geomtools installation prefix" FORCE
        )

    message ( STATUS "Please run 'cmake uninstall' to uninstall this build" )
    add_custom_target (uninstall cmake -E remove_directory ${geomtools_here_install_prefix} )
  endif ( GEOMTOOLS_WITH_INSTALL_HERE )
endif ()

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
message ( STATUS "_find_module_install_dir = '${_find_module_install_dir}'" )

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
if ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE )
  if ( DEFINED GEOMTOOLS_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${GEOMTOOLS_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "GEOMTOOLS_FIND_MODULE_INSTALL_DIR : " "${GEOMTOOLS_FIND_MODULE_INSTALL_DIR}" )
  endif ()
endif ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE )

message ( STATUS "GEOMTOOLS_WITH_DEBUG           : " "${GEOMTOOLS_WITH_DEBUG}" )
message ( STATUS "GEOMTOOLS_WITH_BIO             : " "${GEOMTOOLS_WITH_BIO}" )
message ( STATUS "GEOMTOOLS_WITH_PYTHON_WRAPPER  : " "${GEOMTOOLS_WITH_PYTHON_WRAPPER}" )
if (UNIX OR APPLE)
  message ( STATUS "GEOMTOOLS_WITH_CONFIG_SCRIPT   : " "${GEOMTOOLS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "GEOMTOOLS_WITH_PKGCONFIG       : " "${GEOMTOOLS_WITH_PKGCONFIG}" )
  message ( STATUS "GEOMTOOLS_WITH_INSTALL_HERE    : " "${GEOMTOOLS_WITH_INSTALL_HERE}" )
endif (UNIX OR APPLE)
message ( STATUS "GEOMTOOLS_WITH_DOC             : " "${GEOMTOOLS_WITH_DOC}" )
message ( STATUS "GEOMTOOLS_WITH_TEST            : " "${GEOMTOOLS_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX      : " "${CMAKE_INSTALL_PREFIX}" )


# ----------------------------------------------------------------------
# geomtools API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#


configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/geomtools_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/geomtools/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/geomtools_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/geomtools_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/geomtools_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _geomtools_pkgconfig_Requires 
     "Requires: datatools >= ${_datatools_min_version},GSL >= ${_gsl_min_version}" 
    )

set ( _geomtools_pkgconfig_Libs "-lgeomtools" )

set ( _geomtools_pkgconfig_Libs 
      ${_geomtools_pkgconfig_Libs} 
      ${GSL_LIBRARIES} 
      ${DATATOOLS_LIBRARIES} ) 

if ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindGeomtools.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindGeomtools.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindGeomtools.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

  set ( GEOMTOOLS_CMAKE_MODULE_INSTALL_DIR ${_find_module_install_dir} 
        CACHE PATH "The CMake Module subdirectory" FORCE )

  message ( STATUS "GEOMTOOLS_CMAKE_MODULE_INSTALL_DIR = '${GEOMTOOLS_CMAKE_MODULE_INSTALL_DIR}' " )

endif ()

if ( UNIX OR APPLE )
  if ( GEOMTOOLS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/geomtools-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/geomtools-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/geomtools-config
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
if ( GEOMTOOLS_WITH_DOC )
  add_subdirectory ( doc )
endif ()

#----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( GEOMTOOLS_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

###############################
# packaging
###############################

include ( ${GEOMTOOLS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for geomtools project.
