# - CMake script for building/running Bayeux examples as tests
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

if(NOT BAYEUX_ENABLE_TESTING)
  return()
endif()

include(ExternalProject)

#-----------------------------------------------------------------------
# Basic example
ExternalProject_Add(Bayeux_BasicVersionCheck
  DEPENDS Bayeux
  LIST_SEPARATOR @
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/examples/bxversioning
  DOWNLOAD_COMMAND ""
  CONFIGURE_COMMAND ${CMAKE_COMMAND}
  -DCMAKE_PREFIX_PATH=${BAYEUX_BUILDPRODUCT_DIR}
  <SOURCE_DIR>
  BUILD_COMMAND     make VERBOSE=1
  TEST_COMMAND      make test
  INSTALL_COMMAND   ""
  )

#-----------------------------------------------------------------------
# Basic example
ExternalProject_Add(Bayeux_BasicModulesCheck
  DEPENDS Bayeux
  LIST_SEPARATOR @
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/examples/bxexample01
  DOWNLOAD_COMMAND ""
  CONFIGURE_COMMAND ${CMAKE_COMMAND}
  -DCMAKE_PREFIX_PATH=${BAYEUX_BUILDPRODUCT_DIR}
  <SOURCE_DIR>
  BUILD_COMMAND     make VERBOSE=1
  TEST_COMMAND      make test
  INSTALL_COMMAND   ""
  )

#-----------------------------------------------------------------------
# MCTools/Geant4 test
if(Bayeux_BUILD_GEANT4_MODULE)
  ExternalProject_Add(Bayeux_MCToolsGeant4Check
    DEPENDS Bayeux Bayeux_mctools_geant4
    LIST_SEPARATOR @
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/examples/bxmctools_geant4
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ${CMAKE_COMMAND}
    -DCMAKE_PREFIX_PATH=${BAYEUX_BUILDPRODUCT_DIR}
    <SOURCE_DIR>
    BUILD_COMMAND     make VERBOSE=1
    TEST_COMMAND      make test
    INSTALL_COMMAND   ""
    )
endif()
