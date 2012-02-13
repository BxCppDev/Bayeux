# - Top level CMakeLists.txt for mygsl project
# -*- mode: cmake; -*-
#
# ----------------------------------------------------------------------------
# Copyright (c) 2012, François Mauger <mauger@lpccaen.in2p3.fr>
#                     Ben Morgan <Ben.Morgan@warwick.ac.uk>
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
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

# ----------------------------------------------------------------------------
# Setup CMake requirements and project's name
#
cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( mygsl )

set ( mygsl_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${mygsl_PACKAGE_NAME}'" )
	
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

if (DEFINED ADDITIONAL_CMAKE_MODULE_PATH AND NOT "x${ADDITIONAL_CMAKE_MODULE_PATH}" STREQUAL "x" )
  message ( STATUS "mygsl: Using additional CMAKE_MODULE_PATH : '${ADDITIONAL_CMAKE_MODULE_PATH}'" )
  set(CMAKE_MODULE_PATH
      ${ADDITIONAL_CMAKE_MODULE_PATH}
      ${CMAKE_MODULE_PATH}
  )
endif ()

message ( STATUS "mygsl: CMAKE_MODULE_PATH         : '${CMAKE_MODULE_PATH}'" )
	
#----------------------------------------------------------------------------
# Load any required modules
#
include ( Config )
include ( InstallDirs )
include ( PackageFilename )

# ----------------------------------------------------------------------
# Options :
#

option ( MYGSL_WITH_DEBUG
         "Build the mygsl project using debugging code"
         0 )

option ( MYGSL_WITH_DOC 
         "Build the mygsl documentation" 
 	 1 )

option ( MYGSL_WITH_TEST 
         "Build the mygsl test material" 
 	 1 )

set ( MYGSL_WITH_BIO_SUPPORT 0 )

set ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT 0 )

option ( MYGSL_WITH_CMAKE_FIND_MODULE 
         "Build the mygsl' CMake find module"
 	 0 )

# ----------------------------------------------------------------------
# Dependencies :
#

# - Depends on GSL:
set ( _gsl_min_version 1.14 ) 
set ( GSL_MIN_VERSION ${_gsl_min_version} )
if ( NOT "$ENV{GSL_ROOT_DIR}" STREQUAL "")  
  message ( STATUS "Environment GSL_ROOT_DIR exists : '$ENV{GSL_ROOT_DIR}' ")
  set ( GSL_ROOT_DIR $ENV{GSL_ROOT_DIR} )
else ()
  message ( WARNING  "Environment GSL_ROOT_DIR is not set !" )
endif ()

find_package ( GSL ${GSL_MIN_VERSION} )
if ( GSL_FOUND )
     message ( STATUS "Found GSL")
     message ( STATUS " o GSL_VERSION           : ${GSL_VERSION}" )
     message ( STATUS " o GSL_CONFIG_EXECUTABLE : ${GSL_CONFIG_EXECUTABLE}" )
     message ( STATUS " o GSL_INCLUDE_DIRS      : ${GSL_INCLUDE_DIRS}" )
     message ( STATUS " o GSL_LIBRARIES         : ${GSL_LIBRARIES}" )
     include_directories ( ${GSL_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find GSL >= ${GSL_MIN_VERSION} library !" )
endif ()

set ( _depends_on_datatools 1 )

if ( MYGSL_WITH_BIO_SUPPORT )
  set ( _depends_on_datatools 1 )
endif ()

if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT )
  set ( _depends_on_datatools 1 )
endif ()

set ( _default_bio_activation 0 )
set ( _default_python_wrapper_activation 0 )

if ( _depends_on_datatools )
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
  if ( datatools_CONFIG )
    message ( STATUS "Found datatools >= ${_datatools_min_version}" )
    message ( STATUS " o datatools_CONFIG              : ${datatools_CONFIG}" )
    message ( STATUS " o datatools_VERSION             : ${datatools_VERSION}" )
    message ( STATUS " o datatools_LIBRARIES           : ${datatools_LIBRARIES}" )
    message ( STATUS " o datatools_INCLUDE_DIRS        : ${datatools_INCLUDE_DIRS}" )
    message ( STATUS " o datatools_WITH_DEBUG          : ${datatools_WITH_DEBUG}" )
    message ( STATUS " o datatools_WITH_BIO            : ${datatools_WITH_BIO}" )
    message ( STATUS " o datatools_WITH_PYTHON_WRAPPER : ${datatools_WITH_PYTHON_WRAPPER}" )
  else ()
    message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
  endif ()
  
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
       message ( STATUS "  datatools_WITH_BIO            : ${datatools_WITH_BIO}" )
       message ( STATUS "  datatools_WITH_PYTHON_WRAPPER : ${datatools_WITH_PYTHON_WRAPPER}" )
  else ()
       message ( FATAL_ERROR "Cannot find datatools >= ${_datatools_min_version} library !" )
  endif ()
  # if ( MYGSL_WITH_BIO_SUPPORT )
  #   if ( datatools_WITH_BIO )
  #     set ( _default_bio_activation 1 )
  #   endif ()
  # endif ()
  # if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT )
  #   if ( datatools_WITH_PYTHON_WRAPPER )
  #     set ( _default_python_wrapper_activation 1 )
  #   endif ()
  # endif ()
endif ()

# if ( MYGSL_WITH_BIO_SUPPORT )
#   option ( MYGSL_WITH_BIO 
#            "Build the mygsl with Boost I/O serialization" 
#       	   ${_default_bio_activation} )
# endif ()


# if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT )
#   option ( MYGSL_WITH_PYTHON_WRAPPER
#            "Build the mygsl Boost/Python wrapper module" 
#            ${_default_python_wrapper_activation} )
# endif ()

# ---------------------------------------------------

if (MYGSL_WITH_DEBUG)
  set (MYGSL_WITH_DEBUG 1)
endif ()

if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT AND MYGSL_WITH_BIO )
  set (MYGSL_WITH_BIO 1)
endif ()

if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT AND MYGSL_WITH_PYTHON_WRAPPER )
  set (MYGSL_WITH_PYTHON_WRAPPER 1)
endif ()

if (MYGSL_WITH_DOC)
  set (MYGSL_WITH_DOC 1)
endif ()

if (MYGSL_WITH_TEST)
  set (MYGSL_WITH_TEST 1)
endif ()
# ---------------------------------------------------

if ( UNIX OR APPLE )
  option ( MYGSL_WITH_CONFIG_SCRIPT 
           "Build the mygsl-config script" 
     	   1 )
  option ( MYGSL_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   0 )
endif ()
# ---------------------------------------------------

option ( MYGSL_WITH_CMAKE_FIND_MODULE 
         "Build the datatools' CMake find module"
 	 OFF )

# ----------------------------------------------------------------------

message ( STATUS "MYGSL_WITH_DEBUG           : " "${MYGSL_WITH_DEBUG}" )
if ( MYGSL_WITH_BIO_SUPPORT )
  message ( STATUS "MYGSL_WITH_BIO             : " "${MYGSL_WITH_BIO}" )
endif ()
if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT )
  message ( STATUS "MYGSL_WITH_PYTHON_WRAPPER  : " "${MYGSL_WITH_PYTHON_WRAPPER}" )
endif ()
if ( UNIX OR APPLE )
  message ( STATUS "MYGSL_WITH_CONFIG_SCRIPT   : " "${MYGSL_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "MYGSL_WITH_PKGCONFIG       : " "${MYGSL_WITH_PKGCONFIG}" )
endif ()
message ( STATUS "MYGSL_WITH_DOC             : " "${MYGSL_WITH_DOC}" )
message ( STATUS "MYGSL_WITH_TEST            : " "${MYGSL_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX    : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# ----------------------------------------------------------------------
# mygsl API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

# - CMake Config file:
set ( mygsl_cmake_config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} )

# - Pkgconfig file:
if ( MYGSL_WITH_PKGCONFIG )
  set ( _pkgconfig_install_dir ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
  if ( DEFINED MYGSL_PKGCONFIG_INSTALL_DIR)
    set ( _pkgconfig_install_dir ${MYGSL_PKGCONFIG_INSTALL_DIR} )
  endif ()
endif ()

# - CMake Module file (Find) :
if ( MYGSL_WITH_CMAKE_FIND_MODULE )
  set ( mygsl_cmake_find_module_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/Modules ) 
  if ( DEFINED MYGSL_CMAKE_FIND_MODULE_INSTALL_DIR )
    set ( mygsl_cmake_find_module_install_dir ${MYGSL_CMAKE_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "MYGSL_CMAKE_FIND_MODULE_INSTALL_DIR : " "${MYGSL_CMAKE_FIND_MODULE_INSTALL_DIR}" )
  endif ()
  message ( STATUS "datatools CMake find module install dir = '${mygsl_cmake_find_module_install_dir}'" )
endif ()

# - C++ configuration header file :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl_config.h.in"
                 "${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Bash setup script :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl_setup.sh.in"
                 "${PROJECT_BINARY_DIR}/etc/mygsl_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/etc/mygsl_setup.sh
          DESTINATION ${CMAKE_INSTALL_ETCDIR}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Pkgconfig configuration file :
if ( MYGSL_WITH_PKGCONFIG )

  set ( _mygsl_pkgconfig_Requires "Requires: " )

  if ( _depends_on_datatools )
    set ( _mygsl_pkgconfig_Requires "${_mygsl_pkgconfig_Requires}datatools >= ${_datatools_min_version}" )
  endif ()

  # if ( _depends_on_YYY )
  #   set ( _mygsl_pkgconfig_Requires "${_mygsl_pkgconfig_Requires}YYY >= ${_YYY_min_version}" )
  # endif ()

  set ( _mygsl_pkgconfig_Libs "-lmygsl" )

  if ( _depends_on_datatools )

    set ( _mygsl_pkgconfig_Cflags 
	  ${DATATOOLS_INCLUDE_DIRS} 
        )
 
    set ( _mygsl_pkgconfig_Libs 
          ${_mygsl_pkgconfig_Libs} 
          ${DATATOOLS_LIBRARIES} 
        ) 

  endif ()

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl.pc.in"
                   "${PROJECT_BINARY_DIR}/cmake/mygsl.pc" @ONLY 
                 )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/mygsl.pc
            DESTINATION ${_pkgconfig_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
          )
endif ()

if ( MYGSL_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindMygsl.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindMygsl.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindMygsl.cmake
            DESTINATION ${_cmake_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( MYGSL_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config.in"
                     "${PROJECT_BINARY_DIR}/cmake/mygsl-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/mygsl-config
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

if ( MYGSL_WITH_PYTHON_WRAPPER )
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
     set ( MYGSL_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of datatools, requires Boost.Python" FORCE)
  endif ()
endif () 
  
#set ( CMAKE_INSTALL_PYTHONDIR "" )
#message ( STATUS "CMAKE_INSTALL_PYTHONDIR = '${CMAKE_INSTALL_PYTHONDIR}' " )

if ( MYGSL_WITH_PYTHON_WRAPPER )
    message ( STATUS "Add subdirectory 'wrapping/python' " )
    add_subdirectory ( wrapping/python )
endif () 

# 
# ----------------------------------------------------------------------------
# Add documentation.
#
if ( MYGSL_WITH_DOC )
  add_subdirectory ( doc )
endif ()

# ----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( MYGSL_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

# ----------------------------------------------------------------------------
# packaging
#

include ( ${MYGSL_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# ----------------------------------------------------------------------------
# - Generate Build Tree configuration files
# - CMake
# Export targets from the build tree
# Done by hand for now because we know the targets exactly
export(TARGETS mygsl 
  FILE ${PROJECT_BINARY_DIR}/mygsl-libraries.cmake
  )
if ( MYGSL_WITH_BIO_SUPPORT AND MYGSL_WITH_BIO )
  export(TARGETS mygsl_bio 
    APPEND FILE ${PROJECT_BINARY_DIR}/mygsl-libraries.cmake
    )
endif()

# Configure the build tree CMake Config mode file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config.cmake.in
  ${PROJECT_BINARY_DIR}/mygsl-config.cmake
  @ONLY
  )

# Configure the build tree versioning file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/mygsl-config-version.cmake
  @ONLY
  )

#----------------------------------------------------------------------------
# - Generate Install Tree configuration files
# - CMake
# Export targets from the install tree
# Just need to install the named export to a suitable location
if(WIN32)
  set(MYGSL_CMAKE_CONFIG_DIR mygsl-${mygsl_VERSION_STR} )
else()
  if(MYGSL_IS_FRAMEWORK)
    # Mac Framework location - may not be needed.
  else()
    # Standard Unix Tool
    set(MYGSL_CMAKE_CONFIG_DIR ${mygsl_cmake_config_install_dir} )
  endif()
endif()
message (STATUS "CMAKE_INSTALL_PREFIX='${CMAKE_INSTALL_PREFIX}'" )
message (STATUS "MYGSL_CMAKE_CONFIG_DIR='${MYGSL_CMAKE_CONFIG_DIR}'" )
message (STATUS "mygsl_cmake_config_install_dir='${mygsl_cmake_config_install_dir}'" )

# set(TOTO ${mygsl_cmake_config_install_dir} )
# message (STATUS "TOTO='${TOTO}'" )

install(EXPORT mygsl-libraries
  DESTINATION ${MYGSL_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )

# Configure the install tree CMake Config mode file
# Set variables
set(mygsl_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR} )
set(mygsl_LIB_DIR     ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}     )
# Configure file
# This is different from the build tree file, so store in dedicated directory
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config.cmake
  @ONLY
  )

# Configure the install tree versioning file
# N.B. it *probably* does not differ from the build tree file, but configuring
# separately allows easy changes should we need this.
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config-version.cmake
  @ONLY
  )

# Install the CMake Config mode files into the install tree
install ( FILES
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config.cmake
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config-version.cmake
          DESTINATION ${MYGSL_CMAKE_CONFIG_DIR}
          COMPONENT devel
        )

# - end of top level CMakeLists.txt for mygsl project.
