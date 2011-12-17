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
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "datatools requires an out-of-source build." )
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
project ( datatools )

set ( datatools_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${datatools_PACKAGE_NAME}'" )

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

### Depends on Boost:
set ( _boost_version 1.47.0 )
if ( NOT "$ENV{BOOST_ROOT}" STREQUAL "")  
  message ( STATUS "Environment BOOST_ROOT exists : '$ENV{BOOST_ROOT}' ")
  set ( BOOST_ROOT $ENV{BOOST_ROOT} )
else ()
  message ( WARNING  "Environment BOOST_ROOT is not set !" )
endif ()

string ( REGEX REPLACE ".[0-9]+$" "" _boost_short_version "${_boost_version}" )
message ( STATUS "Boost short version is : '${_boost_short_version}'" )
set ( Boost_ADDITIONAL_VERSIONS "${_boost_short_version}" "${_boost_version}" )
set ( Boost_NO_SYSTEM_PATHS ON )
set ( _boost_requested_components filesystem serialization iostreams system )

if ( DATATOOLS_WITH_PYTHON_WRAPPER )
  set ( _boost_requested_components ${_boost_requested_components} python )
endif ()

find_package ( Boost ${_boost_version} REQUIRED COMPONENTS ${_boost_requested_components} )
if ( Boost_FOUND )
     message ( STATUS "Found Boost ${_boost_version}")
     message ( STATUS "  Boost_VERSION          = ${Boost_VERSION}" )
     message ( STATUS "  Boost_LIB_VERSION      = ${Boost_LIB_VERSION}" )
     message ( STATUS "  Boost_INCLUDE_DIRS     = ${Boost_INCLUDE_DIRS}" )
     message ( STATUS "  Boost_LIBRARIES        = ${Boost_LIBRARIES}" )
     include_directories ( ${Boost_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find Boost ${_boost_version} library !" )
endif ()

### Depends on CLHEP:
set ( _clhep_version 2.1.0.1 )
find_package ( CLHEP ${_clhep_do_version} REQUIRED )
if ( CLHEP_FOUND )
     message ( STATUS "Found CLHEP ${_clhep_version}" )
     include_directories ( ${CLHEP_INCLUDE_DIRS} )
     message ( STATUS "CLHEP_LIBRARIES : ${CLHEP_LIBRARIES}" )
else ()
     message ( FATAL_ERROR "Cannot find CLHEP ${_clhep_version} library !" )
endif ()

### Depends on GSL:
# set ( _gsl_version 1.14 )
# find_package ( GSL ${_gsl_version} REQUIRED )
# if ( GSL_FOUND )
#      message ( STATUS "Found GSL ${_gsl_version}" )
#      include_directories ( ${GSL_INCLUDE_DIRS} )
# else ()
#      message ( FATAL_ERROR "Cannot find GSL ${_gsl_version} library !" )
# endif ()

### Depends on XercesC:
# set ( _xercesc_version 3.1.1 )
# find_package ( XercesC ${_xercesc_version} REQUIRED )
# if ( XercesC_FOUND )
#   message ( STATUS "Found XercesC ${_xercesc_version}" )
#   include_directories ( ${XercesC_INCLUDE_DIRS} )
# else ()
#   message ( FATAL_ERROR "Cannot find XercesC ${_xercesc_version} library !" )
# endif ()

# ----------------------------------------------------------------------
# Options :
#

#option ( DATATOOLS_WITH_EMBEDDED_KWSYS
#         "Use an embedded kwsys library" 
#     	 OFF )
set ( DATATOOLS_WITH_EMBEDDED_KWSYS OFF )

### Depends on kwsys:
if ( NOT DATATOOLS_WITH_EMBEDDED_KWSYS )
  set ( _kwsys_version 0.1.0 )
  find_package ( KWSYS ${_kwsys_version} REQUIRED )
  if ( KWSYS_FOUND )
     message ( STATUS "Found KWSYS ${_kwsys_version}" )
     include_directories ( ${KWSYS_INCLUDE_DIRS} )
     message ( STATUS "KWSYS_LIBRARIES : ${KWSYS_LIBRARIES}" )
  else ()
     message ( FATAL_ERROR "Cannot find kwsys ${_kwsys_version} library !" )
  endif ()
endif ()

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
 	 ON )

if ( UNIX OR APPLE )
  option ( DATATOOLS_WITH_CONFIG_SCRIPT 
           "Build the datatools-config script" 
     	   ON )
  option ( DATATOOLS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( DATATOOLS_WITH_CMAKE_FIND_MODULE 
         "Build the datatools' CMake find module"
 	 ON )

if ( UNIX OR APPLE )
  option ( DATATOOLS_WITH_INSTALL_HERE
           "Installation path is stored in the datatools source tree" 
     	   OFF )

  message ( STATUS "Default install prefix : " "${CMAKE_INSTALL_PREFIX}" )

  if ( DATATOOLS_WITH_INSTALL_HERE )
    message ( STATUS "DEBUG: DATATOOLS_WITH_INSTALL_HERE" )
    set ( _datatools_here_install_prefix 
          "${PROJECT_SOURCE_DIR}/.install_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
         )
    set ( CMAKE_INSTALL_PREFIX 
          "${_datatools_here_install_prefix}" 
  	  CACHE PATH "datatools installation prefix" FORCE
        )
    message ( STATUS "Please run 'cmake uninstall' to uninstall this build" )
    add_custom_target (uninstall cmake -E remove_directory ${_datatools_here_install_prefix} )
  endif ( DATATOOLS_WITH_INSTALL_HERE )
endif ()

message ( STATUS "DATATOOLS_WITH_DEBUG           : " "${DATATOOLS_WITH_DEBUG}" )
message ( STATUS "DATATOOLS_WITH_BIO             : " "${DATATOOLS_WITH_BIO}" )
message ( STATUS "DATATOOLS_WITH_PYTHON_WRAPPER  : " "${DATATOOLS_WITH_PYTHON_WRAPPER}" )
if (UNIX OR APPLE)
  message ( STATUS "DATATOOLS_WITH_CONFIG_SCRIPT   : " "${DATATOOLS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "DATATOOLS_WITH_PKGCONFIG       : " "${DATATOOLS_WITH_PKGCONFIG}" )
  message ( STATUS "DATATOOLS_WITH_INSTALL_HERE    : " "${DATATOOLS_WITH_INSTALL_HERE}" )
endif (UNIX OR APPLE)
message ( STATUS "DATATOOLS_WITH_DOC             : " "${DATATOOLS_WITH_DOC}" )
message ( STATUS "DATATOOLS_WITH_TEST            : " "${DATATOOLS_WITH_TEST}" )
message ( STATUS "DATATOOLS_WITH_EMBEDDED_KWSYS  : " "${DATATOOLS_WITH_EMBEDDED_KWSYS}" )
message ( STATUS "CMAKE_INSTALL_PREFIX           : " "${CMAKE_INSTALL_PREFIX}" )
message ( STATUS "CMAKE_INSTALL_PYTHONDIR        : " "${CMAKE_INSTALL_PYTHONDIR}" )

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
  set (KWSYS_BUILD_SHARED ON)
  set (KWSYS_STANDALONE OFF)
  set (KWSYS_INSTALL_BIN_DIR bin)
  set (KWSYS_INSTALL_LIB_DIR lib)
  set (KWSYS_INSTALL_INCLUDE_DIR include)
  set (KWSYS_INSTALL_DOC_DIR share/kwsys/doc)
  set (KWSYS_INSTALL_COMPONENT_NAME_RUNTIME Runtime)
  set (KWSYS_INSTALL_COMPONENT_NAME_DEVELOPMENT Development)

  set (KWSYS_USE_Base64 1)
  set (KWSYS_USE_Directory 1)
  set (KWSYS_USE_DynamicLoader 1)
  set (KWSYS_USE_Glob 1)
  set (KWSYS_USE_MD5 1)
  set (KWSYS_USE_Process 1)
  set (KWSYS_USE_RegularExpression 1)
  set (KWSYS_USE_Registry 1)
  set (KWSYS_USE_System 1)
  set (KWSYS_USE_SystemTools 1)
  set (KWSYS_USE_CommandLineArguments 1)
  set (KWSYS_USE_FundamentalType 1)
  set (KWSYS_USE_Terminal 1)
  set (KWSYS_USE_IOStream 1)
  set (KWSYS_USE_DateStamp 1)
  set (KWSYS_USE_String 1)
  set (KWSYS_USE_SystemInformation 1)
  set (KWSYS_USE_CPU 1)

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

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
message ( STATUS "************ ${DATATOOLS_CMAKE_MODULE_INSTALL_DIR}" )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _boost_libs "date_time filesystem iostreams program_options serialization system thread" )
set ( _datatools_pkgconfig_Requires 
     "Requires: CLHEP >= ${_clhep_version},Boost/date_time >= ${_boost_version},Boost/iostreams,Boost/filesystem,Boost/program_options,Boost/serialization,Boost/system Boost/thread" 
    )
if ( DATATOOLS_WITH_PYTHON_WRAPPER )
   set ( _datatools_pkgconfig_Requires "${_datatools_pkgconfig_Requires},Boost/python" )
   set ( _boost_libs "${_boost_libs} python" )
endif ( DATATOOLS_WITH_PYTHON_WRAPPER )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools.pc.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/datatools.pc" @ONLY 
               )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/datatools.pc
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _datatools_pkgconfig_Libs "-ldatatools" )

set ( _datatools_pkgconfig_Libs 
    ${_datatools_pkgconfig_Libs} 
    ${CLHEP_LIBRARIES} 
    ${Boost_LIBRARIES}  
    -lm 
    -lkwsys -lkwsys_c -ldl )

if ( DATATOOLS_WITH_BIO )
  set ( _datatools_pkgconfig_Libs -ldatatools_bio ${_datatools_pkgconfig_Libs} )
endif ()

if ( DATATOOLS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindDatatools.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindDatatools.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindDatatools.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

  set ( DATATOOLS_CMAKE_MODULE_INSTALL_DIR ${_find_module_install_dir} 
        CACHE PATH "The CMake Module subdirectory" FORCE )

  message ( STATUS "DATATOOLS_CMAKE_MODULE_INSTALL_DIR = '${DATATOOLS_CMAKE_MODULE_INSTALL_DIR}' " )
endif ()

if ( UNIX OR APPLE )
  if ( DATATOOLS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/datatools-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/datatools-config
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
# Add Python wrapping.
#
if ( DATATOOLS_WITH_PYTHON_WRAPPER )
  set ( CMAKE_USE_PYTHON_VERSION "2.7" "2.6" )
  find_package ( PythonInterp REQUIRED )
  set ( CMAKE_USE_PYTHON_VERSION "2.7" "2.6" )
  find_package ( PythonLibs REQUIRED )

  # We need to force the cache to disable Python if we didn't find any
  # of the needed components
  message ( STATUS "Boost_PYTHON_LIBRARY = ${Boost_PYTHON_LIBRARY}" )
  message ( STATUS "PYTHON_INCLUDE_DIRS  = ${PYTHON_INCLUDE_DIRS}" )
  message ( STATUS "PYTHON_EXECUTABLE    = ${PYTHON_EXECUTABLE}" )
  message ( STATUS "PYTHON_LIBRARIES     = ${PYTHON_LIBRARIES}" )
  message ( STATUS "PYTHON_DEBUG_LIBRARIES = ${PYTHON_DEBUG_LIBRARIES}" )

  if (NOT Boost_PYTHON_LIBRARY OR NOT PYTHON_INCLUDE_DIRS OR NOT PYTHON_EXECUTABLE)
     message ( STATUS "Missing Python stuff " )
     set ( DATATOOLS_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of datatools, requires Boost.Python" FORCE)
  endif ()
endif () 
  
set ( CMAKE_INSTALL_PYTHONDIR "" )
message ( STATUS "CMAKE_INSTALL_PYTHONDIR = '${CMAKE_INSTALL_PYTHONDIR}' " )

if ( DATATOOLS_WITH_PYTHON_WRAPPER )
    message ( STATUS "Add subdirectory 'wrapping/python' " )
    add_subdirectory ( wrapping/python )
endif () 

#----------------------------------------------------------------------------
# Add documentation.
#
if ( DATATOOLS_WITH_DOC )
  add_subdirectory ( doc )
endif ()

#----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( DATATOOLS_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

###############################
# packaging
###############################

include ( ${DATATOOLS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for datatools project.
