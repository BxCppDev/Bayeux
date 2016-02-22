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
# CMake/project requirements and configuration
#
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(Bayeux VERSION "2.1.0")

# - Load Custom Modules
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

#-----------------------------------------------------------------------
# Additional SVN Revision tracking for developers
include(SVNUtilities)
set(Bayeux_VERSION_REVISION 0)

Subversion_DIRECTORY_IS_WC(${PROJECT_SOURCE_DIR} Bayeux)
if(Bayeux_DIRECTORY_IS_WC)
  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Bayeux)
  set(Bayeux_VERSION_REVISION ${Bayeux_WC_REVISION})
endif()

#-----------------------------------------------------------------------
# Compiler configuration
# - Enforce all warnings, but ensure other flags are retained
# Note that the added flags *won't* show up in the cache.
# TODO : better way to do this with the "Make Override" system?
option(Bayeux_FORCE_CXX_ALL_WARNINGS "Enable all C++ compiler warnings" ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|(Apple)+Clang")
  # Developers may need to mute warnings in order to track easily
  # error and warning messages mixed up in the output stream with
  # multi-threaded build. So we make this compiler flag an option,
  # activated by default for a production build:
  if (Bayeux_FORCE_CXX_ALL_WARNINGS)
    set(Bayeux_CXX_FLAGS "-W -Wall")
    ### set(Bayeux_CXX_FLAGS "-W -Wall -Woverloaded-virtual")
  endif()

  # On clang, need larger template depth
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(Bayeux_CXX_FLAGS "${Bayeux_CXX_FLAGS} -ftemplate-depth=512")
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  # Needs more testing
  set(Bayeux_CXX_FLAGS "-w2")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Bayeux_CXX_FLAGS}")

#-----------------------------------------------------------------------
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to RelWithDebInfo
#
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    # Default to a RelWithDebInfo build if nothing else...
    set(CMAKE_BUILD_TYPE RelWithDebInfo
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo MinSizeRel."
      FORCE
      )
  else()
    # Force to the cache, but use existing value.
    set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo MinSizeRel."
      FORCE
      )
  endif()
endif()

#-----------------------------------------------------------------------
# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
include(GNUInstallDirs)

# Validate that certain paths are relative, otherwise relocation may fail
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# Add a path for CMake config files
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
set(CMAKE_INSTALL_FULL_CMAKEDIR ${CMAKE_INSTALL_FULL_LIBDIR}/cmake)


#-----------------------------------------------------------------------
# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
#   We allow a parent project to change the output root so that Bayeux
#   can be embedded in another project
if(NOT Bayeux_EMBEDDED OR NOT Bayeux_BUILDPRODUCT_DIR)
  set(Bayeux_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
endif()

# - Note that at present, this layout only supports single mode
# generators like make/ninja...
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
option(Bayeux_BUILD_IMPLICIT_INIT_FINI "Build implicit init/fini code for Bayeux" OFF)
mark_as_advanced(Bayeux_BUILD_IMPLICIT_INIT_FINI)

if (Bayeux_BUILD_IMPLICIT_INIT_FINI)
  set(BAYEUX_WITH_IMPLICIT_INIT_FINI 1)
else()
  set(BAYEUX_WITH_IMPLICIT_INIT_FINI 0)
endif()


# - Options for enabling Bayeux library modules
option(Bayeux_WITH_BRIO       "Build Bayeux/brio library module"       ON)
option(Bayeux_WITH_CUTS       "Build Bayeux/cuts library module"       ON)
option(Bayeux_WITH_MYGSL      "Build Bayeux/mygsl library module"      ON)
option(Bayeux_WITH_DPP        "Build Bayeux/dpp library module"        ON)
option(Bayeux_WITH_MATERIALS  "Build Bayeux/materials library module"  ON)
option(Bayeux_WITH_GEOMTOOLS  "Build Bayeux/geomtools library module"  ON)
option(Bayeux_WITH_EMFIELD    "Build Bayeux/emfield library module"    ON)
option(Bayeux_WITH_GENBB_HELP "Build Bayeux/genbb_help library module" ON)
option(Bayeux_WITH_GENVTX     "Build Bayeux/genvtx library module"     ON)
option(Bayeux_WITH_MCTOOLS    "Build Bayeux/mctools library module"    ON)
option(Bayeux_WITH_LAHAGUE    "Build Bayeux/lahague library module"    OFF)
mark_as_advanced(Bayeux_BUILD_LAHAGUE_MODULE)
mark_as_advanced(Bayeux_WITH_BRIO)
mark_as_advanced(Bayeux_WITH_CUTS)
mark_as_advanced(Bayeux_WITH_MYGSL)
mark_as_advanced(Bayeux_WITH_DPP)
mark_as_advanced(Bayeux_WITH_MATERIALS)
mark_as_advanced(Bayeux_WITH_GEOMTOOLS)
mark_as_advanced(Bayeux_WITH_EMFIELD)
mark_as_advanced(Bayeux_WITH_GENBB_HELP)
mark_as_advanced(Bayeux_WITH_GENVTX)
mark_as_advanced(Bayeux_WITH_MCTOOLS)

#-----------------------------------------------------------------------
# Transitional option for backward compatibility with old ROOT versions (<6)
#
option(Bayeux_USE_LEGACY_ROOT "Bayeux is built using ROOT version (<6.X)" OFF)
mark_as_advanced(Bayeux_USE_LEGACY_ROOT)

#-----------------------------------------------------------------------
# Option for enabling Geant4 extension library module
#
option(Bayeux_BUILD_GEANT4_MODULE "Build Bayeux/mctools Geant4 Monte-Carlo module" ON)
mark_as_advanced(Bayeux_BUILD_GEANT4_MODULE)

#-----------------------------------------------------------------------
# Option for enabling MCNP extension library module
#
option(Bayeux_BUILD_MCNP_MODULE "Build Bayeux/mctools MCNP Monte-Carlo module" OFF)
mark_as_advanced(Bayeux_BUILD_MCNP_MODULE)

#-----------------------------------------------------------------------
# Option for enabling Qt GUI components
#
option(Bayeux_BUILD_QT_GUI "Build Qt-based GUI components" OFF)
mark_as_advanced(Bayeux_BUILD_QT_GUI)
if (Bayeux_BUILD_QT_GUI)
  set(BAYEUX_WITH_QT_GUI 1)
else()
  set(BAYEUX_WITH_QT_GUI 0)
endif()

#-----------------------------------------------------------------------
# Configure testing if required
#
option(Bayeux_ENABLE_TESTING "Build unit testing system for Bayeux" OFF)
mark_as_advanced(Bayeux_ENABLE_TESTING)

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

option(Bayeux_BUILD_DOCS "Build Bayeux documentation products" ON)
mark_as_advanced(Bayeux_BUILD_DOCS)

if(Bayeux_BUILD_DOCS)
  set(BAYEUX_WITH_DOCS 1)
else()
  set(BAYEUX_WITH_DOCS 0)
endif()

if (Bayeux_BUILD_DOCS)
  #-----------------------------------------------------------------------
  # Build OCD documentation for Bayeux's classes
  option(Bayeux_BUILD_OCD_DOCS "Build OCD documentation" ON)
  mark_as_advanced(Bayeux_BUILD_OCD_DOCS)

  if(Bayeux_BUILD_OCD_DOCS)
    set(BAYEUX_WITH_OCD_DOCS 1)
    set(BAYEUX_WITH_DOCS 1)
  else()
    set(BAYEUX_WITH_OCD_DOCS 0)
  endif()

endif()

#-----------------------------------------------------------------------
# Optional for installing examples
#
option(Bayeux_WITH_EXAMPLES "Install Bayeux with examples" ON)
mark_as_advanced(Bayeux_WITH_EXAMPLES)

if(Bayeux_WITH_EXAMPLES)
  set(BAYEUX_WITH_EXAMPLES 1)
else()
  set(BAYEUX_WITH_EXAMPLES 0)
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
