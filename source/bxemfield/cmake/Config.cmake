###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the emfield library.
##
## emfield is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## emfield is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with emfield.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# setup version numbers
set ( emfield_VERSION_MAJOR 0 )
set ( emfield_VERSION_MINOR 2 )
set ( emfield_PATCH_VERSION 0 )
set ( emfield_VERSION_STR "${emfield_VERSION_MAJOR}.${emfield_VERSION_MINOR}.${emfield_PATCH_VERSION}" )
math ( EXPR emfield_VERSION_NUMERIC "${emfield_VERSION_MAJOR}*100000+${emfield_VERSION_MINOR}*100+${emfield_PATCH_VERSION}" )

message(STATUS "${PROJECT_NAME}: Version major number : ${emfield_VERSION_MAJOR}")
message(STATUS "${PROJECT_NAME}: Version minor number : ${emfield_VERSION_MINOR}")
message(STATUS "${PROJECT_NAME}: Version patch number : ${emfield_PATCH_VERSION}")
message(STATUS "${PROJECT_NAME}: Version              : \"${emfield_VERSION_STR}\"")
message(STATUS "${PROJECT_NAME}: Version (numeric)    : ${emfield_VERSION_NUMERIC}")

# Set the default build type to release
if ( NOT CMAKE_BUILD_TYPE )
    set ( CMAKE_BUILD_TYPE Release
          CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif ()

# Add an option for choosing the build type (shared or static)
if ( NOT BUILD_SHARED_LIBS )
    set ( BUILD_SHARED_LIBS TRUE
          CACHE BOOL "TRUE to build emfield as a shared library, FALSE to build it as a static library."
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
