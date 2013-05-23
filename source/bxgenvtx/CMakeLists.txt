# - Top level CMakeLists.txt for genvtx project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "genvtx requires an out-of-source build.")
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
project(genvtx)

# - Versioning
set(genvtx_VERSION_MAJOR 4)
set(genvtx_VERSION_MINOR 0)
set(genvtx_PATCH_VERSION 0)
set(genvtx_VERSION "${genvtx_VERSION_MAJOR}.${genvtx_VERSION_MINOR}.${genvtx_PATCH_VERSION}")
math(EXPR _genvtx_VERSION_NUMERIC "${genvtx_VERSION_MAJOR}*100000+${genvtx_VERSION_MINOR}*100+${genvtx_PATCH_VERSION}")

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
option(GENVTX_WITH_DEBUG "Build debug stuff" OFF)
option(GENVTX_WITH_BIO  "Build auxiliary Boost I/O library" OFF)
option(GENVTX_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" OFF)
option(GENVTX_WITH_DOC  "Build the genvtx documentation" ON)
option(GENVTX_WITH_TEST "Build the genvtx test suite"    ON)
option(GENVTX_WITH_BASH_SETUP "Build the genvtx with Bash setup macro" ON)
option(GENVTX_WITH_GNUPLOT_DISPLAY  "Build the Gnuplot-based display tools" ON )


if (GENVTX_WITH_DEBUG)
  set (GENVTX_WITH_DEBUG 1)
else()
  set (GENVTX_WITH_DEBUG 0)
endif ()

if(GENVTX_WITH_BIO)
  set (GENVTX_WITH_BIO 1)
else()
  set (GENVTX_WITH_BIO 0)
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
  set(GENVTX_FRAMEWORK_NAME genvtx)
  set(GENVTX_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${GENVTX_FRAMEWORK_NAME}.framework/Versions/${genvtx_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)

if(GENVTX_WITH_DOC)
  set (GENVTX_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set (GENVTX_WITH_DOC 0)
endif()

if(GENVTX_WITH_TEST)
  set (GENVTX_WITH_TEST 1)
  enable_testing()
  add_subdirectory(testing)
else()
  set (GENVTX_WITH_TEST 0)
endif()

add_subdirectory(programs)


#----------------------------------------------------------------
# - Install public resources :

# - Examples :
set(_genvtx_examples_FULL_LOCAL_PATH
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/examples"
   )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples
        DESTINATION ${CMAKE_INSTALL_DATADIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                         GROUP_READ GROUP_WRITE
                         WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                              GROUP_READ GROUP_WRITE GROUP_EXECUTE
                              WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE
	PATTERN "*~" EXCLUDE
       )

#-----------------------------------------------------------------------
# Package
#
include(GenvtxCPack)

# - end of top level CMakeLists.txt for genvtx project.
