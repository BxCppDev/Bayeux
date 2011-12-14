###############################################################################
##
## Copyright (C) 2011 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the datatools library.
##
## datatools is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## datatools is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with datatools.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# setup version numbers
set ( datatools_VERSION_MAJOR 4 )
set ( datatools_VERSION_MINOR 0 )
set ( datatools_PATCH_VERSION 0 )
set ( datatools_VERSION_STR "${datatools_VERSION_MAJOR}.${datatools_VERSION_MINOR}.${datatools_PATCH_VERSION}" )
math ( EXPR datatools_VERSION_NUMERIC "${datatools_VERSION_MAJOR}*100000+${datatools_VERSION_MINOR}*100+${datatools_PATCH_VERSION}" )

message(STATUS "Version major number : ${datatools_VERSION_MAJOR}")
message(STATUS "Version minor number : ${datatools_VERSION_MINOR}")
message(STATUS "Version patch number : ${datatools_PATCH_VERSION}")
message(STATUS "Version              : \"${datatools_VERSION_STR}\"")
message(STATUS "Version (numeric)    : ${datatools_VERSION_NUMERIC}")

# Set the default build type to release
if ( NOT CMAKE_BUILD_TYPE )
    set ( CMAKE_BUILD_TYPE Release
          CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif ()

# Add an option for choosing the build type (shared or static)
if ( NOT BUILD_SHARED_LIBS )
    set ( BUILD_SHARED_LIBS TRUE
          CACHE BOOL "TRUE to build datatools as a shared library, FALSE to build it as a static library."
    )
endif ()

# Set the default test build option to false
if ( NOT BUILD_TEST )
    set(BUILD_TEST FALSE
        CACHE BOOL "TRUE to build the unit tests (requires the Boost Test Library), FALSE otherwise."
    )
endif()

# define install directory for miscelleneous files
if ( WIN32 AND NOT UNIX )
    set ( INSTALL_MISC_DIR . )
elseif ( UNIX)
    set ( INSTALL_MISC_DIR share/datatools )
endif ()

set (INSTALL_RESOURCES_DIR ${INSTALL_MISC_DIR}/resources )

# setup MacOSX build environment if necessary
include ( ${CMAKE_SOURCE_DIR}/cmake/MacOSX.cmake )


message ( STATUS "INSTALL_MISC_DIR      : ${INSTALL_MISC_DIR}" )
message ( STATUS "INSTALL_RESOURCES_DIR : ${INSTALL_RESOURCES_DIR}" )
