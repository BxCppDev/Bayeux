# - Top level CMakeLists.txt for cuts project
# -*- mode: cmake; -*-
#
# ----------------------------------------------------------------------------
# Copyright (c) 2012, François Mauger <mauger@lpccaen.in2p3.fr>
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
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

# ----------------------------------------------------------------------------
# Setup CMake requirements and project's name
#
cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( cuts )

set ( cuts_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${cuts_PACKAGE_NAME}'" )

#----------------------------------------------------------------------------
# Prepend this project's custom module path(s) to CMAKE_MODULE_PATH
# Append any path from the environment
# This allows us to customize, add, and factor out functionality
# NB: if our custom modules include others that we don't supply, those in the
# base path will be used, so watch for incompatibilities!!
#
file(TO_CMAKE_PATH "$ENV{CMAKE_MODULE_PATH}" __env_cmake_module_path)
	
set(CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
  ${PROJECT_SOURCE_DIR}/cmake/Modules
  ${__env_cmake_module_path}
  ${CMAKE_MODULE_PATH}
)
	
#----------------------------------------------------------------------------
# Load any required modules
#
include ( Config )
include ( InstallDirs )
include ( PackageFilename )

# ----------------------------------------------------------------------
# Dependencies :
#

# ----------------------------------------------------------------------
# Options :
#

option ( CUTS_WITH_DEBUG
         "Build the cuts project using debugging code"
         0 )

option ( CUTS_WITH_DOC 
         "Build the cuts documentation" 
 	 1 )

option ( CUTS_WITH_TEST 
         "Build the cuts test material" 
 	 1 )

set ( CUTS_WITH_BIO_SUPPORT 0 )

set ( CUTS_WITH_PYTHON_WRAPPER_SUPPORT 0 )

option ( CUTS_WITH_CMAKE_FIND_MODULE 
         "Build the cuts' CMake find module"
 	 0 )

# set ( _default_bio_activation 0 )
# set ( _default_python_wrapper_activation 0 )
  

# - Depends on datatools:

  if ( DEFINED datatools_DIR )
    message ( STATUS "datatools_DIR is : '${datatools_DIR}' " )
  else ()
    if ( NOT "$ENV{DATATOOLS_CMAKE_CONFIG_DIR}" STREQUAL "" )
      message ( STATUS "Environment DATATOOLS_CMAKE_CONFIG_DIR exists : '$ENV{DATATOOLS_CMAKE_CONFIG_DIR}' " )
      set ( datatools_DIR "$ENV{DATATOOLS_CMAKE_CONFIG_DIR}" )
    endif ()
  endif ()
  set ( _datatools_min_version 4.0 )
  find_package ( datatools ${_datatools_min_version} REQUIRED NO_MODULE )
  if ( DEFINED datatools_CONFIG )
       message ( STATUS "Found datatools >= ${_datatools_min_version}" )
       message ( STATUS "  datatools_CONFIG              : ${datatools_CONFIG}" )
       message ( STATUS "  datatools_VERSION             : ${datatools_VERSION}" )
       message ( STATUS "  datatools_LIBRARIES           : ${datatools_LIBRARIES}" )
       message ( STATUS "  datatools_INCLUDE_DIRS        : ${datatools_INCLUDE_DIRS}" )
       message ( STATUS "  datatools_WITH_DEBUG          : ${datatools_WITH_DEBUG}" )
       # message ( STATUS "  datatools_WITH_BIO            : ${datatools_WITH_BIO}" )
       # message ( STATUS "  datatools_WITH_PYTHON_WRAPPER : ${datatools_WITH_PYTHON_WRAPPER}" )
  else ()
       message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
  endif ()
  # if ( CUTS_WITH_BIO_SUPPORT )
  #   if ( datatools_WITH_BIO )
  #     set ( _default_bio_activation 1 )
  #   endif ()
  # endif ()
  # if ( CUTS_WITH_PYTHON_WRAPPER_SUPPORT )
  #   if ( datatools_WITH_PYTHON_WRAPPER )
  #     set ( _default_python_wrapper_activation 1 )
  #   endif ()
  # endif ()

# if ( CUTS_WITH_BIO_SUPPORT )
#   option ( CUTS_WITH_BIO 
#            "Build the cuts with Boost I/O serialization" 
#       	   ${_default_bio_activation} )
# endif ()

# if ( CUTS_WITH_PYTHON_WRAPPER_SUPPORT )
#   option ( CUTS_WITH_PYTHON_WRAPPER
#            "Build the cuts Boost/Python wrapper module" 
#            ${_default_python_wrapper_activation} )
# endif ()

# ---------------------------------------------------

if (CUTS_WITH_DEBUG)
  set (CUTS_WITH_DEBUG 1)
endif ()

# if ( CUTS_WITH_BIO_SUPPORT AND CUTS_WITH_BIO )
#   set (CUTS_WITH_BIO 1)
# endif ()

# if ( CUTS_WITH_PYTHON_WRAPPER_SUPPORT AND CUTS_WITH_PYTHON_WRAPPER )
#   set (CUTS_WITH_PYTHON_WRAPPER 1)
# endif ()

if (CUTS_WITH_DOC)
  set (CUTS_WITH_DOC 1)
endif ()

if (CUTS_WITH_TEST)
  set (CUTS_WITH_TEST 1)
endif ()
# ---------------------------------------------------

if ( UNIX OR APPLE )
  option ( CUTS_WITH_CONFIG_SCRIPT 
           "Build the cuts-config script" 
     	   1 )
  option ( CUTS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   0 )
endif ()
# ---------------------------------------------------

option ( CUTS_WITH_CMAKE_FIND_MODULE 
         "Build the cuts CMake find module"
 	 OFF )

# ----------------------------------------------------------------------

message ( STATUS "CUTS_WITH_DEBUG           : " "${CUTS_WITH_DEBUG}" )
# if ( CUTS_WITH_BIO_SUPPORT )
#   message ( STATUS "CUTS_WITH_BIO             : " "${CUTS_WITH_BIO}" )
# endif ()
# if ( CUTS_WITH_PYTHON_WRAPPER_SUPPORT )
#   message ( STATUS "CUTS_WITH_PYTHON_WRAPPER  : " "${CUTS_WITH_PYTHON_WRAPPER}" )
# endif ()

if ( UNIX OR APPLE )
  message ( STATUS "CUTS_WITH_CONFIG_SCRIPT   : " "${CUTS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "CUTS_WITH_PKGCONFIG       : " "${CUTS_WITH_PKGCONFIG}" )
endif ()
message ( STATUS "CUTS_WITH_DOC             : " "${CUTS_WITH_DOC}" )
message ( STATUS "CUTS_WITH_TEST            : " "${CUTS_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX    : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# ----------------------------------------------------------------------
# cuts API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

# - CMake Config file:
set ( cuts_cmake_config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} )

# - Pkgconfig file:
if ( CUTS_WITH_PKGCONFIG )
  set ( _pkgconfig_install_dir ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
  if ( DEFINED CUTS_PKGCONFIG_INSTALL_DIR)
    set ( _pkgconfig_install_dir ${CUTS_PKGCONFIG_INSTALL_DIR} )
  endif ()
endif ()

# - CMake Module file (Find) :
if ( CUTS_WITH_CMAKE_FIND_MODULE )
  set ( cuts_cmake_find_module_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/Modules ) 
  if ( DEFINED CUTS_CMAKE_FIND_MODULE_INSTALL_DIR )
    set ( cuts_cmake_find_module_install_dir ${CUTS_CMAKE_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "CUTS_CMAKE_FIND_MODULE_INSTALL_DIR : " "${CUTS_CMAKE_FIND_MODULE_INSTALL_DIR}" )
  endif ()
  message ( STATUS "cuts CMake find module install dir = '${cuts_cmake_find_module_install_dir}'" )
endif ()

# - C++ configuration header file :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cuts_config.h.in"
                 "${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cuts
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Bash setup script :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cuts_setup.sh.in"
                 "${PROJECT_BINARY_DIR}/cmake/cuts_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/cuts_setup.sh
          DESTINATION ${CMAKE_INSTALL_ETCDIR}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Pkgconfig configuration file :
if ( CUTS_WITH_PKGCONFIG )

  set ( _cuts_pkgconfig_Requires "Requires: " )

  set ( _cuts_pkgconfig_Requires "${_cuts_pkgconfig_Requires}datatools >= ${_datatools_min_version}" )

  # if ( _depends_on_YYY )
  #   set ( _cuts_pkgconfig_Requires "${_cuts_pkgconfig_Requires}YYY >= ${_YYY_min_version}" )
  # endif ()

  set ( _cuts_pkgconfig_Libs "-lcuts" )

  set ( _cuts_pkgconfig_Cflags 
        ${DATATOOLS_INCLUDE_DIRS} 
      )
 
  set ( _cuts_pkgconfig_Libs 
        ${_cuts_pkgconfig_Libs} 
        ${DATATOOLS_LIBRARIES} 
      ) 

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cuts.pc.in"
                   "${PROJECT_BINARY_DIR}/cmake/cuts.pc" @ONLY 
                 )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/cuts.pc
            DESTINATION ${_pkgconfig_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
          )
endif ()

if ( CUTS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindCuts.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindCuts.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindCuts.cmake
            DESTINATION ${_cmake_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( CUTS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cuts-config.in"
                     "${PROJECT_BINARY_DIR}/cmake/cuts-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/cuts-config
              DESTINATION ${CMAKE_INSTALL_BINDIR}
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
                          GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
             )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/mkuniqueflags.py
              DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgtools
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
	                  GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
            )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/libpkgtools.sh
              DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgtools/
              PERMISSIONS OWNER_READ OWNER_WRITE 
                          GROUP_READ
                          WORLD_READ
             )

  endif ()
endif ()  

install ( FILES README.txt COPYING.txt LICENSE.GPL3.txt
          DESTINATION ${CMAKE_INSTALL_DATADIR}
         )

# ----------------------------------------------------------------------------
# - Add Python wrapping.
#

if ( CUTS_WITH_PYTHON_WRAPPER_SUPPORT AND CUTS_WITH_PYTHON_WRAPPER )
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
     set ( CUTS_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of __PackName_, requires Boost.Python" FORCE)
  endif ()
endif () 
  
#set ( CMAKE_INSTALL_PYTHONDIR "" )
#message ( STATUS "CMAKE_INSTALL_PYTHONDIR = '${CMAKE_INSTALL_PYTHONDIR}' " )

if ( CUTS_WITH_PYTHON_WRAPPER )
    message ( STATUS "Add subdirectory 'wrapping/python' " )
    add_subdirectory ( wrapping/python )
endif () 

# 
# ----------------------------------------------------------------------------
# Add documentation.
#
if ( CUTS_WITH_DOC )
  add_subdirectory ( doc )
endif ()

# ----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( CUTS_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

# ----------------------------------------------------------------------------
# packaging
#

include ( ${CUTS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# ----------------------------------------------------------------------------
# - Generate Build Tree configuration files
# - CMake
# Export targets from the build tree
# Done by hand for now because we know the targets exactly
export(TARGETS cuts 
  FILE ${PROJECT_BINARY_DIR}/cuts-libraries.cmake
  )
if ( CUTS_WITH_BIO_SUPPORT AND CUTS_WITH_BIO )
  export(TARGETS cuts_bio 
    APPEND FILE ${PROJECT_BINARY_DIR}/cuts-libraries.cmake
    )
endif()

# Configure the build tree CMake Config mode file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/cuts-config.cmake.in
  ${PROJECT_BINARY_DIR}/cuts-config.cmake
  @ONLY
  )

# Configure the build tree versioning file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/cuts-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/cuts-config-version.cmake
  @ONLY
  )

#----------------------------------------------------------------------------
# - Generate Install Tree configuration files
# - CMake
# Export targets from the install tree
# Just need to install the named export to a suitable location
if(WIN32)
  set(CUTS_CMAKE_CONFIG_DIR cuts-${cuts_VERSION_STR} )
else()
  if(CUTS_IS_FRAMEWORK)
    # Mac Framework location - may not be needed.
  else()
    # Standard Unix Tool
    set(CUTS_CMAKE_CONFIG_DIR ${cuts_cmake_config_install_dir} )
  endif()
endif()

install(EXPORT cuts-libraries
  DESTINATION ${CUTS_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )

# Configure the install tree CMake Config mode file
# Set variables
set(cuts_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(cuts_LIB_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
# Configure file
# This is different from the build tree file, so store in dedicated directory
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/cuts-config.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/cuts-config.cmake
  @ONLY
  )


# Configure the install tree versioning file
# N.B. it *probably* does not differ from the build tree file, but configuring
# separately allows easy changes should we need this.
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/cuts-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/cuts-config-version.cmake
  @ONLY
  )

# Install the CMake Config mode files into the install tree
install ( FILES
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/cuts-config.cmake
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/cuts-config-version.cmake
          DESTINATION ${CUTS_CMAKE_CONFIG_DIR}
          COMPONENT devel
        )

# - end of top level CMakeLists.txt for cuts project.
