# - Top level CMakeLists.txt for brio project
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
  message ( STATUS "brio requires an out-of-source build." )
  message ( STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:" )
  message ( STATUS "  CMakeCache.txt" )
  message ( STATUS "  CMakeFiles" )
  message ( STATUS "Once these files are removed, create a separate directory" )
  message ( STATUS "and run CMake from there, pointing it to:" )
  message ( STATUS "  ${CMAKE_SOURCE_DIR}" )
  message ( FATAL_ERROR "in-source build detected" )
endif ()

# ----------------------------------------------------------------------------
# - Setup CMake requirements and project's name
#

cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( brio )

set ( brio_PACKAGE_NAME ${CMAKE_PROJECT_NAME} 
      CACHE STRING "The name of the package" FORCE 
    )
message ( STATUS "Package name         : '${brio_PACKAGE_NAME}'" )

# - include project configuration
include ( ${CMAKE_SOURCE_DIR}/cmake/Config.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/PackageFilename.cmake )

# ----------------------------------------------------------------------------
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

# ----------------------------------------------------------------------------
# - Dependencies :
#

# - Depends on Root:
set ( _root_min_version 5.26.00 ) 
set ( ROOT_MIN_VERSION ${_root_min_version} )
if ( NOT "$ENV{ROOT_ROOT_DIR}" STREQUAL "")  
  message ( STATUS "Environment ROOT_ROOT_DIR exists : '$ENV{ROOT_ROOT_DIR}' ")
  set ( ROOT_ROOT_DIR $ENV{ROOT_ROOT_DIR} )
else ()
  message ( WARNING  "Environment ROOT_ROOT_DIR is not set !" )
  if ( NOT "$ENV{ROOTSYS}" STREQUAL "")  
    message ( STATUS "Environment ROOTSYS exists : '$ENV{ROOTSYS}' ")
    set ( ROOT_ROOT_DIR $ENV{ROOTSYS} )
  else ()
    message ( WARNING  "Environment ROOTSYS is not set !" )
  endif ()
endif ()

find_package ( ROOT ${ROOT_MIN_VERSION} )
if ( ROOT_FOUND )
     message ( STATUS "Found ROOT")
     message ( STATUS "  ROOT_VERSION           = ${ROOTVERSION}" )
     message ( STATUS "  ROOT_CONFIG_EXECUTABLE = ${ROOT_CONFIG_EXECUTABLE}" )
     message ( STATUS "  ROOT_CINT_EXECUTABLE   = ${ROOT_CINT_EXECUTABLE}" )
     message ( STATUS "  ROOT_INCLUDE_DIR       = ${ROOT_INCLUDE_DIR}" )
     message ( STATUS "  ROOT_LIBRARIES         = ${ROOT_LIBRARIES}" )
     include_directories ( ${Boost_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find ROOT >= ${ROOT_MIN_VERSION} library !" )
endif ()

# - Depends on datatools:
set ( _datatools_min_version 4.0 )
find_package ( Datatools ${_datatools_min_version} )
if ( DATATOOLS_FOUND )
     message ( STATUS "Found datatools >= ${_datatools_min_version}" )
     message ( STATUS "  DATATOOLS_VERSION     : ${DATATOOLS_VERSION}" )
     message ( STATUS "  DATATOOLS_LIBRARIES   : ${DATATOOLS_LIBRARIES}" )
     message ( STATUS "  DATATOOLS_INCLUDE_DIRS : ${DATATOOLS_INCLUDE_DIRS}" )
     message ( STATUS "  DATATOOLS_INCLUDE_DIRS_DEPEND : ${DATATOOLS_INCLUDE_DIRS_DEPEND}" )
     message ( STATUS "  DATATOOLS_LIBRARIES_DEPEND   : ${DATATOOLS_LIBRARIES_DEPEND}" )
     include_directories ( ${DATATOOLS_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
endif ()

# ----------------------------------------------------------------------
# - Options :
#

option ( BRIO_WITH_DEBUG
         "Build the brio project using debugging code"
         OFF )

option ( BRIO_WITH_DOC 
         "Build the brio documentation" 
 	 ON )

option ( BRIO_WITH_TEST 
         "Build the brio test material" 
 	 ON )

if ( UNIX OR APPLE )
  option ( BRIO_WITH_CONFIG_SCRIPT 
           "Build the brio-config script" 
     	   ON )
  option ( BRIO_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( BRIO_WITH_CMAKE_FIND_MODULE 
         "Build the brio' CMake find module"
 	 ON )

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
if ( BRIO_WITH_CMAKE_FIND_MODULE )
  if ( DEFINED BRIO_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${BRIO_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "BRIO_FIND_MODULE_INSTALL_DIR : " "${BRIO_FIND_MODULE_INSTALL_DIR}" )
  endif ()
endif ( BRIO_WITH_CMAKE_FIND_MODULE )
message ( STATUS "_find_module_install_dir = '${_find_module_install_dir}'" )

message ( STATUS "BRIO_WITH_DEBUG           : " "${BRIO_WITH_DEBUG}" )
if (UNIX OR APPLE)
  message ( STATUS "BRIO_WITH_CONFIG_SCRIPT   : " "${BRIO_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "BRIO_WITH_PKGCONFIG       : " "${BRIO_WITH_PKGCONFIG}" )
endif (UNIX OR APPLE)
message ( STATUS "BRIO_WITH_DOC             : " "${BRIO_WITH_DOC}" )
message ( STATUS "BRIO_WITH_TEST            : " "${BRIO_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX      : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------
# - brio API :
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# - Configuration files :
#

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/brio_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/brio/brio_config.h" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/include/brio/brio_config.h
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/brio/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/brio_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/brio_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/brio_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _brio_pkgconfig_Requires 
     "Requires: datatools >= ${_datatools_min_version},ROOT >= ${_root_min_version}" 
    )

set ( _brio_pkgconfig_Libs "-lbrio" )

set ( _brio_pkgconfig_Libs 
      ${_brio_pkgconfig_Libs} 
      ${ROOT_LIBRARIES} 
      ${DATATOOLS_LIBRARIES} ) 

if ( BRIO_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindBrio.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindBrio.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindBrio.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

  set ( BRIO_CMAKE_MODULE_INSTALL_DIR ${_find_module_install_dir} 
        CACHE PATH "The CMake Module subdirectory" FORCE )

  message ( STATUS "BRIO_CMAKE_MODULE_INSTALL_DIR = '${BRIO_CMAKE_MODULE_INSTALL_DIR}' " )

endif ()

if ( UNIX OR APPLE )
  if ( BRIO_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/brio-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/brio-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/brio-config
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
# - Add documentation.
#
if ( BRIO_WITH_DOC )
  add_subdirectory ( doc )
endif ()

#----------------------------------------------------------------------------
# - Add testing - always for now.
#
if ( BRIO_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

###############################
# - packaging
###############################

include ( ${BRIO_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for brio project.
