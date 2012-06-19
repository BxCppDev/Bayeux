# - datatools base configuration
#
# Copyright (C) 2011-2012 François Mauger, Université de Caen Basse-Normandie
# Contact: mauger@lpccaen.in2p3.fr
#
# This file is part of the datatools library.
#
# datatools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# datatools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with datatools.  If not, see <http://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------


# setup version numbers
set(datatools_VERSION_MAJOR 4)
set(datatools_VERSION_MINOR 1)
set(datatools_PATCH_VERSION 1)
set(datatools_VERSION_STR "${datatools_VERSION_MAJOR}.${datatools_VERSION_MINOR}.${datatools_PATCH_VERSION}")
math(EXPR datatools_VERSION_NUMERIC "${datatools_VERSION_MAJOR}*100000+${datatools_VERSION_MINOR}*100+${datatools_PATCH_VERSION}")

# Set the default build type to release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel.")
endif()

# Add an option for choosing the build type (shared or static)
option(BUILD_SHARED_LIBS "Build datatools as a dynamic library" ON)

# Set the default test build option to false
option(BUILD_TEST "Build datatools unit tests" OFF)

# setup MacOSX build environment if necessary
include(${CMAKE_SOURCE_DIR}/cmake/MacOSX.cmake)

# end of Config.cmake
