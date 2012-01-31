###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the geomtools library.
##
## geomtools is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## geomtools is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with geomtools.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# setup version numbers
set ( geomtools_VERSION_MAJOR 3 )
set ( geomtools_VERSION_MINOR 0 )
set ( geomtools_PATCH_VERSION 0 )
set ( geomtools_VERSION_STR "${geomtools_VERSION_MAJOR}.${geomtools_VERSION_MINOR}.${geomtools_PATCH_VERSION}" )
math ( EXPR geomtools_VERSION_NUMERIC "${geomtools_VERSION_MAJOR}*100000+${geomtools_VERSION_MINOR}*100+${geomtools_PATCH_VERSION}" )

message(STATUS "Version major number : ${geomtools_VERSION_MAJOR}")
message(STATUS "Version minor number : ${geomtools_VERSION_MINOR}")
message(STATUS "Version patch number : ${geomtools_PATCH_VERSION}")
message(STATUS "Version              : \"${geomtools_VERSION_STR}\"")
message(STATUS "Version (numeric)    : ${geomtools_VERSION_NUMERIC}")

# Set the default build type to release
if ( NOT CMAKE_BUILD_TYPE )
    set ( CMAKE_BUILD_TYPE Release
          CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif ()

# Add an option for choosing the build type (shared or static)
if ( NOT BUILD_SHARED_LIBS )
    set ( BUILD_SHARED_LIBS TRUE
          CACHE BOOL "TRUE to build geomtools as a shared library, FALSE to build it as a static library."
    )
endif ()

# Set the default test build option to false
if ( NOT BUILD_TEST )
    set(BUILD_TEST FALSE
        CACHE BOOL "TRUE to build the unit tests (requires the Boost Test Library), FALSE otherwise."
    )
endif()

# setup MacOSX build environment if necessary
include ( ${CMAKE_SOURCE_DIR}/cmake/MacOSX.cmake )

# end of Config.cmake
