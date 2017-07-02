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
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(Bayeux VERSION "3.1.0")

# - Load Custom Modules
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

#-----------------------------------------------------------------------
# Common LPC utilities
#
include(LPCCMakeSettings)

# - SVN revision number for dev builds
include(LPCSVNUtilities)
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

#-----------------------------------------------------------------------
# Options for Bayeux modules
include(BayeuxModuleOptions)

# - Check if some ROOT dictionary is generated
set(Bayeux_WITH_ROOT 0)
set(Bayeux_WITH_ROOTDICT 0)
if(Bayeux_WITH_BRIO)
  set(Bayeux_WITH_ROOT 1)
  set(BAYEUX_ROOT_MIN_VERSION "6.08.00")
  set(Bayeux_WITH_ROOTDICT 1)
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
  # Minimal system Qt5 version from Ubuntu 14.04: 5.2.1
  # version for Ubuntu 16.04: 5.5.1
  # version for Linuxbrew 5.8.0 (June 2017)
  set(BAYEUX_QT5_MIN_VERSION "5.8.0")
  set(Bayeux_WITH_QT_GUI 1)
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
include(CMakeDependentOption)
option(BAYEUX_WITH_DOCS "Build Bayeux documentation products" ON)
cmake_dependent_option(BAYEUX_WITH_DOCS_OCD "Build Plugin Module documentation" OFF "BAYEUX_WITH_DOCS" OFF)
mark_as_advanced(BAYEUX_WITH_DOCS BAYEUX_WITH_DOCS_OCD)

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

# - end
