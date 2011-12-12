# - Top level CMakeLists.txt for datatools project
# -*- mode: cmake; -*-
#
#----------------------------------------------------------------------------
# Copyright (c) 2011, François Mauger <mauger@lpccaen.in2p3.fr>
#                     Ben Morgan <Ben.Morgan@warwick.ac.uk>
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "datatools requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#----------------------------------------------------------------------------
# Setup CMake requirements and project's name
#
cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( datatools )

set ( datatools_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message (STATUS "Package name         : '${datatools_PACKAGE_NAME}'" )

# include project configuration
include(${CMAKE_SOURCE_DIR}/cmake/Config.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/PackageFilename.cmake)

#----------------------------------------------------------------------------
# - Prepend our own CMake Modules to the search path
# This allows us to customize, add, and factor out functionality
# NB: if our custom modules include others that we don't supply, those in the
# base path will be used, so watch for incompatibilities!!
#
message ( STATUS "Module path: ${CMAKE_MODULE_PATH} ${CMAKE_ROOT}" )

if ( NOT "$ENV{CMAKE_MODULE_PATH}" STREQUAL "" )
  message (STATUS "Environment CMAKE_MODULE_PATH exists : '$ENV{CMAKE_MODULE_PATH}' ")
  set (__env_cmake_module_path "$ENV{CMAKE_MODULE_PATH}" )
  message (STATUS "Environment CMAKE_MODULE_PATH : '$ENV{CMAKE_MODULE_PATH}'" )
  string ( REGEX REPLACE ":" ";" __env_cmake_module_path "${__env_cmake_module_path}" )
  set (CMAKE_MODULE_PATH "${__env_cmake_module_path}" ${CMAKE_MODULE_PATH} )
endif ()
set ( CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake/Modules
    ${CMAKE_MODULE_PATH} )
message (STATUS "CMAKE_ROOT        : '${CMAKE_ROOT}'" )
message (STATUS "CMAKE_MODULE_PATH : '${CMAKE_MODULE_PATH}'" )

#
# Dependencies :
#

### Depends on Boost:
set (_boost_version 1.47.0 )
if ( NOT "$ENV{BOOST_ROOT}" STREQUAL "")  
  message ( STATUS "Environment BOOST_ROOT exists : '$ENV{BOOST_ROOT}' ")
  set ( BOOST_ROOT $ENV{BOOST_ROOT} )
else ()
  message ( WARNING  "Environment BOOST_ROOT is not set !" )
endif ()

string ( REGEX REPLACE ".[0-9]+$" "" _boost_short_version "${_boost_version}" )
message (STATUS "Boost short version is : '${_boost_short_version}'")
set ( Boost_ADDITIONAL_VERSIONS "${_boost_short_version}" "${_boost_version}" )
set ( _boost_requested_components filesystem serialization iostreams system )

find_package ( Boost ${_boost_version} REQUIRED COMPONENTS ${_boost_requested_components} )
if ( Boost_FOUND )
     message (STATUS "Found Boost ${_boost_version}")
     message (STATUS "  Boost_VERSION          = ${Boost_VERSION}")
     message (STATUS "  Boost_LIB_VERSION      = ${Boost_LIB_VERSION}")
     include_directories ( ${Boost_INCLUDE_DIRS} )
else ()
     message (FATAL_ERROR "Cannot find Boost ${_boost_version} library !")
endif ()

### Depends on CLHEP:
set (_clhep_version 2.1.0.1 )
find_package( CLHEP ${_clhep_version} REQUIRED)
if ( CLHEP_FOUND )
     message (STATUS "Found CLHEP ${_clhep_version}")
     include_directories ( ${CLHEP_INCLUDE_DIRS} )
else ()
     message (FATAL_ERROR "Cannot find CLHEP ${_clhep_version} library !")
endif ()

# ----------------------------------------------------------------------
# Options :
#

option ( DATATOOLS_WITH_DEBUG
         "Build the datatools project using debugging code"
         OFF )

option ( DATATOOLS_WITH_BIO 
         "Generate an auxiliary DLL with Boost I/O code" 
 	 ON )

option ( DATATOOLS_WITH_PYTHON_WRAPPER 
         "Build the datatools Python wrapper module" 
 	 OFF )

option ( DATATOOLS_WITH_DOC 
         "Build the datatools documentation" 
 	 ON )

option ( DATATOOLS_WITH_TEST 
         "Build the datatools test material" 
 	 OFF )

if (UNIX OR APPLE)
  option ( DATATOOLS_WITH_CONFIG_SCRIPT 
           "Build the datatools-config script" 
     	   ON )
  option ( DATATOOLS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif (UNIX OR APPLE)

if (UNIX OR APPLE)
  option ( DATATOOLS_WITH_INSTALL_HERE
           "Installation path is stored in the datatools source tree" 
     	   OFF )
endif (UNIX OR APPLE)

#message ( STATUS "Default install prefix : " "${CMAKE_INSTALL_PREFIX}" )

if ( DATATOOLS_WITH_INSTALL_HERE )
  set (datatools_here_install_prefix 
       "${PROJECT_SOURCE_DIR}/.install_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" 
      )
  set ( CMAKE_INSTALL_PREFIX 
        "${datatools_here_install_prefix}" 
      )

  message ( STATUS "Please run 'cmake uninstall' to uninstall this build" )
  add_custom_target (uninstall cmake -E remove_directory ${datatools_here_install_prefix} )
endif ( DATATOOLS_WITH_INSTALL_HERE )

option ( DATATOOLS_WITH_EMBEDDED_KWSYS
         "Use an embedded kwsys library" 
     	 OFF )

message ( STATUS "DATATOOLS_WITH_DEBUG           : " "${DATATOOLS_WITH_DEBUG}" )
message ( STATUS "DATATOOLS_WITH_BIO             : " "${DATATOOLS_WITH_BIO}" )
message ( STATUS "DATATOOLS_WITH_PYTHON_WRAPPER  : " "${DATATOOLS_WITH_PYTHON_WRAPPER}" )
if (UNIX OR APPLE)
  message ( STATUS "DATATOOLS_WITH_CONFIG_SCRIPT   : " "${DATATOOLS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "DATATOOLS_WITH_PKGCONFIG       : " "${DATATOOLS_WITH_PKGCONFIG}" )
endif (UNIX OR APPLE)
message ( STATUS "DATATOOLS_WITH_DOC             : " "${DATATOOLS_WITH_DOC}" )
message ( STATUS "DATATOOLS_WITH_TEST            : " "${DATATOOLS_WITH_TEST}" )
message ( STATUS "DATATOOLS_WITH_EMBEDDED_KWSYS  : " "${DATATOOLS_WITH_EMBEDDED_KWSYS}" )
message ( STATUS "DATATOOLS_WITH_INSTALL_HERE    : " "${DATATOOLS_WITH_INSTALL_HERE}" )
message ( STATUS "datatools install prefix       : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------
# Embedded kwsys API
#

if ( DATATOOLS_WITH_EMBEDDED_KWSYS )

  #----------------------------------------------------------------------------
  # kwsys project
  # Downloaded from github.com
  set( _kwsys_version 0.1.0 )

  message( STATUS "Building the kwsys project version ${_kwsys_version}..." )

  list ( APPEND DATATOOLS_COMPONENTS "kwsys" )

  set (KWSYS_NAMESPACE kwsys)
  set (KWSYS_HEADER_ROOT ${PROJECT_BINARY_DIR})
  set (KWSYS_BUILD_SHARED 1)
  set (KWSYS_STANDALONE 1)
  set (KWSYS_INSTALL_BIN_DIR bin)
  set (KWSYS_INSTALL_LIB_DIR lib)
  set (KWSYS_INSTALL_INCLUDE_DIR include)
  set (KWSYS_INSTALL_DOC_DIR share/kwsys/doc)
  set (KWSYS_INSTALL_COMPONENT_NAME_RUNTIME Runtime)
  set (KWSYS_INSTALL_COMPONENT_NAME_DEVELOPMENT Development)

  set (KWSYS_USE_Base64 0)
  set (KWSYS_USE_Directory 0)
  set (KWSYS_USE_DynamicLoader 1)
  set (KWSYS_USE_Glob 0)
  set (KWSYS_USE_MD5 0)
  set (KWSYS_USE_Process 0)
  set (KWSYS_USE_RegularExpression 0)
  set (KWSYS_USE_Registry 0)
  set (KWSYS_USE_System 0)
  set (KWSYS_USE_SystemTools 0)
  set (KWSYS_USE_CommandLineArguments 0)
  set (KWSYS_USE_FundamentalType 0)
  set (KWSYS_USE_Terminal 0)
  set (KWSYS_USE_IOStream 0)
  set (KWSYS_USE_DateStamp 0)
  set (KWSYS_USE_String 0)
  set (KWSYS_USE_SystemInformation 0)
  set (KWSYS_USE_CPU 0)

  add_subdirectory ( kwsys )

 
endif (DATATOOLS_WITH_EMBEDDED_KWSYS)

# ----------------------------------------------------------------------
# datatools API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh
         DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set (_boost_libs "date_time filesystem iostreams program_options serialization system thread" )
set (_datatools_pkgconfig_Requires 
     "Requires: CLHEP >= ${_clhep_version},Boost/date_time >= ${_boost_version},Boost/iostreams,Boost/filesystem,Boost/program_options,Boost/serialization,Boost/system Boost/thread" 
)
if ( DATATOOLS_WITH_PYTHON_WRAPPER )
   set (_datatools_pkgconfig_Requires "${_datatools_pkgconfig_Requires},Boost/python" )
   set (_boost_libs "${_boost_libs} python" )
endif (DATATOOLS_WITH_PYTHON_WRAPPER)

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools.pc.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/datatools.pc" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/cmake/datatools.pc
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set (_datatools_pkgconfig_Libs "-ldatatools" )

set (_datatools_pkgconfig_Libs 
    ${_datatools_pkgconfig_Libs} 
    ${CLHEP_LIBRARIES} 
    ${Boost_LIBRARIES}  
    -lm 
    -lkwsys -lkwsys_c -ldl)

if (DATATOOLS_WITH_BIO)
  set (_datatools_pkgconfig_Libs -ldatatools_bio ${_datatools_pkgconfig_Libs})
endif (DATATOOLS_WITH_BIO)

if (UNIX OR APPLE)
  if ( DATATOOLS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/datatools-config" @ONLY )

    install (FILES ${PROJECT_BINARY_DIR}/cmake/datatools-config
             DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
             PERMISSIONS OWNER_READ OWNER_EXECUTE 
                         GROUP_READ GROUP_EXECUTE 
                         WORLD_READ WORLD_EXECUTE
             )

    install (FILES ${PROJECT_SOURCE_DIR}/pkgtools.d/mkuniqueflags.py
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

install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Modules/FindDatatools.cmake
          DESTINATION ${INSTALL_MISC_DIR}/cmake/Modules/
	  COMPONENT utils
         )



#----------------------------------------------------------------------------
# Add documentation.
#
if ( DATATOOLS_WITH_DOC )
  add_subdirectory (doc)
endif ()

#----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( DATATOOLS_WITH_TEST )
  add_subdirectory (tests)
endif ()

###############################
# packaging
###############################

include ( ${DATATOOLS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

message ( STATUS "INSTALL_BIN_DIR     : ${INSTALL_BIN_DIR}" )
message ( STATUS "INSTALL_INCLUDE_DIR : ${INSTALL_INCLUDE_DIR}" )
message ( STATUS "INSTALL_LIB_DIR     : ${INSTALL_LIB_DIR}" )
message ( STATUS "INSTALL_MISC_DIR    : ${INSTALL_MISC_DIR}" )
message ( STATUS "DATATOOLS_BIN_DIR   : ${DATATOOLS_BIN_DIR}" )

# - end of top level CMakeLists.txt for datatools project.
