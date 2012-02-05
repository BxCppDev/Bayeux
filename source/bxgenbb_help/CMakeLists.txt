# - Top level CMakeLists.txt for genbb_help project
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
  message ( STATUS "genbb_help requires an out-of-source build." )
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
project ( genbb_help  C CXX Fortran )

set ( genbb_help_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${genbb_help_PACKAGE_NAME}'" )

#----------------------------------------------------------------------------
# Prepend this project's custom module path(s) to CMAKE_MODULE_PATH
# Append any path from the environment
# This allows us to customize, add, and factor out functionality
# NB: if our custom modules include others that we don't supply, those in the
# base path will be used, so watch for incompatibilities!!
#
file (TO_CMAKE_PATH "$ENV{CMAKE_MODULE_PATH}" __env_cmake_module_path)
	
set(CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
  ${PROJECT_SOURCE_DIR}/cmake/Modules
  ${__env_cmake_module_path}
  ${CMAKE_MODULE_PATH}
)

if (DEFINED ADDITIONAL_CMAKE_MODULE_PATH AND NOT "x${ADDITIONAL_CMAKE_MODULE_PATH}" STREQUAL "x" ))
  message ( STATUS "genbb_help: Using additional CMAKE_MODULE_PATH : '${ADDITIONAL_CMAKE_MODULE_PATH}'" )
  set(CMAKE_MODULE_PATH
      ${ADDITIONAL_CMAKE_MODULE_PATH}
      ${CMAKE_MODULE_PATH}
  )
endif ()

message ( STATUS "genbb_help: CMAKE_MODULE_PATH         : '${CMAKE_MODULE_PATH}'" )
	
#----------------------------------------------------------------------------
# Load any required modules
#
include ( Config )
include ( InstallDirs )
include ( PackageFilename )

# ----------------------------------------------------------------------
# Options :
#

option ( GENBB_HELP_WITH_DEBUG
         "Build the genbb_help project using debugging code"
         0 )

option ( GENBB_HELP_WITH_DOC 
         "Build the genbb_help documentation" 
 	 1 )

option ( GENBB_HELP_WITH_TEST 
         "Build the genbb_help test material" 
 	 1 )

set ( GENBB_HELP_WITH_BIO_SUPPORT 1 )

set ( GENBB_HELP_WITH_PYTHON_WRAPPER_SUPPORT 0 )

option ( GENBB_HELP_WITH_CMAKE_FIND_MODULE 
         "Build the genbb_help' CMake find module"
 	 0 )

# ----------------------------------------------------------------------
# Dependencies :
#

set ( _default_bio_activation 0 )
set ( _default_python_wrapper_activation 0 )

# - Depends on geomtools:
if ( DEFINED geomtools_DIR )
    message ( STATUS "geomtools_DIR is : '${geomtools_DIR}' " )
else ()
    if ( NOT "$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}" STREQUAL "" )
      message ( STATUS "Environment GEOMTOOLS_CMAKE_CONFIG_DIR exists : '$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}' " )
      set ( geomtools_DIR "$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}" )
    endif ()
endif ()

set ( _geomtools_min_version 3.0 )
find_package ( geomtools ${_geomtools_min_version} REQUIRED NO_MODULE )
if ( DEFINED geomtools_CONFIG )
       message ( STATUS "Found geomtools >= ${_geomtools_min_version}" )
       message ( STATUS "  geomtools_CONFIG              : ${geomtools_CONFIG}" )
       message ( STATUS "  geomtools_VERSION             : ${geomtools_VERSION}" )
       message ( STATUS "  geomtools_LIBRARIES           : ${geomtools_LIBRARIES}" )
       message ( STATUS "  geomtools_INCLUDE_DIRS        : ${geomtools_INCLUDE_DIRS}" )
       message ( STATUS "  geomtools_WITH_DEBUG          : ${geomtools_WITH_DEBUG}" )
       message ( STATUS "  geomtools_WITH_BIO            : ${geomtools_WITH_BIO}" )
       message ( STATUS "  geomtools_WITH_PYTHON_WRAPPER : ${geomtools_WITH_PYTHON_WRAPPER}" )
else ()
       message ( FATAL_ERROR "Cannot find geomtools >= ${_geomtools_min_version} library !" )
endif ()
if ( GENBB_HELP_WITH_BIO_SUPPORT )
    if ( geomtools_WITH_BIO )
      set ( _default_bio_activation 1 )
    endif ()
endif ()
if ( GENBB_HELP_WITH_PYTHON_WRAPPER_SUPPORT )
    if ( geomtools_WITH_PYTHON_WRAPPER )
      set ( _default_python_wrapper_activation 1 )
    endif ()
endif ()

if ( GENBB_HELP_WITH_BIO_SUPPORT )
  option ( GENBB_HELP_WITH_BIO 
           "Build the genbb_help with Boost I/O serialization" 
      	   ${_default_bio_activation} )
endif ()

if ( GENBB_HELP_WITH_PYTHON_WRAPPER_SUPPORT )
  option ( GENBB_HELP_WITH_PYTHON_WRAPPER
           "Build the genbb_help Boost/Python wrapper module" 
           ${_default_python_wrapper_activation} )
endif ()

# ---------------------------------------------------

if (GENBB_HELP_WITH_DEBUG)
  set (GENBB_HELP_WITH_DEBUG 1)
endif ()

if ( GENBB_HELP_WITH_BIO_SUPPORT AND GENBB_HELP_WITH_BIO )
  set (GENBB_HELP_WITH_BIO 1)
endif ()

if ( GENBB_HELP_WITH_PYTHON_WRAPPER_SUPPORT AND GENBB_HELP_WITH_PYTHON_WRAPPER )
  set (GENBB_HELP_WITH_PYTHON_WRAPPER 1)
endif ()

if (GENBB_HELP_WITH_DOC)
  set (GENBB_HELP_WITH_DOC 1)
endif ()

if (GENBB_HELP_WITH_TEST)
  set (GENBB_HELP_WITH_TEST 1)
endif ()

# ---------------------------------------------------

if ( UNIX OR APPLE )
  option ( GENBB_HELP_WITH_CONFIG_SCRIPT 
           "Build the genbb_help-config script" 
     	   1 )
  option ( GENBB_HELP_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   0 )
endif ()
# ---------------------------------------------------

option ( GENBB_HELP_WITH_CMAKE_FIND_MODULE 
         "Build the genbb_help CMake find module"
 	 OFF )

# ----------------------------------------------------------------------

message ( STATUS "GENBB_HELP_WITH_DEBUG           : " "${GENBB_HELP_WITH_DEBUG}" )
if ( GENBB_HELP_WITH_BIO_SUPPORT )
  message ( STATUS "GENBB_HELP_WITH_BIO             : " "${GENBB_HELP_WITH_BIO}" )
endif ()
if ( GENBB_HELP_WITH_PYTHON_WRAPPER_SUPPORT )
  message ( STATUS "GENBB_HELP_WITH_PYTHON_WRAPPER  : " "${GENBB_HELP_WITH_PYTHON_WRAPPER}" )
endif ()
if ( UNIX OR APPLE )
  message ( STATUS "GENBB_HELP_WITH_CONFIG_SCRIPT   : " "${GENBB_HELP_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "GENBB_HELP_WITH_PKGCONFIG       : " "${GENBB_HELP_WITH_PKGCONFIG}" )
endif ()
message ( STATUS "GENBB_HELP_WITH_DOC             : " "${GENBB_HELP_WITH_DOC}" )
message ( STATUS "GENBB_HELP_WITH_TEST            : " "${GENBB_HELP_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX    : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# ----------------------------------------------------------------------
# genbb_help API
#

set ( _genbb_help_fortran_definitions " -Dfermi_wrap=fermi_wrap_ -Drng_shoot_reset=rng_shoot_reset_ -Drng_shoot_flat=rng_shoot_flat_ " )

set ( _genbb_help_definitions " -Df2cFortran -Dgenbbsub=genbbsub_ -Dset_genbb_random_seed=set_genbb_random_seed_ -Dgrndm=grndm_ -Dgenevent=genevent_ -Dgenbb_init_enrange=genbb_init_enrange_ -Denrange=enrange_ " )

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

message ( STATUS "Add subdirectory : " "'programs'" )
add_subdirectory ( programs )   

# ----------------------------------------------------------------------
# Configuration files :
#

# - CMake Config file:
set ( genbb_help_cmake_config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} )

# - Pkgconfig file:
if ( GENBB_HELP_WITH_PKGCONFIG )
  set ( _pkgconfig_install_dir ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
  if ( DEFINED GENBB_HELP_PKGCONFIG_INSTALL_DIR)
    set ( _pkgconfig_install_dir ${GENBB_HELP_PKGCONFIG_INSTALL_DIR} )
  endif ()
endif ()

# - CMake Module file (Find) :
if ( GENBB_HELP_WITH_CMAKE_FIND_MODULE )
  set ( genbb_help_cmake_find_module_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/Modules ) 
  if ( DEFINED GENBB_HELP_CMAKE_FIND_MODULE_INSTALL_DIR )
    set ( genbb_help_cmake_find_module_install_dir ${GENBB_HELP_CMAKE_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "GENBB_HELP_CMAKE_FIND_MODULE_INSTALL_DIR : " "${GENBB_HELP_CMAKE_FIND_MODULE_INSTALL_DIR}" )
  endif ()
  message ( STATUS "genbb_help CMake find module install dir = '${genbb_help_cmake_find_module_install_dir}'" )
endif ()

# - C++ configuration header file :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help_config.h.in"
                 "${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Bash setup script :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help_setup.sh.in"
                 "${PROJECT_BINARY_DIR}/cmake/genbb_help_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/genbb_help_setup.sh
          DESTINATION ${CMAKE_INSTALL_ETCDIR}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Pkgconfig configuration file :
if ( GENBB_HELP_WITH_PKGCONFIG )

  set ( _genbb_help_pkgconfig_Requires "Requires: " )

  set ( _genbb_help_pkgconfig_Requires "${_genbb_help_pkgconfig_Requires}geomtools >= ${_geomtools_min_version}" )

  set ( _genbb_help_pkgconfig_Libs "-lgenbb_help" )

  set ( _genbb_help_pkgconfig_Cflags 
        ${geomtools_INCLUDE_DIRS} 
      )
 
  set ( _genbb_help_pkgconfig_Libs 
        ${_genbb_help_pkgconfig_Libs} 
        ${geomtools_LIBRARIES} 
      ) 
  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help.pc.in"
                   "${PROJECT_BINARY_DIR}/cmake/genbb_help.pc" @ONLY 
                 )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/genbb_help.pc
            DESTINATION ${_pkgconfig_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
          )
endif ()

if ( GENBB_HELP_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindGenbb_help.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindGenbb_help.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindGenbb_help.cmake
            DESTINATION ${_cmake_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( GENBB_HELP_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help-config.in"
                     "${PROJECT_BINARY_DIR}/cmake/genbb_help-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/genbb_help-config
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

if ( GENBB_HELP_WITH_PYTHON_WRAPPER_SUPPORT AND GENBB_HELP_WITH_PYTHON_WRAPPER )
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
     set ( GENBB_HELP_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of genbb_help, requires Boost.Python" FORCE)
  endif ()
endif () 
  
#set ( CMAKE_INSTALL_PYTHONDIR "" )
#message ( STATUS "CMAKE_INSTALL_PYTHONDIR = '${CMAKE_INSTALL_PYTHONDIR}' " )

if ( GENBB_HELP_WITH_PYTHON_WRAPPER )
    message ( STATUS "Add subdirectory 'wrapping/python' " )
    add_subdirectory ( wrapping/python )
endif () 

# ----------------------------------------------------------------------------
# Add documentation.
#
if ( GENBB_HELP_WITH_DOC )
  add_subdirectory ( doc )
endif ()

# ----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( GENBB_HELP_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

# ----------------------------------------------------------------------------
# packaging
#

include ( ${GENBB_HELP_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# ----------------------------------------------------------------------------
# - Generate Build Tree configuration files
# - CMake
# Export targets from the build tree
# Done by hand for now because we know the targets exactly

export(TARGETS genbb_help_f77 
  FILE ${PROJECT_BINARY_DIR}/genbb_help-libraries.cmake
  )

export(TARGETS genbb_help APPEND
  FILE ${PROJECT_BINARY_DIR}/genbb_help-libraries.cmake
  )

if ( GENBB_HELP_WITH_BIO_SUPPORT AND GENBB_HELP_WITH_BIO )
  export(TARGETS genbb_help_bio 
    APPEND FILE ${PROJECT_BINARY_DIR}/genbb_help-libraries.cmake
    )
endif()

# Configure the build tree CMake Config mode file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help-config.cmake.in
  ${PROJECT_BINARY_DIR}/genbb_help-config.cmake
  @ONLY
  )

# Configure the build tree versioning file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/genbb_help-config-version.cmake
  @ONLY
  )

#----------------------------------------------------------------------------
# - Generate Install Tree configuration files
# - CMake
# Export targets from the install tree
# Just need to install the named export to a suitable location
if(WIN32)
  set(GENBB_HELP_CMAKE_CONFIG_DIR genbb_help-${genbb_help_VERSION_STR} )
else()
  if(GENBB_HELP_IS_FRAMEWORK)
    # Mac Framework location - may not be needed.
  else()
    # Standard Unix Tool
    set(GENBB_HELP_CMAKE_CONFIG_DIR ${genbb_help_cmake_config_install_dir} )
  endif()
endif()

install ( EXPORT genbb_help-libraries
          DESTINATION ${GENBB_HELP_CMAKE_CONFIG_DIR}
          COMPONENT devel
        )

# Configure the install tree CMake Config mode file
# Set variables
set ( genbb_help_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR} )
set ( genbb_help_LIB_DIR     ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}     )
# Configure file
# This is different from the build tree file, so store in dedicated directory
configure_file (
  ${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help-config.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/genbb_help-config.cmake
  @ONLY
  )

# Configure the install tree versioning file
# N.B. it *probably* does not differ from the build tree file, but configuring
# separately allows easy changes should we need this.
configure_file (
  ${PROJECT_SOURCE_DIR}/cmake/Templates/genbb_help-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/genbb_help-config-version.cmake
  @ONLY
  )

# Install the CMake Config mode files into the install tree
install ( FILES
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/genbb_help-config.cmake
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/genbb_help-config-version.cmake
          DESTINATION ${GENBB_HELP_CMAKE_CONFIG_DIR}
          COMPONENT devel
        )

# - end of top level CMakeLists.txt for genbb_help project.
