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
option(BAYEUX_FORCE_CXX_ALL_WARNINGS "Enable all C++ compiler warnings" ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|(Apple)+Clang")
  if (BAYEUX_FORCE_CXX_ALL_WARNINGS)
    set(BAYEUX_CXX_FLAGS "-W -Wall")
  endif()

  # On clang, need larger template depth
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(BAYEUX_CXX_FLAGS "${Bayeux_CXX_FLAGS} -ftemplate-depth=512")
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  # Needs more testing
  set(BAYEUX_CXX_FLAGS "-w2")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BAYEUX_CXX_FLAGS}")

#-----------------------------------------------------------------------
# Optional IWYU support
option(BAYEUX_WITH_IWYU_CHECK "Run include-what-you-use on Bayeux sources" OFF)
if(BAYEUX_WITH_IWYU_CHECK)
  find_program(IWYU_EXECUTABLE
    NAMES include-what-you use iwyu
    DOC "Path to include-what-you-use-executable"
    )
endif()

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

# Resources directories based on name-version subdirs
set(BAYEUX_TAG "Bayeux-${Bayeux_VERSION}")
set(BAYEUX_RESOURCE_DIR "${BAYEUX_TAG}/resources")
set(BAYEUX_EXAMPLE_DIR "${BAYEUX_TAG}/examples")
set(BAYEUX_DOCUMENTATION_DIR "${BAYEUX_TAG}/Documentation")


#-----------------------------------------------------------------------
# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
#   We allow a parent project to change the output root so that Bayeux
#   can be embedded in another project
if(NOT BAYEUX_EMBEDDED OR NOT BAYEUX_BUILDPRODUCT_DIR)
  set(BAYEUX_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
endif()

# - Note that at present, this layout only supports single mode
# generators like make/ninja...
file(RELATIVE_PATH BAYEUX_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}"  "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${BAYEUX_TAG}")
file(RELATIVE_PATH BAYEUX_CMAKEDIR_TO_INCLUDEDIR "${CMAKE_INSTALL_FULL_CMAKEDIR}/${BAYEUX_TAG}"  "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")


#-----------------------------------------------------------------------
# Option for building/installing developer tools
#
option(BAYEUX_WITH_DEVELOPER_TOOLS "Build/install developer tools" ON)
mark_as_advanced(BAYEUX_WITH_DEVELOPER_TOOLS)

#-----------------------------------------------------------------------
# Use implicit Bayeux init/fini instruction
#
option(BAYEUX_WITH_IMPLICIT_INIT_FINI "Build implicit init/fini code for Bayeux" OFF)
mark_as_advanced(BAYEUX_WITH_IMPLICIT_INIT_FINI)

# - Options for enabling Bayeux library modules
option(BAYEUX_WITH_BRIO       "Build BAYEUX/brio library module"       ON)
option(BAYEUX_WITH_CUTS       "Build BAYEUX/cuts library module"       ON)
option(BAYEUX_WITH_MYGSL      "Build BAYEUX/mygsl library module"      ON)
option(BAYEUX_WITH_DPP        "Build BAYEUX/dpp library module"        ON)
option(BAYEUX_WITH_MATERIALS  "Build BAYEUX/materials library module"  ON)
option(BAYEUX_WITH_GEOMTOOLS  "Build BAYEUX/geomtools library module"  ON)
option(BAYEUX_WITH_EMFIELD    "Build BAYEUX/emfield library module"    ON)
option(BAYEUX_WITH_GENBB_HELP "Build BAYEUX/genbb_help library module" ON)
option(BAYEUX_WITH_GENVTX     "Build BAYEUX/genvtx library module"     ON)
option(BAYEUX_WITH_MCTOOLS    "Build BAYEUX/mctools library module"    ON)
option(BAYEUX_WITH_LAHAGUE    "Build BAYEUX/lahague library module"    OFF)
mark_as_advanced(
  BAYEUX_BUILD_LAHAGUE_MODULE
  BAYEUX_WITH_BRIO
  BAYEUX_WITH_CUTS
  BAYEUX_WITH_MYGSL
  BAYEUX_WITH_DPP
  BAYEUX_WITH_MATERIALS
  BAYEUX_WITH_GEOMTOOLS
  BAYEUX_WITH_EMFIELD
  BAYEUX_WITH_GENBB_HELP
  BAYEUX_WITH_GENVTX
  BAYEUX_WITH_MCTOOLS
  )

#-----------------------------------------------------------------------
# Transitional option for backward compatibility with old ROOT versions (<6)
#
option(BAYEUX_USE_LEGACY_ROOT "Bayeux is built using ROOT version (<6.X)" OFF)
mark_as_advanced(BAYEUX_USE_LEGACY_ROOT)

#-----------------------------------------------------------------------
# Option for enabling Geant4 extension library module
#
option(BAYEUX_WITH_GEANT4_MODULE "Build Bayeux/mctools Geant4 Monte-Carlo module" ON)
mark_as_advanced(BAYEUX_WITH_GEANT4_MODULE)

#-----------------------------------------------------------------------
# Option for enabling MCNP extension library module
#
option(BAYEUX_WITH_MCNP_MODULE "Build Bayeux/mctools MCNP Monte-Carlo module" OFF)
mark_as_advanced(BAYEUX_WITH_MCNP_MODULE)

#-----------------------------------------------------------------------
# Option for enabling Qt GUI components
#
option(BAYEUX_WITH_QT_GUI "Build Qt-based GUI components" OFF)
mark_as_advanced(BAYEUX_WITH_QT_GUI)

#-----------------------------------------------------------------------
# Configure testing if required
#
option(BAYEUX_ENABLE_TESTING "Build unit testing system for Bayeux" OFF)

# - If testing required, *must* call enable_testing here!!
if(BAYEUX_ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------
# Optional for enabling documentation
#
include(CMakeDependentOption)
option(BAYEUX_WITH_DOCS "Build Bayeux documentation products" ON)
cmake_dependent_option(BAYEUX_WITH_DOCS_OCD "Build Plugin Module documentation" OFF "BAYEUX_WITH_DOCS" OFF)
mark_as_advanced(BAYEUX_WITH_DOCS BAYEUX_WITH_OCD_DOCS)

#-----------------------------------------------------------------------
# Optional for installing examples
#
option(BAYEUX_WITH_EXAMPLES "Install Bayeux with examples" ON)
mark_as_advanced(BAYEUX_WITH_EXAMPLES)

#-----------------------------------------------------------------------
# Build the subdirectories
#
add_subdirectory(source)
add_subdirectory(doc)
add_subdirectory(examples)

#-----------------------------------------------------------------------
# Install common resources
install(DIRECTORY ${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${BAYEUX_TAG}
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

#-----------------------------------------------------------------------
# Package Sources and Binaries - only if not embedded
#
if(NOT BAYEUX_EMBEDDED)
  include(BXCPack)
endif()
