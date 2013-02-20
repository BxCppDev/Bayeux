# - Top level CMakeLists.txt for datatools project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
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

#-----------------------------------------------------------------------
# Project configuration
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(datatools)

# - Versioning
set(datatools_VERSION_MAJOR 5)
set(datatools_VERSION_MINOR 0)
set(datatools_VERSION_PATCH 0)
set(datatools_VERSION "${datatools_VERSION_MAJOR}.${datatools_VERSION_MINOR}.${datatools_VERSION_PATCH}")
math(EXPR _datatools_VERSION_NUMERIC "${datatools_VERSION_MAJOR}*100000+${datatools_VERSION_MINOR}*100+${datatools_VERSION_PATCH}")

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
option(DATATOOLS_WITH_DEBUG "Build debug stuff" OFF) 
option(DATATOOLS_WITH_BIO  "Build auxiliary Boost I/O library" ON) 
option(DATATOOLS_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" ON) 
option(DATATOOLS_WITH_DOC  "Build the datatools documentation" ON)
option(DATATOOLS_WITH_TEST "Build the datatools test suite"    ON) 
option(DATATOOLS_WITH_BASH_SETUP "Build the datatools with Bash setup macro" ON) 

if (DATATOOLS_WITH_DEBUG)
  set (DATATOOLS_WITH_DEBUG 1)
else()
  set (DATATOOLS_WITH_DEBUG 0)
endif ()

if(DATATOOLS_WITH_BIO)
  set (DATATOOLS_WITH_BIO 1)
else()
  set (DATATOOLS_WITH_BIO 0)
endif ()


#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - Boost
set(Boost_COMPONENTS iostreams serialization filesystem system)
find_package(Boost 1.47 QUIET COMPONENTS ${Boost_COMPONENTS} NO_MODULE)
find_package(Boost 1.47 REQUIRED ${Boost_COMPONENTS})

# - CLHEP
find_package(CLHEP 2.1.0.1 REQUIRED)

# - CAMP
if(DATATOOLS_WITH_REFLECTION)
  set(DATATOOLS_WITH_REFLECTION 1)
  find_package(CAMP 0.7.1 REQUIRED)
else()
  set(DATATOOLS_WITH_REFLECTION 0)
endif()

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory. 
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(DATATOOLS_FRAMEWORK_NAME datatools)
  set(DATATOOLS_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${DATATOOLS_FRAMEWORK_NAME}.framework/Versions/${datatools_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)   

if(DATATOOLS_WITH_DOC)
  set(DATATOOLS_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set(DATATOOLS_WITH_DOC 0)
endif()

if(DATATOOLS_WITH_TEST)
  set(DATATOOLS_WITH_TEST 1)
  enable_testing() 
  add_subdirectory(testing)
else()
  set(DATATOOLS_WITH_TEST 0)
endif()

#-----------------------------------------------------------------------
# Package
#
include(DatatoolsCPack)

# - end of top level CMakeLists.txt for datatools project.
