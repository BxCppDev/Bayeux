###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the mygsl library.
##
## mygsl is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## mygsl is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with mygsl.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# setup version numbers
set ( mygsl_VERSION_MAJOR 3 )
set ( mygsl_VERSION_MINOR 2 )
set ( mygsl_PATCH_VERSION 1 )
set ( mygsl_VERSION_STR "${mygsl_VERSION_MAJOR}.${mygsl_VERSION_MINOR}.${mygsl_PATCH_VERSION}" )
math ( EXPR mygsl_VERSION_NUMERIC "${mygsl_VERSION_MAJOR}*100000+${mygsl_VERSION_MINOR}*100+${mygsl_PATCH_VERSION}" )

message(STATUS "${PROJECT_NAME}: Version major number : ${mygsl_VERSION_MAJOR}")
message(STATUS "${PROJECT_NAME}: Version minor number : ${mygsl_VERSION_MINOR}")
message(STATUS "${PROJECT_NAME}: Version patch number : ${mygsl_PATCH_VERSION}")
message(STATUS "${PROJECT_NAME}: Version              : \"${mygsl_VERSION_STR}\"")
message(STATUS "${PROJECT_NAME}: Version (numeric)    : ${mygsl_VERSION_NUMERIC}")

# Set the default build type to release
if ( NOT CMAKE_BUILD_TYPE )
    set ( CMAKE_BUILD_TYPE Release
          CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif ()

# Add an option for choosing the build type (shared or static)
if ( NOT BUILD_SHARED_LIBS )
    set ( BUILD_SHARED_LIBS TRUE
          CACHE BOOL "TRUE to build mygsl as a shared library, FALSE to build it as a static library."
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
