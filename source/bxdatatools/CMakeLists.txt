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


# - Fixup default install prefix on Mac
if(APPLE AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/Library/Frameworks" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif()

# - Load custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(InstallDirs)

#-----------------------------------------------------------------------
# Build options
#
#option(DATATOOLS_WITH_DEBUG "Build debug code" OFF)
option(DATATOOLS_WITH_BIO   "Build auxiliary Boost I/O library" ON)
option(DATATOOLS_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" ON)
option(DATATOOLS_WITH_DOC   "Build the datatools documentation" ON)
option(DATATOOLS_WITH_TEST  "Build the datatools test suite"    ON)

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - Boost
set(Boost_COMPONENTS iostreams serialization filesystem system program_options)
find_package(Boost 1.47 QUIET COMPONENTS ${Boost_COMPONENTS} NO_MODULE)
find_package(Boost 1.47 REQUIRED ${Boost_COMPONENTS})

# - CLHEP
find_package(CLHEP 2.1.0.1 REQUIRED)

# - CAMP
if(DATATOOLS_WITH_REFLECTION)
  find_package(CAMP 0.7.1 REQUIRED)
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
add_subdirectory(programs)

if(DATATOOLS_WITH_DOC)
  add_subdirectory(doc)
endif()

if(DATATOOLS_WITH_TEST)
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
  foreach(_ebv BIO REFLECTION)
    if(DATATOOLS_WITH_${_ebv})
      set(DATATOOLS_WITH_${_ebv} 1)
    else()
      set(DATATOOLS_WITH_${_ebv} 0)
    endif()
  endforeach()

  set(_ld_library_path_env "LD_LIBRARY_PATH")
  if(APPLE)
    set(_ld_library_path_env "DYLD_LIBRARY_PATH")
  endif()

  configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/datatools-config.in
    ${PROJECT_BINARY_DIR}/cmake/datatools-config
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/datatools-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS
      OWNER_READ OWNER_EXECUTE
    	GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/datatools_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
    )
endif()

#-----------------------------------------------------------------------
# Package
#
include(DatatoolsCPack)

# - end of top level CMakeLists.txt for datatools project.
