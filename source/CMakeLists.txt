# - CMake script for building Bayeux from its submodules
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
# Configure build of Bayeux library
# - Packages
find_package(Boost 1.48.0 REQUIRED filesystem system serialization iostreams)
find_package(CLHEP REQUIRED)

# - Core Includes for Bayeux + Third Party
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include)
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include/bayeux)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CLHEP_INCLUDE_DIRS})

# - Configure Bayeux submodule first
set(MODULE_HEADER_ROOT "${Bayeux_BUILDPRODUCT_DIR}/include")
include(bxbayeux_module.cmake)
set(Bayeux_HEADERS ${bayeux_MODULE_HEADERS})
set(Bayeux_SOURCES ${bayeux_MODULE_SOURCES})

# - The LPC Modules
# The processing order should be from least to most dependent
# That's to ensure that any needed CMake variables are set in the correct
# order, and that ordering of source files (and hence object files)
# is correct.
set(lpcmodules datatools)
set(MODULE_HEADER_ROOT "${Bayeux_BUILDPRODUCT_DIR}/include/bayeux")
foreach(_m ${lpcmodules})
  include(bx${_m}_module.cmake)
  list(APPEND Bayeux_HEADERS ${${_m}_MODULE_HEADERS})
  list(APPEND Bayeux_SOURCES ${${_m}_MODULE_SOURCES})
endforeach()

#-----------------------------------------------------------------------
# Build/Link the library
#
add_library(bayeux SHARED ${Bayeux_HEADERS} ${Bayeux_SOURCES})
target_link_libraries(bayeux ${CMAKE_DL_LIBS})
target_link_libraries(bayeux ${Boost_LIBRARIES})

#-----------------------------------------------------------------------
# Install targets and resources
#
install(TARGETS bayeux
  EXPORT  bayeux-targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )

install(DIRECTORY ${Bayeux_BUILDPRODUCT_DIR}/include/
  DESTINATION include
  )

