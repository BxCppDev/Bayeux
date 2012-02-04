# - Top level CMakeLists.txt for geomtools project
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
  message ( STATUS "geomtools requires an out-of-source build." )
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
project ( geomtools )

set ( geomtools_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${geomtools_PACKAGE_NAME}'" )

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

option ( GEOMTOOLS_WITH_DEBUG
         "Build the geomtools project using debugging code"
         0 )

option ( GEOMTOOLS_WITH_DOC 
         "Build the geomtools documentation" 
 	 1 )

option ( GEOMTOOLS_WITH_TEST 
         "Build the geomtools test material" 
 	 1 )

set ( GEOMTOOLS_WITH_BIO_SUPPORT 1 )

set ( GEOMTOOLS_WITH_PYTHON_WRAPPER_SUPPORT 1 )

option ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE 
         "Build the geomtools' CMake find module"
 	 0 )

# ---------------------------------------------------
# mygsl dependency :
#

if ( DEFINED mygsl_DIR )
  message ( STATUS "mygsl_DIR is : '${mygsl_DIR}' " )
else ()
  if ( NOT "$ENV{MYGSL_CMAKE_CONFIG_DIR}" STREQUAL "" )
    message ( STATUS "Environment MYGSL_CMAKE_CONFIG_DIR exists : '$ENV{MYGSL_CMAKE_CONFIG_DIR}' " )
    set ( mygsl_DIR "$ENV{MYGSL_CMAKE_CONFIG_DIR}" )
  endif ()
endif ()
set ( _mygsl_min_version 3.0 )
find_package ( mygsl ${_mygsl_min_version} REQUIRED NO_MODULE )
if ( DEFINED mygsl_CONFIG )
     message ( STATUS "Found mygsl >= ${_mygsl_min_version}" )
     message ( STATUS "  mygsl_CONFIG              : ${mygsl_CONFIG}" )
     message ( STATUS "  mygsl_VERSION             : ${mygsl_VERSION}" )
     message ( STATUS "  mygsl_LIBRARIES           : ${mygsl_LIBRARIES}" )
     message ( STATUS "  mygsl_INCLUDE_DIRS        : ${mygsl_INCLUDE_DIRS}" )
     message ( STATUS "  mygsl_WITH_DEBUG          : ${mygsl_WITH_DEBUG}" )
     #message ( STATUS "  mygsl_WITH_BIO            : ${mygsl_WITH_BIO}" )
     #message ( STATUS "  mygsl_WITH_PYTHON_WRAPPER : ${mygsl_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find mygsl >= ${_mygsl_min_version} library !" )
endif ()

# ---------------------------------------------------
# Gnuplot dependency :
#

find_package ( Gnuplot "4" REQUIRED)
if ( NOT GNUPLOT_FOUND )
   message ( FATAL_ERROR "Cannot find 'gnuplot' program !" )
else ()
   message ( STATUS "Found the 'gnuplot' program : '${GNUPLOT_EXECUTABLE}'" )     
endif () 

# ---------------------------------------------------
# datatools dependency :
#

set ( _depends_on_datatools 1 )

set ( _default_bio_activation 0 )
set ( _default_python_wrapper_activation 0 )

if ( _depends_on_datatools )
  # - Depends on datatools:

  ### if ( DEFINED datatools_DIR )
  ###   message ( STATUS "datatools_DIR is : '${datatools_DIR}' " )
  ### else ()
  ###   if ( NOT "$ENV{DATATOOLS_CMAKE_CONFIG_DIR}" STREQUAL "" )
  ###     message ( STATUS "Environment DATATOOLS_CMAKE_CONFIG_DIR exists : '$ENV{DATATOOLS_CMAKE_CONFIG_DIR}' " )
  ###     set ( datatools_DIR "$ENV{DATATOOLS_CMAKE_CONFIG_DIR}" )
  ###   endif ()
  ### endif ()
  ### set ( _datatools_min_version 4.0 )
  ### find_package ( datatools ${_datatools_min_version} REQUIRED NO_MODULE )
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
  if ( GEOMTOOLS_WITH_BIO_SUPPORT )
    if ( datatools_WITH_BIO )
      set ( _default_bio_activation 1 )
    endif ()
  endif ()
  if ( GEOMTOOLS_WITH_PYTHON_WRAPPER_SUPPORT )
    if ( datatools_WITH_PYTHON_WRAPPER )
      # XXXX
      message ( FATAL_ERROR "No support for Python wrapper for now !" )
      set ( _default_python_wrapper_activation 1 )
    endif ()
  endif ()
endif ()

option ( GEOMTOOLS_WITH_BIO 
         "Build the geomtools with Boost I/O serialization" 
	  ${_default_bio_activation} )

set ( GEOMTOOLS_WITH_PYTHON_WRAPPER 0)
  # option ( GEOMTOOLS_WITH_PYTHON_WRAPPER
  #          "Build the geomtools Boost/Python wrapper module" 
  #         ${_default_python_wrapper_activation} )

# ---------------------------------------------------

if (GEOMTOOLS_WITH_DEBUG)
  set (GEOMTOOLS_WITH_DEBUG 1)
endif ()

if ( GEOMTOOLS_WITH_BIO )
  set (GEOMTOOLS_WITH_BIO 1)
endif ()

if ( GEOMTOOLS_WITH_PYTHON_WRAPPER )
  set (GEOMTOOLS_WITH_PYTHON_WRAPPER 1)
endif ()

if (GEOMTOOLS_WITH_DOC)
  set (GEOMTOOLS_WITH_DOC 1)
endif ()

if (GEOMTOOLS_WITH_TEST)
  set (GEOMTOOLS_WITH_TEST 1)
endif ()

# ---------------------------------------------------

if ( UNIX OR APPLE )
  option ( GEOMTOOLS_WITH_CONFIG_SCRIPT 
           "Build the geomtools-config script" 
     	   1 )
  option ( GEOMTOOLS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   0 )
endif ()

# ---------------------------------------------------

option ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE 
         "Build the geomtools' CMake find module"
 	 OFF )

# ----------------------------------------------------------------------

message ( STATUS "GEOMTOOLS_WITH_DEBUG           : " "${GEOMTOOLS_WITH_DEBUG}" )
message ( STATUS "GEOMTOOLS_WITH_BIO             : " "${GEOMTOOLS_WITH_BIO}" )
message ( STATUS "GEOMTOOLS_WITH_PYTHON_WRAPPER  : " "${GEOMTOOLS_WITH_PYTHON_WRAPPER}" )
if ( UNIX OR APPLE )
  message ( STATUS "GEOMTOOLS_WITH_CONFIG_SCRIPT   : " "${GEOMTOOLS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "GEOMTOOLS_WITH_PKGCONFIG       : " "${GEOMTOOLS_WITH_PKGCONFIG}" )
endif ()
message ( STATUS "GEOMTOOLS_WITH_DOC             : " "${GEOMTOOLS_WITH_DOC}" )
message ( STATUS "GEOMTOOLS_WITH_TEST            : " "${GEOMTOOLS_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# ----------------------------------------------------------------------
# geomtools API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

# - CMake Config file:
set ( geomtools_cmake_config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} )

# - Pkgconfig file:
if ( GEOMTOOLS_WITH_PKGCONFIG )
  set ( _pkgconfig_install_dir ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
  if ( DEFINED GEOMTOOLS_PKGCONFIG_INSTALL_DIR)
    set ( _pkgconfig_install_dir ${GEOMTOOLS_PKGCONFIG_INSTALL_DIR} )
  endif ()
endif ()

# - CMake Module file (Find) :
if ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE )
  set ( geomtools_cmake_find_module_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/Modules ) 
  if ( DEFINED GEOMTOOLS_CMAKE_FIND_MODULE_INSTALL_DIR )
    set ( geomtools_cmake_find_module_install_dir ${GEOMTOOLS_CMAKE_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "GEOMTOOLS_CMAKE_FIND_MODULE_INSTALL_DIR : " "${GEOMTOOLS_CMAKE_FIND_MODULE_INSTALL_DIR}" )
  endif ()
  message ( STATUS "geomtools CMake find module install dir = '${geomtools_cmake_find_module_install_dir}'" )
endif ()

# ----------------------------------------------------------------------------------
# Installation path for resources :
#

set ( _geomtools_GDML_SCHEMA_FULL_LOCAL_PATH 
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/resources/gdml_schema" 
    ) 
message ( STATUS "GDML schema full local path = '${_geomtools_GDML_SCHEMA_FULL_LOCAL_PATH}' " )

# - C++ configuration header file :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools_config.h.in"
                 "${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
	  
        )

# - Bash setup script :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools_setup.sh.in"
                 "${PROJECT_BINARY_DIR}/cmake/geomtools_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/geomtools_setup.sh
          DESTINATION ${CMAKE_INSTALL_ETCDIR}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Pkgconfig configuration file :
if ( GEOMTOOLS_WITH_PKGCONFIG )

  set ( _geomtools_pkgconfig_Requires "Requires: " )

  set ( _geomtools_pkgconfig_Requires "${_geomtools_pkgconfig_Requires}mygsl >= ${_mygsl}" )

  set ( _geomtools_pkgconfig_Libs "-lgeomtools" )


  set ( _geomtools_pkgconfig_Cflags 
	  ${mygsl_INCLUDE_DIRS} 
        )
 
  set ( _geomtools_pkgconfig_Libs 
          ${_geomtools_pkgconfig_Libs} 
          ${mygsl_INCLUDE_DIRS} 
        ) 
  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools.pc.in"
                   "${PROJECT_BINARY_DIR}/cmake/geomtools.pc" @ONLY 
                 )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/geomtools.pc
            DESTINATION ${_pkgconfig_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
          )
endif ()

if ( GEOMTOOLS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindGeomtools.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindGeomtools.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindGeomtools.cmake
            DESTINATION ${_cmake_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( GEOMTOOLS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools-config.in"
                     "${PROJECT_BINARY_DIR}/cmake/geomtools-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/geomtools-config
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

if ( GEOMTOOLS_WITH_PYTHON_WRAPPER_SUPPORT AND GEOMTOOLS_WITH_PYTHON_WRAPPER )
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
     set ( GEOMTOOLS_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of geomtools, requires Boost.Python" FORCE)
  endif ()
endif () 
  
#set ( CMAKE_INSTALL_PYTHONDIR "" )
#message ( STATUS "CMAKE_INSTALL_PYTHONDIR = '${CMAKE_INSTALL_PYTHONDIR}' " )

if ( GEOMTOOLS_WITH_PYTHON_WRAPPER )
    message ( STATUS "Add subdirectory 'wrapping/python' " )
    add_subdirectory ( wrapping/python )
endif () 

# ----------------------------------------------------------------------------
# Install resources :
#

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/resources/gdml_schema
          DESTINATION ${CMAKE_INSTALL_DATADIR}/resources
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                           GROUP_READ
                           WORLD_READ
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                                GROUP_READ GROUP_EXECUTE 
                                WORLD_READ WORLD_EXECUTE
          PATTERN .svn EXCLUDE
         )

# 
# ----------------------------------------------------------------------------
# Add documentation.
#
if ( GEOMTOOLS_WITH_DOC )
  add_subdirectory ( doc )
endif ()

# ----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( GEOMTOOLS_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

# ----------------------------------------------------------------------------
# packaging
#

include ( ${GEOMTOOLS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# ----------------------------------------------------------------------------
# - Generate Build Tree configuration files
# - CMake
# Export targets from the build tree
# Done by hand for now because we know the targets exactly
export(TARGETS geomtools 
  FILE ${PROJECT_BINARY_DIR}/geomtools-libraries.cmake
  )
if ( GEOMTOOLS_WITH_BIO_SUPPORT AND GEOMTOOLS_WITH_BIO )
  export(TARGETS geomtools_bio 
    APPEND FILE ${PROJECT_BINARY_DIR}/geomtools-libraries.cmake
    )
endif()

# Configure the build tree CMake Config mode file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools-config.cmake.in
  ${PROJECT_BINARY_DIR}/geomtools-config.cmake
  @ONLY
  )

# Configure the build tree versioning file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/geomtools-config-version.cmake
  @ONLY
  )



#----------------------------------------------------------------------------
# - Generate Install Tree configuration files
# - CMake
# Export targets from the install tree
# Just need to install the named export to a suitable location
if(WIN32)
  set(GEOMTOOLS_CMAKE_CONFIG_DIR geomtools-${geomtools_VERSION_STR} )
else()
  if(GEOMTOOLS_IS_FRAMEWORK)
    # Mac Framework location - may not be needed.
  else()
    # Standard Unix Tool
    set(GEOMTOOLS_CMAKE_CONFIG_DIR ${geomtools_cmake_config_install_dir} )
  endif()
endif()

install(EXPORT geomtools-libraries
  DESTINATION ${GEOMTOOLS_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )

# Configure the install tree CMake Config mode file
# Set variables
set(geomtools_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(geomtools_LIB_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
# Configure file
# This is different from the build tree file, so store in dedicated directory
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools-config.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/geomtools-config.cmake
  @ONLY
  )


# Configure the install tree versioning file
# N.B. it *probably* does not differ from the build tree file, but configuring
# separately allows easy changes should we need this.
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/geomtools-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/geomtools-config-version.cmake
  @ONLY
  )

# Install the CMake Config mode files into the install tree
install ( FILES
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/geomtools-config.cmake
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/geomtools-config-version.cmake
          DESTINATION ${GEOMTOOLS_CMAKE_CONFIG_DIR}
          COMPONENT devel
        )

# - end of top level CMakeLists.txt for geomtools project.
