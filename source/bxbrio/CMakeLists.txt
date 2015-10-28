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
set(brio_VERSION_MAJOR 6)
set(brio_VERSION_MINOR 0)
set(brio_VERSION_PATCH 0)
set(brio_VERSION "${brio_VERSION_MAJOR}.${brio_VERSION_MINOR}.${brio_VERSION_PATCH}")

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

if (BRIO_WITH_DEBUG)
  set (BRIO_WITH_DEBUG 1)
else()
  set (BRIO_WITH_DEBUG 0)
endif ()

#-----------------------------------------------------------------------
# Find external packages needed to build
#

# - datatools
find_package(datatools 5.0.0 REQUIRED)
#message ( STATUS "datatools_INCLUDE_DIRS=${datatools_INCLUDE_DIRS}")
#message ( STATUS "datatools_LIBRARIES=${datatools_LIBRARIES}")
#message ( STATUS "datatools_WITH_BIO=${datatools_WITH_BIO}")

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
endif ()

if(BRIO_WITH_TEST)
  enable_testing()
  add_subdirectory(testing)
endif()

#-----------------------------------------------------------------------
# Build the support files
#-----------------------------------------------------------------------
# On UNIX platforms, configure and install the shell support scripts
#
if(UNIX)
  # - Normalize exported boolean variables
  foreach(_ebv DEBUG)
    if(BRIO_WITH_${_ebv})
      set(BRIO_WITH_${_ebv} 1)
    else()
      set(BRIO_WITH_${_ebv} 0)
    endif()
  endforeach()

  set(_ld_library_path_env "LD_LIBRARY_PATH")
  if(APPLE)
    set(_ld_library_path_env "DYLD_LIBRARY_PATH")
  endif()

  configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/brio-config.in
    ${PROJECT_BINARY_DIR}/cmake/brio-config
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/brio-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS
      OWNER_READ OWNER_EXECUTE
    	GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/brio_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/brio_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/brio_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
    )

endif()

#----------------------------------------------------------------
# - Install public resources :

# - Examples :
set(_brio_examples_FULL_LOCAL_PATH
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
include(BrioCPack)

# - end of top level CMakeLists.txt for brio project.
