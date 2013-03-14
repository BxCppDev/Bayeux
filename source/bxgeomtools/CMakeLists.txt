# - Top level CMakeLists.txt for geomtools project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "geomtools requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# Project configuration
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(geomtools)

# - Versioning
set(geomtools_VERSION_MAJOR 4)
set(geomtools_VERSION_MINOR 0)
set(geomtools_PATCH_VERSION 0)
set(geomtools_VERSION "${geomtools_VERSION_MAJOR}.${geomtools_VERSION_MINOR}.${geomtools_PATCH_VERSION}")
math(EXPR _geomtools_VERSION_NUMERIC "${geomtools_VERSION_MAJOR}*100000+${geomtools_VERSION_MINOR}*100+${geomtools_PATCH_VERSION}")

# - Fixup default install prefix on Mac
if(APPLE AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/Library/Frameworks" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif()

# - Load custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(InstallDirs)

set( _ld_library_path_env "LD_LIBRARY_PATH")
if(APPLE)
  set(_ld_library_path_env "DYLD_LIBRARY_PATH")
endif()

#-----------------------------------------------------------------------
# Build options
#
option(GEOMTOOLS_WITH_DEBUG "Build debug stuff" OFF) 
option(GEOMTOOLS_WITH_BIO  "Build auxiliary Boost I/O library" ON) 
option(GEOMTOOLS_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" OFF) 
option(GEOMTOOLS_WITH_DOC  "Build the geomtools documentation" ON)
option(GEOMTOOLS_WITH_TEST "Build the geomtools test suite"    ON) 
option(GEOMTOOLS_WITH_BASH_SETUP "Build the geomtools with Bash setup macro" ON)
option(GEOMTOOLS_WITH_GNUPLOT_DISPLAY  "Build the Gnuplot-based display tools" ON )
option(GEOMTOOLS_WITH_ROOT_DISPLAY  "Allow the ROOT display tools" ON )

if (GEOMTOOLS_WITH_DEBUG)
  set (GEOMTOOLS_WITH_DEBUG 1)
else()
  set (GEOMTOOLS_WITH_DEBUG 0)
endif ()

if(GEOMTOOLS_WITH_BIO)
  set (GEOMTOOLS_WITH_BIO 1)
else()
  set (GEOMTOOLS_WITH_BIO 0)
endif ()

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - mygsl
find_package(mygsl 4.0.0 REQUIRED)

# - materials
find_package(materials 4.0.0 REQUIRED)

###message ( STATUS "********** CMAKE_MODULE_PATH='${CMAKE_MODULE_PATH}' ")

# - CLHEP
find_package(CLHEP 2.1.0.1 REQUIRED NOMODULE)

# - CAMP
if (GEOMTOOLS_WITH_REFLECTION)
  set (GEOMTOOLS_WITH_REFLECTION 1)
  find_package(CAMP 0.7.1 REQUIRED)
else()
  set (GEOMTOOLS_WITH_REFLECTION 0)
endif ()

if(GEOMTOOLS_WITH_GNUPLOT_DISPLAY)
  set (GEOMTOOLS_WITH_GNUPLOT_DISPLAY 1)
  # Gnuplot dependency :  
  set ( _gnuplot_min_version 4.4 ) 
  if ( NOT "$ENV{GNUPLOT_ROOT_DIR}" STREQUAL "")  
    message ( STATUS "Environment GNUPLOT_ROOT_DIR exists : '$ENV{GNUPLOT_ROOT_DIR}' ")
    set ( GNUPLOT_ROOT_DIR $ENV{GNUPLOT_ROOT_DIR} )
  endif ()
  find_package ( Gnuplot ${_gnuplot_min_version} REQUIRED)
else()
  set (GEOMTOOLS_WITH_GNUPLOT_DISPLAY 0)
endif ()

if(GEOMTOOLS_WITH_ROOT_DISPLAY)
  set (GEOMTOOLS_WITH_ROOT_DISPLAY 1)
  find_package(ROOT 5.26.00 REQUIRED)
else()
  set (GEOMTOOLS_WITH_ROOT_DISPLAY 0)
endif ()

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory. 
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(GEOMTOOLS_FRAMEWORK_NAME geomtools)
  set(GEOMTOOLS_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${GEOMTOOLS_FRAMEWORK_NAME}.framework/Versions/${geomtools_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)   

add_subdirectory(programs)   

if(GEOMTOOLS_WITH_DOC)
  set (GEOMTOOLS_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set (GEOMTOOLS_WITH_DOC 0)
endif()

if(GEOMTOOLS_WITH_TEST)
  set (GEOMTOOLS_WITH_TEST 1)
  enable_testing() 
  add_subdirectory(testing)
else()
  set (GEOMTOOLS_WITH_TEST 0)
endif()

# ----------------------------------------------------------------------------
# Install resources :
#
set ( _geomtools_GDML_SCHEMA_FULL_LOCAL_PATH 
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/resources/gdml_schema" 
    ) 
message ( STATUS "${PROJECT_NAME} : GDML schema full local path = '${_geomtools_GDML_SCHEMA_FULL_LOCAL_PATH}' " )

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/resources/gdml_schema
          DESTINATION ${CMAKE_INSTALL_DATADIR}/resources
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                           GROUP_READ GROUP_WRITE
                           WORLD_READ
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                                GROUP_READ GROUP_WRITE  GROUP_EXECUTE 
                                WORLD_READ WORLD_EXECUTE
          PATTERN ".svn" EXCLUDE
          PATTERN "*~" EXCLUDE
         )

set ( _geomtools_testing_FULL_LOCAL_PATH 
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/testing" 
    ) 
message ( STATUS "${PROJECT_NAME} : geomtools testing full local path = '${_geomtools_testing_FULL_LOCAL_PATH}' " )

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/testing
          DESTINATION ${CMAKE_INSTALL_DATADIR}
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                           GROUP_READ GROUP_WRITE
                           WORLD_READ
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                                GROUP_READ GROUP_WRITE  GROUP_EXECUTE 
                                WORLD_READ WORLD_EXECUTE
          PATTERN ".svn" EXCLUDE
          PATTERN "*~" EXCLUDE
         )

# ----------------------------------------------------------------------------
# Install resources :
#

set ( _geomtools_testing_FULL_LOCAL_PATH 
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/testing" 
    ) 
message ( STATUS "${PROJECT_NAME} : geomtools testing full local path = '${_geomtools_testing_FULL_LOCAL_PATH}' " )

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/testing
          DESTINATION ${CMAKE_INSTALL_DATADIR}
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                           GROUP_READ GROUP_WRITE
                           WORLD_READ
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                                GROUP_READ GROUP_WRITE  GROUP_EXECUTE 
                                WORLD_READ WORLD_EXECUTE
          PATTERN ".svn" EXCLUDE
          PATTERN "*~" EXCLUDE
          PATTERN CMakeLists.txt EXCLUDE
          PATTERN testDriver.bash EXCLUDE
         )

#-----------------------------------------------------------------------
# Package
#
include(GeomtoolsCPack)

# - end of top level CMakeLists.txt for geomtools project.
