# - Top level CMakeLists.txt for emfield project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "emfield requires an out-of-source build.")
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
project(emfield)

# - Versioning
set(emfield_VERSION_MAJOR 1)
set(emfield_VERSION_MINOR 0)
set(emfield_PATCH_VERSION 0)
set(emfield_VERSION "${emfield_VERSION_MAJOR}.${emfield_VERSION_MINOR}.${emfield_PATCH_VERSION}")
math(EXPR _emfield_VERSION_NUMERIC "${emfield_VERSION_MAJOR}*100000+${emfield_VERSION_MINOR}*100+${emfield_PATCH_VERSION}")

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
option(EMFIELD_WITH_DEBUG "Build debug stuff" OFF)
# option(EMFIELD_WITH_BIO  "Build auxiliary Boost I/O library" OFF)
# option(EMFIELD_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" OFF)
option(EMFIELD_WITH_DOC  "Build the emfield documentation" ON)
option(EMFIELD_WITH_TEST "Build the emfield test suite"    ON)
option(EMFIELD_WITH_BASH_SETUP "Build the emfield with Bash setup macro" ON)
option(EMFIELD_WITH_GNUPLOT_DISPLAY  "Build the Gnuplot-based display tools" ON )


if (EMFIELD_WITH_DEBUG)
  set (EMFIELD_WITH_DEBUG 1)
else()
  set (EMFIELD_WITH_DEBUG 0)
endif ()

if(EMFIELD_WITH_BIO)
  set (EMFIELD_WITH_BIO 1)
else()
  set (EMFIELD_WITH_BIO 0)
endif ()

if(EMFIELD_WITH_GNUPLOT_DISPLAY)
  set (EMFIELD_WITH_GNUPLOT_DISPLAY 1)
else()
  set (EMFIELD_WITH_GNUPLOT_DISPLAY 0)
endif ()

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - geomtools
find_package(geomtools 4.0.0 REQUIRED)

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory.
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(EMFIELD_FRAMEWORK_NAME emfield)
  set(EMFIELD_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${EMFIELD_FRAMEWORK_NAME}.framework/Versions/${emfield_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)

if(EMFIELD_WITH_DOC)
  set (EMFIELD_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set (EMFIELD_WITH_DOC 0)
endif()

if(EMFIELD_WITH_TEST)
  set (EMFIELD_WITH_TEST 1)
  enable_testing()
  add_subdirectory(testing)
else()
  set (EMFIELD_WITH_TEST 0)
endif()

# ----------------------------------------------------------------------------
# Install resources :
#

set ( _emfield_TESTING_FULL_LOCAL_PATH
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/testing"
    )
message ( STATUS "${PROJECT_NAME} : emfield testing full local path = '${_emfield_TESTING_FULL_LOCAL_PATH}' " )

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/testing
          DESTINATION ${CMAKE_INSTALL_DATADIR}
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                           GROUP_READ GROUP_WRITE
                           WORLD_READ
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                GROUP_READ GROUP_WRITE  GROUP_EXECUTE
                                WORLD_READ WORLD_EXECUTE
          PATTERN 		".svn" EXCLUDE
          PATTERN "*~" EXCLUDE
          PATTERN "CMakeLists.txt" EXCLUDE
          PATTERN "testDriver.bash" EXCLUDE
         )

#-----------------------------------------------------------------------
# Package
#
include(EmfieldCPack)

# - end of top level CMakeLists.txt for emfield project.
