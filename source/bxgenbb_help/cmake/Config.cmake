###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the genbb_help library.
##
## genbb_help is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## genbb_help is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with genbb_help.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# setup version numbers
set ( genbb_help_VERSION_MAJOR 4 )
set ( genbb_help_VERSION_MINOR 1 )
set ( genbb_help_PATCH_VERSION 1 )
set ( genbb_help_VERSION_STR "${genbb_help_VERSION_MAJOR}.${genbb_help_VERSION_MINOR}.${genbb_help_PATCH_VERSION}" )
math ( EXPR genbb_help_VERSION_NUMERIC "${genbb_help_VERSION_MAJOR}*100000+${genbb_help_VERSION_MINOR}*100+${genbb_help_PATCH_VERSION}" )

message(STATUS "Version major number : ${genbb_help_VERSION_MAJOR}")
message(STATUS "Version minor number : ${genbb_help_VERSION_MINOR}")
message(STATUS "Version patch number : ${genbb_help_PATCH_VERSION}")
message(STATUS "Version              : \"${genbb_help_VERSION_STR}\"")
message(STATUS "Version (numeric)    : ${genbb_help_VERSION_NUMERIC}")

# Set the default build type to release
if ( NOT CMAKE_BUILD_TYPE )
    set ( CMAKE_BUILD_TYPE Release
          CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif ()

# Add an option for choosing the build type (shared or static)
if ( NOT BUILD_SHARED_LIBS )
    set ( BUILD_SHARED_LIBS TRUE
          CACHE BOOL "TRUE to build genbb_help as a shared library, FALSE to build it as a static library."
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
