# - Top level CMakeLists.txt for genbb_help project
# -*- mode: cmake; -*-
#
#----------------------------------------------------------------------------
# Copyright (C) 2012 François Mauger, LPC Caen CNRS/IN2P3 and Université de Caen Basse-Normandie
# Contact: mauger@lpccaen.in2p3.fr
#----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "genbb_help requires an out-of-source build." )
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
project ( genbb_help  C CXX Fortran )

set ( genbb_help_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${genbb_help_PACKAGE_NAME}'" )

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

message ( STATUS "genbb_help depends on " ) 

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

# - Depends on mygsl:
set ( _mygsl_min_version 3.0 )
find_package ( Mygsl ${_mygsl_min_version} )
if ( MYGSL_FOUND )
     message ( STATUS "Found mygsl >= ${_mygsl_min_version}" )
     message ( STATUS "  MYGSL_VERSION             : ${MYGSL_VERSION}" )
     message ( STATUS "  MYGSL_LIBRARIES           : ${MYGSL_LIBRARIES}" )
     message ( STATUS "  MYGSL_INCLUDE_DIRS        : ${MYGSL_INCLUDE_DIRS}" )
     message ( STATUS "  MYGSL_INCLUDE_DIRS_DEPEND : ${MYGSL_INCLUDE_DIRS_DEPEND}" )
     message ( STATUS "  MYGSL_LIBRARIES_DEPEND    : ${MYGSL_LIBRARIES_DEPEND}" )
else ()
     message ( FATAL_ERROR "Cannot find mygsl >= ${_mygsl_min_version} library !" )
endif ()

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

option ( GENBB_HELP_WITH_DEBUG
         "Build the genbb_help project using debugging code"
         OFF )

option ( GENBB_HELP_WITH_BIO 
         "Generate an auxiliary DLL with Boost I/O code" 
 	 ON )

option ( GENBB_HELP_WITH_DOC 
         "Build the genbb_help documentation" 
 	 ON )

option ( GENBB_HELP_WITH_TEST 
         "Build the genbb_help test material" 
 	 ON )

if ( UNIX OR APPLE )
  option ( GENBB_HELP_WITH_CONFIG_SCRIPT 
           "Build the genbb_help-config script" 
     	   ON )
  option ( GENBB_HELP_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( GENBB_HELP_WITH_CMAKE_FIND_MODULE 
         "Build the genbb_help' CMake find module"
 	 ON )

message ( STATUS "GENBB_HELP_WITH_DEBUG           : " "${GENBB_HELP_WITH_DEBUG}" )
message ( STATUS "GENBB_HELP_WITH_BIO             : " "${GENBB_HELP_WITH_BIO}" )
if ( UNIX OR APPLE )
  message ( STATUS "GENBB_HELP_WITH_CONFIG_SCRIPT   : " "${GENBB_HELP_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "GENBB_HELP_WITH_PKGCONFIG       : " "${GENBB_HELP_WITH_PKGCONFIG}" )
endif ( UNIX OR APPLE )
message ( STATUS "GENBB_HELP_WITH_DOC             : " "${GENBB_HELP_WITH_DOC}" )
message ( STATUS "GENBB_HELP_WITH_TEST            : " "${GENBB_HELP_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX      : " "${CMAKE_INSTALL_PREFIX}" )


# ----------------------------------------------------------------------
# genbb_help API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
if ( DEFINED GENBB_HELP_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${GENBB_HELP_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "GENBB_HELP_FIND_MODULE_INSTALL_DIR : " "${GENBB_HELP_FIND_MODULE_INSTALL_DIR}" )
endif ()
message ( STATUS "_find_module_install_dir : " "${_find_module_install_dir}" )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/genbb_help/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/genbb_help_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/genbb_help_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _genbb_help_pkgconfig_Requires 
     "Requires: geomtools >= ${_geomtools_min_version}" 
    )

set ( _genbb_help_pkgconfig_Libs "-lgenbb_help" )

set ( _genbb_help_pkgconfig_Libs 
      ${_genbb_help_pkgconfig_Libs} 
      ${GEOMTOOLS_LIBRARIES_DEPEND} 
    ) 

if ( GENBB_HELP_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindGenbbHelp.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindGenbbHelp.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindGenbbHelp.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( GENBB_HELP_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/genbb_help-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/genbb_help-config
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
# Add production programs.
#
add_subdirectory ( programs )

#----------------------------------------------------------------------------
# Add documentation.
#
if ( GENBB_HELP_WITH_DOC )
  add_subdirectory ( doc )
endif ()

#----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( GENBB_HELP_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

###############################
# packaging
###############################

include ( ${GENBB_HELP_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for genbb_help project.
