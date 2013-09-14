# - Top level CMakeLists.txt for dpp project
#-----------------------------------------------------------------------
# Copyright (c) 2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                     Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "dpp requires an out-of-source build.")
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
project(dpp)

# - Versioning
set(dpp_VERSION_MAJOR 1)
set(dpp_VERSION_MINOR 0)
set(dpp_VERSION_PATCH 0)
set(dpp_VERSION "${dpp_VERSION_MAJOR}.${dpp_VERSION_MINOR}.${dpp_VERSION_PATCH}")

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
option(DPP_WITH_DEBUG "Build debug stuff"           OFF )
option(DPP_WITH_DOC   "Build the dpp documentation" ON  )
option(DPP_WITH_TEST  "Build the dpp test suite"    ON  )

if (DPP_WITH_DEBUG)
  set(DPP_WITH_DEBUG 1)
else()
  set(DPP_WITH_DEBUG 0)
endif()

#-----------------------------------------------------------------------
# Find external packages needed to build
#

# - Boost
set(Boost_COMPONENTS program_options)
find_package(Boost 1.47 QUIET COMPONENTS ${Boost_COMPONENTS} NO_MODULE)
find_package(Boost 1.47 REQUIRED ${Boost_COMPONENTS})

# - datatools
find_package(datatools 5.0.0 REQUIRED)

# - cuts
find_package(cuts 4.0.0 REQUIRED)

# - brio
find_package(brio 4.0.0 REQUIRED)

# - mygsl
find_package(mygsl 4.0.0 REQUIRED)

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory.
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(DPP_FRAMEWORK_NAME dpp)
  set(DPP_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${DPP_FRAMEWORK_NAME}.framework/Versions/${dpp_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)

#-----------------------------------------------------------------------
# Build the executables as required
#
add_subdirectory(programs)

if(DPP_WITH_DOC)
  set (DPP_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set (DPP_WITH_DOC 0)
endif()

if(DPP_WITH_TEST)
  set (DPP_WITH_TEST 1)
  enable_testing()
  add_subdirectory(testing)
else()
  set (DPP_WITH_TEST 0)
endif()

#----------------------------------------------------------------
# - Install public resources :

# - Examples :
set(_dpp_examples_FULL_LOCAL_PATH
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
include(DppCPack)

# - end of top level CMakeLists.txt for dpp project.
