# - Top level CMakeLists.txt for materials project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "materials requires an out-of-source build.")
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
project(materials)

# - Versioning
set(materials_VERSION_MAJOR 4)
set(materials_VERSION_MINOR 0)
set(materials_PATCH_VERSION 1)
set(materials_VERSION "${materials_VERSION_MAJOR}.${materials_VERSION_MINOR}.${materials_PATCH_VERSION}")
math(EXPR _materials_VERSION_NUMERIC "${materials_VERSION_MAJOR}*100000+${materials_VERSION_MINOR}*100+${materials_PATCH_VERSION}")

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
option(MATERIALS_WITH_DEBUG "Build debug stuff" OFF)
option(MATERIALS_WITH_BIO  "Build auxiliary Boost I/O library" OFF)
option(MATERIALS_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" OFF)
option(MATERIALS_WITH_DOC  "Build the materials documentation" ON)
option(MATERIALS_WITH_TEST "Build the materials test suite"    ON)
option(MATERIALS_WITH_BASH_SETUP "Build the materials with Bash setup macro" ON)
option(MATERIALS_WITH_GNUPLOT_DISPLAY  "Build the Gnuplot-based display tools" ON )


if (MATERIALS_WITH_DEBUG)
  set (MATERIALS_WITH_DEBUG 1)
else()
  set (MATERIALS_WITH_DEBUG 0)
endif ()

if(MATERIALS_WITH_BIO)
  set (MATERIALS_WITH_BIO 1)
else()
  set (MATERIALS_WITH_BIO 0)
endif ()

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - datatools
find_package(datatools 5.0.0 REQUIRED)

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory.
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(MATERIALS_FRAMEWORK_NAME materials)
  set(MATERIALS_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${MATERIALS_FRAMEWORK_NAME}.framework/Versions/${materials_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)

if(MATERIALS_WITH_DOC)
  set (MATERIALS_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set (MATERIALS_WITH_DOC 0)
endif()

if(MATERIALS_WITH_TEST)
  set (MATERIALS_WITH_TEST 1)
  enable_testing()
  add_subdirectory(testing)
else()
  set (MATERIALS_WITH_TEST 0)
endif()

# ----------------------------------------------------------------------------
# Install resources :
#
set ( _materials_DATA_SCHEMA_FULL_LOCAL_PATH
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/resources/data"
    )

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/resources/data
          DESTINATION ${CMAKE_INSTALL_DATADIR}/resources
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                           GROUP_READ GROUP_WRITE
                           WORLD_READ
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                GROUP_READ GROUP_WRITE  GROUP_EXECUTE
                                WORLD_READ WORLD_EXECUTE
          PATTERN ".svn" EXCLUDE
         )

set ( _materials_TESTING_FULL_LOCAL_PATH
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/testing"
    )
message ( STATUS "${PROJECT_NAME} : materials testing full local path = '${_materials_TESTING_FULL_LOCAL_PATH}' " )

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
include(MaterialsCPack)

# - end of top level CMakeLists.txt for materials project.
