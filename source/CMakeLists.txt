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
# - Boost
find_package(Boost 1.48.0 REQUIRED filesystem system serialization iostreams program_options NO_MODULE)

# - Camp
find_package(CAMP 0.7.1 REQUIRED)

# - CLHEP
find_package(CLHEP 2.1.3.1 REQUIRED)

# - GSL
find_package(GSL 1.15 QUIET NO_MODULE)

# - ROOT
find_package(ROOT 5.34.00 REQUIRED Core Hist RIO Tree)


# - Core Includes for Bayeux + Third Party
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include)
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include/bayeux)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CLHEP_INCLUDE_DIRS})
include_directories(${GSL_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${CAMP_INCLUDE_DIRS})

# - The LPC Modules
# The processing order should be from least to most dependent
# That's to ensure that any needed CMake variables are set in the correct
# order, and that ordering of source files (and hence object files)
# is correct.
# NB: Order may need reviewing because may need least dependent object
# files last because of the statics...
# NB2: The use of the COFU idiom should solve most of potential issues...
set(Bayeux_SUBMODULES
  datatools
  cuts
  materials
  mygsl
  geomtools
  emfield
  brio
  dpp
  genbb_help
  genvtx
  mctools
)


# - Resource installation base dir
#set(CMAKE_INSTALL_DATAROOTDIR "share/Bayeux-${Bayeux_VERSION}")

# - Configure Bayeux submodule first
set(MODULE_HEADER_ROOT "${Bayeux_BUILDPRODUCT_DIR}/include")
include(bxbayeux_module.cmake)
set(Bayeux_HEADERS ${bayeux_MODULE_HEADERS})
set(Bayeux_SOURCES ${bayeux_MODULE_SOURCES})
set(Bayeux_TESTS)
set(Bayeux_RESOURCES)

set(MODULE_HEADER_ROOT "${Bayeux_BUILDPRODUCT_DIR}/include/bayeux")
set(MODULE_RESOURCE_ROOT_BASE "${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATADIR}/Bayeux-${Bayeux_VERSION}/resources")

foreach(_m ${Bayeux_SUBMODULES})
  set(MODULE_RESOURCE_ROOT "${MODULE_RESOURCE_ROOT_BASE}/${_m}")
  include(bx${_m}_module.cmake)
  list(APPEND Bayeux_HEADERS ${${_m}_MODULE_HEADERS})
  list(APPEND Bayeux_SOURCES ${${_m}_MODULE_SOURCES})
  list(APPEND Bayeux_RESOURCES ${${_m}_MODULE_RESOURCES})
  list(APPEND Bayeux_APPS ${${_m}_MODULE_APPS})
  if (DEFINED ${_m}_GEANT4_MODULE_APPS)
    list(APPEND Bayeux_GEANT4_APPS ${${_m}_GEANT4_MODULE_APPS})
  endif()
endforeach()

# - We must put this at the end because it needs static code from
#   datatools, cuts, materials...
list(APPEND Bayeux_SOURCES ${bayeux_ENDING_MODULE_SOURCES})

# Hack for datatools headers
#include_directories(${datatools_include_dir})

# Hack for geomtools private headers
include_directories(${geomtools_include_dir})

# Hack for genbb_help private headers
include_directories(${genbb_help_include_dir})

# Hack for mctools private headers
include_directories(${mctools_include_dir})

#-----------------------------------------------------------------------
# Build/Link the library
#
add_library(Bayeux SHARED ${Bayeux_HEADERS} ${Bayeux_SOURCES})
target_link_libraries(Bayeux ${CMAKE_DL_LIBS})
target_link_libraries(Bayeux ${Boost_LIBRARIES})
target_link_libraries(Bayeux ${CAMP_LIBRARIES})
target_link_libraries(Bayeux ${GSL_LIBRARIES})
target_link_libraries(Bayeux ${CLHEP_LIBRARIES})
target_link_libraries(Bayeux ${ROOT_LIBRARIES})

# - On Apple, ensure dynamic_lookup of undefined symbols
if(APPLE)
  set_target_properties(Bayeux PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

#-----------------------------------------------------------------------
# Set variable to hold addons for later export
#
set(Bayeux_ADDON_TARGETS)

#-----------------------------------------------------------------------
# - Build/Link the geant4 extension module, if required
#
if(Bayeux_WITH_GEANT4)
  find_package(Geant4 9.6.0 REQUIRED ui_all vis_all gdml)
  # Also need Boost.Thread, ugh
  find_package(Boost REQUIRED thread)

  include_directories(${Geant4_INCLUDE_DIRS})
  add_library(Bayeux_mctools_geant4 SHARED ${mctools_GEANT4_SOURCES} ${mctool_GEANT4_HEADERS})
  # Hack - strip "-D" flag as we should only supply the def names
  set(Bayeux_Geant4_DEFINITIONS)
  foreach(_def ${Geant4_DEFINITIONS})
    string(REGEX REPLACE "^-D" "" _bxdef ${_def})
    list(APPEND Bayeux_Geant4_DEFINITIONS ${_bxdef})
  endforeach()

  set_target_properties(Bayeux_mctools_geant4
    PROPERTIES COMPILE_DEFINITIONS "${Bayeux_Geant4_DEFINITIONS}"
    )
  target_link_libraries(Bayeux_mctools_geant4 ${Geant4_LIBRARIES} ${Boost_thread_LIBRARY})

  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(Bayeux_mctools_geant4 PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()

  # - Create the public link interface
  set_target_properties(Bayeux_mctools_geant4 PROPERTIES LINK_INTERFACE_LIBRARIES "")

  list(APPEND Bayeux_ADDON_TARGETS Bayeux_mctools_geant4)
endif()

#-----------------------------------------------------------------------
# Build/Link the executables
#
foreach(_bxapp IN LISTS Bayeux_APPS Bayeux_GEANT4_APPS)
  get_filename_component(_app_basename ${_bxapp} NAME_WE)
  add_executable(bx${_app_basename} ${_bxapp})
  set_target_properties(bx${_app_basename}
    PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1
    )
  if(UNIX AND NOT APPLE)
    set_target_properties(bx${_app_basename}
      PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
      )
  elseif(APPLE)
    # Temporary setting - needs testing
    set_target_properties(bx${_app_basename}
      PROPERTIES
        INSTALL_RPATH "@loader_path"
        LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  if(Bayeux_WITH_GEANT4)
    list(FIND Bayeux_GEANT4_APPS ${_bxapp} _g4app_found)
    if (_g4app_found GREATER -1)
      message(STATUS "Application '${_bxapp}' is link to the Geant4 addon library.")
      target_link_libraries(bx${_app_basename} Bayeux_mctools_geant4)
    endif()
  endif()
  target_link_libraries(bx${_app_basename} Bayeux)

  # - Hack to get program_options in without linking it to libBayeux
  target_link_libraries(bx${_app_basename} ${Boost_program_options_LIBRARY})

  list(APPEND Bayeux_ADDON_TARGETS bx${_app_basename})
endforeach()

#-----------------------------------------------------------------------
# Install targets and resources
#
install(TARGETS Bayeux ${Bayeux_ADDON_TARGETS}
  EXPORT  BayeuxTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
install(DIRECTORY ${Bayeux_BUILDPRODUCT_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
install(DIRECTORY ${Bayeux_BUILDPRODUCT_DIR}/share/
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

#-----------------------------------------------------------------------
# Configure/Install support files
# Done here so that external package variables are visible
#
# - Targets
export(TARGETS Bayeux ${Bayeux_ADDON_TARGETS} FILE ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-${Bayeux_VERSION}/BayeuxTargets.cmake)
install(EXPORT BayeuxTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-${Bayeux_VERSION})

# - Versioning (build and install trees)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxConfigVersion.cmake.in
  ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-${Bayeux_VERSION}/BayeuxConfigVersion.cmake
  @ONLY
  )

# - Config (build and install trees *at present*)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxConfig.cmake.in
  ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-${Bayeux_VERSION}/BayeuxConfig.cmake
  @ONLY
  )

install(
  FILES
    ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-${Bayeux_VERSION}/BayeuxConfigVersion.cmake
    ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-${Bayeux_VERSION}/BayeuxConfig.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-${Bayeux_VERSION}
  )

#-----------------------------------------------------------------------
# Build unit tests if required
# - NB assumes each test .cxx file generates a single executable
# - We also output the executables to a dedicated directory
if(Bayeux_ENABLE_TESTING)
  foreach(_m ${Bayeux_SUBMODULES})
    foreach(_testsource ${${_m}_MODULE_TESTS})
      get_filename_component(_testname "${_testsource}" NAME_WE)
      # message(STATUS "Module: ${_m} Test: ${_testname}")
      set(_testname "${_m}-${_testname}")

      add_executable(${_testname} ${_testsource})

      # Output to testing subdirectory
      set_property(TARGET ${_testname}
        PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bxtests"
        )

      # Handle CMake version difference temporarily
      if(CMAKE_VERSION VERSION_GREATER 2.8.7)
        set_property(TARGET ${_testname}
          APPEND PROPERTY INCLUDE_DIRECTORIES ${${_m}_test_dir}
          )
      else()
        include_directories(BEFORE ${${_m}_test_dir})
      endif()

      target_link_libraries(${_testname} Bayeux)

      # - On Apple, ensure dynamic_lookup of undefined symbols
      if(APPLE)
        set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
      endif()


      add_test(NAME ${_testname} COMMAND ${_testname})
      set_property(TEST ${_testname}
        APPEND PROPERTY ENVIRONMENT ${${_m}_TEST_ENVIRONMENT}
        )
    endforeach()
  endforeach()
endif()
