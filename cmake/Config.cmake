###############################################################################
##
## Copyright (C) 2011-2012 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the Bayeux library.
##
## Bayeux is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Bayeux is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# setup version numbers
set ( Bayeux_VERSION_MAJOR 0 ) # do not remove the spaces
set ( Bayeux_VERSION_MINOR 9 ) # they are used by external scripts
set ( Bayeux_PATCH_VERSION 5 ) # to extract the version number (<100)

set ( Bayeux_VERSION_STR "${Bayeux_VERSION_MAJOR}.${Bayeux_VERSION_MINOR}.${Bayeux_PATCH_VERSION}" )
math ( EXPR Bayeux_VERSION_NUMERIC "${Bayeux_VERSION_MAJOR}*100000+${Bayeux_VERSION_MINOR}*100+${Bayeux_PATCH_VERSION}" )

message(STATUS "${PROJECT_NAME}: Version major number : ${Bayeux_VERSION_MAJOR}")
message(STATUS "${PROJECT_NAME}: Version minor number : ${Bayeux_VERSION_MINOR}")
message(STATUS "${PROJECT_NAME}: Version patch number : ${Bayeux_PATCH_VERSION}")
message(STATUS "${PROJECT_NAME}: Version              : \"${Bayeux_VERSION_STR}\"")
message(STATUS "${PROJECT_NAME}: Version (numeric)    : ${Bayeux_VERSION_NUMERIC}")

# Set the default build type to release
if ( NOT CMAKE_BUILD_TYPE )
    set ( CMAKE_BUILD_TYPE Release
          CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif ()

# Add an option for choosing the build type (shared or static)
if ( NOT BUILD_SHARED_LIBS )
    set ( BUILD_SHARED_LIBS TRUE
          CACHE BOOL "TRUE to build Bayeux as a shared library, FALSE to build it as a static library."
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
