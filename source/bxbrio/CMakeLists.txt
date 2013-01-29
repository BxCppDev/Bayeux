# - Top level CMakeLists.txt for brio project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "brio requires an out-of-source build.")
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
project(brio)

# - Versioning
set(brio_VERSION_MAJOR 4)
set(brio_VERSION_MINOR 0)
set(brio_PATCH_VERSION 0)
set(brio_VERSION "${brio_VERSION_MAJOR}.${brio_VERSION_MINOR}.${brio_PATCH_VERSION}")
math(EXPR _brio_VERSION_NUMERIC "${brio_VERSION_MAJOR}*100000+${brio_VERSION_MINOR}*100+${brio_PATCH_VERSION}")

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
option(BRIO_WITH_DEBUG "Build debug stuff" OFF) 
option(BRIO_WITH_DOC  "Build the brio documentation" ON)
option(BRIO_WITH_TEST "Build the brio test suite"    ON) 
option(BRIO_WITH_BASH_SETUP "Build the brio with Bash setup macro" ON) 

#-----------------------------------------------------------------------
# Find external packages needed to build
#

# - datatools
find_package(datatools 5.0.0 REQUIRED)
message ( STATUS "datatools_INCLUDE_DIRS=${datatools_INCLUDE_DIRS}")
message ( STATUS "datatools_LIBRARIES=${datatools_LIBRARIES}")

# - ROOT
find_package(ROOT 5.26.00 REQUIRED)

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory. 
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(BRIO_FRAMEWORK_NAME brio)
  set(BRIO_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${BRIO_FRAMEWORK_NAME}.framework/Versions/${brio_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)   

if(BRIO_WITH_DOC)
  add_subdirectory(doc)
endif()

if(BRIO_WITH_TEST)
  enable_testing() 
  add_subdirectory(testing)
endif()

#-----------------------------------------------------------------------
# Package
#
include(BrioCPack)

# - end of top level CMakeLists.txt for brio project.
