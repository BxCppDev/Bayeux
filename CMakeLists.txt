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
# Include main settings file
#
include(BXCMakeSettings)

# - Write compiler workaround header for features:
#    cxx_thread_local           - OS X doesn't currently provide this
#    cxx_attribute_deprecated   - C++14, but easy workaround for lesser standards
include(WriteCompilerDetectionHeader)
write_compiler_detection_header(
  FILE "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_INCLUDEDIR}/bayeux/datatools/compiler_detection.h"
  PREFIX "DT"
  EPILOG "#include \"datatools/compiler_macros.h\""
  COMPILERS GNU Clang AppleClang MSVC
  FEATURES cxx_thread_local cxx_attribute_deprecated
  )

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
option(BAYEUX_WITH_BRIO       "Build Bayeux/brio library module"       ON)
option(BAYEUX_WITH_CUTS       "Build Bayeux/cuts library module"       ON)
option(BAYEUX_WITH_MYGSL      "Build Bayeux/mygsl library module"      ON)
option(BAYEUX_WITH_DPP        "Build Bayeux/dpp library module"        ON)
option(BAYEUX_WITH_MATERIALS  "Build Bayeux/materials library module"  ON)
option(BAYEUX_WITH_GEOMTOOLS  "Build Bayeux/geomtools library module"  ON)
option(BAYEUX_WITH_EMFIELD    "Build Bayeux/emfield library module"    ON)
option(BAYEUX_WITH_GENBB_HELP "Build Bayeux/genbb_help library module" ON)
option(BAYEUX_WITH_GENVTX     "Build Bayeux/genvtx library module"     ON)
option(BAYEUX_WITH_MCTOOLS    "Build Bayeux/mctools library module"    ON)
option(BAYEUX_WITH_LAHAGUE    "Build Bayeux/lahague library module"    OFF)
mark_as_advanced(
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
  BAYEUX_WITH_LAHAGUE
  )

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
mark_as_advanced(BAYEUX_WITH_DOCS BAYEUX_WITH_DOCS_OCD)

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
include(BXCPack)

