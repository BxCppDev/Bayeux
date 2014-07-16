# - Top level CMake script for Bayeux
#

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
#
# This file is part of Bayeux.
#
# Bayeux is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Bayeux is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "Bayeux requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Release prior to calling PROJECT()
#
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

#-----------------------------------------------------------------------
# CMake/project requirements and configuration
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# If Policy CMP0022 exists, use OLD because Bayeux still needs
# to support versions of CMake < 2.8.12.
# This relates to LINK_INTERFACE_LIBRARIES and can be removed once
# minimum required CMake version reaches 2.8.12
if(POLICY CMP0022)
  cmake_policy(SET CMP0022 OLD)
endif()

project(Bayeux)

# - Load Custom Modules
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)
include(SVNUtilities)

# - Versioning
set(Bayeux_VERSION_MAJOR 1)
set(Bayeux_VERSION_MINOR 1)
set(Bayeux_VERSION_PATCH 0)
set(Bayeux_VERSION "${Bayeux_VERSION_MAJOR}.${Bayeux_VERSION_MINOR}.${Bayeux_VERSION_PATCH}")

# - Enforce all warnings, but ensure other flags are retained
# Note that the added flags *won't* show up in the cache.
# TODO : better way to do this with the "Make Override" system?
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Developers may need to mute warnings in order to track easily
  # error and warning messages mixed up in the output stream with
  # multi-threaded build. So we make this compiler flag an option,
  # activated by default for a production build:
  option(Bayeux_FORCE_CXX_ALL_WARNINGS "Force all C++ compiler warnings" ON)
  if (Bayeux_FORCE_CXX_ALL_WARNINGS)
    set(Bayeux_CXX_FLAGS "-W -Wall")
    ### set(Bayeux_CXX_FLAGS "-W -Wall -Woverloaded-virtual")
  endif()

  # On clang, need larger template depth
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(Bayeux_CXX_FLAGS "${Bayeux_CXX_FLAGS} -ftemplate-depth=512")
  endif()
elseif(CMAKE_CXX_COMPILER MATCHES "icpc.*|icc.*")
  # Needs more testing
  set(Bayeux_CXX_FLAGS "-w2")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Bayeux_CXX_FLAGS}")


# - SVN Versioning, if available
set(Bayeux_VERSION_REVISION 0)

Subversion_DIRECTORY_IS_WC(${PROJECT_SOURCE_DIR} Bayeux)

if(Bayeux_DIRECTORY_IS_WC)
  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Bayeux)
  set(Bayeux_VERSION_REVISION ${Bayeux_WC_REVISION})
endif()

# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
# Validate that certain paths are relative, otherwise relocation may fail
include(GNUInstallDirs)
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# Add a path for CMake config files
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
set(CMAKE_INSTALL_FULL_CMAKEDIR ${CMAKE_INSTALL_FULL_LIBDIR}/cmake)


# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
#   We allow a parent project to change the output root so that Bayeux
#   can be embedded in another project
if(NOT Bayeux_EMBEDDED OR NOT Bayeux_BUILDPRODUCT_DIR)
  set(Bayeux_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
endif()

file(RELATIVE_PATH Bayeux_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}"  "${CMAKE_INSTALL_FULL_DATAROOTDIR}/Bayeux-${Bayeux_VERSION}")
file(RELATIVE_PATH Bayeux_CMAKEDIR_TO_INCLUDEDIR "${CMAKE_INSTALL_FULL_CMAKEDIR}/Bayeux-${Bayeux_VERSION}"  "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")

#-----------------------------------------------------------------------
# Option for building/installing developer tools
#
option(Bayeux_BUILD_DEVELOPER_TOOLS "Build/install developer tools" ON)
mark_as_advanced(Bayeux_BUILD_DEVELOPER_TOOLS)

#-----------------------------------------------------------------------
# Use implicit Bayeux init/fini instruction
#
option(BAYEUX_WITH_IMPLICIT_INIT_FINI "Build implicit init/fini code for Bayeux" OFF)
mark_as_advanced(BAYEUX_WITH_IMPLICIT_INIT_FINI)

#-----------------------------------------------------------------------
# Option for enabling Geant4 module
#
if(NOT DEFINED Bayeux_WITH_GEANT4)
  set(Bayeux_WITH_GEANT4 ON CACHE BOOL "Build Geant4 extension module")
endif()

#-----------------------------------------------------------------------
# Configure testing if required
#
if(NOT DEFINED Bayeux_ENABLE_TESTING)
  set(Bayeux_ENABLE_TESTING ON CACHE BOOL "Build unit testing system for Bayeux")
endif()

if(Bayeux_ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------
# - Parent subdirectory of all Bayeux data files: define the Bayeux_TAG variable:
set(Bayeux_TAG "Bayeux-${Bayeux_VERSION}")

#-----------------------------------------------------------------------
# - Resource subdirectory: define the Bayeux_RESOURCE_DIR variable:
set(Bayeux_RESOURCE_DIR "${Bayeux_TAG}/resources")

#-----------------------------------------------------------------------
# - Example subdirectory: define the Bayeux_EXAMPLE_DIR variable:
set(Bayeux_EXAMPLE_DIR "${Bayeux_TAG}/examples")

#-----------------------------------------------------------------------
# - Documentation subdirectory: define the Bayeux_DOCUMENTATION_DIR:
set(Bayeux_DOCUMENTATION_DIR "${Bayeux_TAG}/Documentation")

#-----------------------------------------------------------------------
# Optional for enabling documentation
#
set(Bayeux_DOCEXPORT_DIR "${PROJECT_BINARY_DIR}/doc")
if(NOT DEFINED Bayeux_BUILD_DOCS)
  set(Bayeux_BUILD_DOCS ON CACHE BOOL "Build Bayeux documentation products")
endif()

if(NOT Bayeux_BUILD_DOCS)
  set(BAYEUX_WITH_DOCS 0)
else()
  set(BAYEUX_WITH_DOCS 1)
endif()

#-----------------------------------------------------------------------
# Optional for installing examples
#
if(NOT DEFINED Bayeux_WITH_EXAMPLES)
  set(Bayeux_WITH_EXAMPLES ON CACHE BOOL "Install Bayeux with examples")
endif()
if(NOT Bayeux_WITH_EXAMPLES)
  set(BAYEUX_WITH_EXAMPLES 0)
else()
  set(BAYEUX_WITH_EXAMPLES 1)
endif()

#-----------------------------------------------------------------------
# Build the subdirectories
#
add_subdirectory(source)
add_subdirectory(doc)

if(Bayeux_ENABLE_TESTING)
  add_subdirectory(examples)
endif()

#-----------------------------------------------------------------------
# Install common resources
install(DIRECTORY ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${Bayeux_TAG}
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

#-----------------------------------------------------------------------
# Package Sources and Binaries - only if not embedded
#
if(NOT Bayeux_EMBEDDED)
  include(BXCPack)
endif()
