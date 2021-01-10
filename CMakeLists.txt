# - Top level CMake script for Bayeux
#

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
# Copyright 2012,2021 François Mauger <mauger@lpccaen.in2p3.fr>
# Copyright 2012,2021 Université de Caen Normandie
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
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Bayeux)

# - Load Custom Modules
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

set(_bx_CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH})
if (_bx_CMAKE_MODULE_PATH)
  list(INSERT CMAKE_MODULE_PATH 0 ${_bx_CMAKE_MODULE_PATH})
endif()

# - Version number management utilities:
include(BxVersionManager)

# - Versioning only giving major and minor. Patch number is
#   automatically deduced from an external file.
bx_version_set(Bayeux 3 4)
message(STATUS "[info] Project version = '${PROJECT_VERSION}'")

#-----------------------------------------------------------------------
# Common LPC utilities
#
include(LPCCMakeSettings)
include(CMakeDependentOption)
set(Bayeux_VERSION_REVISION 0)

# Subversion_DIRECTORY_IS_WC(${PROJECT_SOURCE_DIR} Bayeux)
# if(Bayeux_DIRECTORY_IS_WC)
#   Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Bayeux)
#   set(Bayeux_VERSION_REVISION ${Bayeux_WC_REVISION})
# endif()

# - Write compiler workaround header for features:
#    cxx_thread_local           - OS X doesn't currently provide this
#    cxx_attribute_deprecated   - C++14, but easy workaround for lesser standards
include(WriteCompilerDetectionHeader)
write_compiler_detection_header(
  FILE "${BAYEUX_BUILD_INCLUDEDIR}/bayeux/datatools/compiler_detection.h"
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
set(Bayeux_WITH_DEVELOPER_TOOLS 0)
if(BAYEUX_WITH_DEVELOPER_TOOLS)
  set(Bayeux_WITH_DEVELOPER_TOOLS 1)
endif()

#-----------------------------------------------------------------------
# Options for Bayeux modules
include(BayeuxModuleOptions)

# - Check if some ROOT dictionary is generated
set(Bayeux_WITH_ROOT 0)
set(Bayeux_WITH_ROOTDICT 0)
if(Bayeux_WITH_BRIO)
  set(Bayeux_WITH_ROOT 1)
  set(Bayeux_WITH_ROOTDICT 1)
endif()

set(Bayeux_WITH_BXDECAY0 0)
if(Bayeux_WITH_GENBB)
  option(BAYEUX_WITH_BXDECAY0 "Use the BxDecay0 library" ON)
  if (BAYEUX_WITH_BXDECAY0)
    set(Bayeux_WITH_BXDECAY0 1)
  endif()
endif()

#-----------------------------------------------------------------------
# Option for enabling Qt GUI components
#
set(Bayeux_WITH_QT 0)
option(BAYEUX_WITH_QT_GUI "Build Qt-based GUI components" OFF)
mark_as_advanced(BAYEUX_WITH_QT_GUI)
set(Bayeux_WITH_QT_GUI 0)
if (BAYEUX_WITH_QT_GUI)
  set(Bayeux_WITH_QT 1)
  set(Bayeux_WITH_QT_GUI 1)
endif()
# set(Bayeux_WITH_QT_SVG 0)
# if (Bayeux_WITH_QT_GUI)
#   set(Bayeux_WITH_QT_SVG 1)
# endif()
# option(BAYEUX_WITH_QT_SVG "Build Qt-based SVG components" OFF)
# mark_as_advanced(BAYEUX_WITH_QT_SVG)
# if (BAYEUX_WITH_QT_SVG)
#   set(Bayeux_WITH_QT_SVG 1)
# endif()

#-----------------------------------------------------------------------
# Option for enabling UNICODE symbols
#
set(Bayeux_WITH_UNICODE_SYMBOLS 0)
option(BAYEUX_WITH_UNICODE_SYMBOLS "Bayeux supports UNICODE symbols" ON)
mark_as_advanced(BAYEUX_WITH_UNICODE_SYMBOLS)
if (BAYEUX_WITH_UNICODE_SYMBOLS)
  set(Bayeux_WITH_UNICODE_SYMBOLS 1)
endif()

#-----------------------------------------------------------------------
# Option for enabling ANSI colors
#
set(Bayeux_WITH_ANSI_COLORS 0)
option(BAYEUX_WITH_ANSI_COLORS "Bayeux supports ANSI colors" ON)
mark_as_advanced(BAYEUX_WITH_ANSI_COLORS)
if (BAYEUX_WITH_ANSI_COLORS)
  set(Bayeux_WITH_ANSI_COLORS 1)
endif()

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
option(BAYEUX_WITH_DOCS "Build Bayeux documentation products" ON)
cmake_dependent_option(BAYEUX_WITH_DOCS_OCD "Build Plugin Module documentation" OFF "BAYEUX_WITH_DOCS" OFF)
mark_as_advanced(BAYEUX_WITH_DOCS BAYEUX_WITH_DOCS_OCD)
set(Bayeux_WITH_DOCS 0)
set(Bayeux_WITH_DOCS_OCD 0)
if(BAYEUX_WITH_DOCS)
  set(Bayeux_WITH_DOCS 1)
  if(BAYEUX_WITH_DOCS_OCD)
    set(Bayeux_WITH_DOCS_OCD 1)
  endif()
endif()

#-----------------------------------------------------------------------
include(BayeuxDependencies)

#-----------------------------------------------------------------------
# Build the subdirectories
#
add_subdirectory(source)
add_subdirectory(doc)

#-----------------------------------------------------------------------
# Install common resources
install(DIRECTORY ${BAYEUX_BUILD_DATAROOTDIR}/${BAYEUX_TAG}
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

#-----------------------------------------------------------------------
# Install CMake scripts if needed
# ${CMAKE_SOURCE_DIR}/cmake/FindBoost.cmake
# ${CMAKE_SOURCE_DIR}/cmake/FindReadline.cmake
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/FindBoost.cmake)
  install(FILES
    ${CMAKE_SOURCE_DIR}/cmake/FindBoost.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${BAYEUX_TAG}/cmake/
    )
endif()

# - end
