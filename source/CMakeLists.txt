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
# the Free Software Foundation, ether version 3 of the License, or
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
find_package(Boost 1.55.0
  REQUIRED
  filesystem
  system
  serialization
  iostreams
  program_options
  regex
  # thread
  )
set(Bayeux_Boost_LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_SERIALIZATION_LIBRARY}
  ${Boost_IOSTREAMS_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  )

# - Camp
find_package(CAMP 0.7.1 REQUIRED NO_MODULE)

# - CLHEP
find_package(CLHEP 2.1.3.1 REQUIRED NO_MODULE)

# - GSL
find_package(GSL 1.15 REQUIRED)

# - ROOT
set(CMAKE_MODULE_PATH_PREROOT ${CMAKE_MODULE_PATH})
find_package(ROOT 5.34.00 REQUIRED Core Hist RIO Tree)
if(ROOT_VERSION VERSION_LESS 6)
  include(RootNewMacros)
else()
  include(${ROOT_DIR}/modules/RootNewMacros.cmake)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_PREROOT})

if (BAYEUX_WITH_QT_GUI)
  find_package(Qt5Core ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Gui ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Widgets ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Svg ${BAYEUX_QT5_VERSION} REQUIRED)
endif()

# - Geant4 if building the MCTools extension library
# We do this here because we may need to configure files in MCTools
# using the results of the search
if(BAYEUX_WITH_GEANT4_MODULE)
  find_package(Geant4 9.6.0 REQUIRED ui_all vis_all gdml NO_MODULE)
  if(Geant4_DATASETS)
    set(MCTOOLS_G4_DATA_ASSIGNMENTS)
    foreach(_ds ${Geant4_DATASETS})
      list(APPEND MCTOOLS_G4_DATA_ASSIGNMENTS "(\"${Geant4_DATASET_${_ds}_ENVVAR}\", \"${Geant4_DATASET_${_ds}_PATH}\")")
    endforeach()
    string(REPLACE ";" "" MCTOOLS_G4_DATA_ASSIGNMENTS ${MCTOOLS_G4_DATA_ASSIGNMENTS})
  endif()
  # Also need Boost.Thread, ugh
  find_package(Boost REQUIRED thread)
endif()

# - Core Includes for Bayeux + Third Party
include_directories(SYSTEM ${CLHEP_INCLUDE_DIRS})
include_directories(SYSTEM ${ROOT_INCLUDE_DIRS})
#include_directories(${CAMP_INCLUDE_DIRS})

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

#-----------------------------------------------------------------------
# Set variable to hold addons for later export
#
set(BAYEUX_ADDON_TARGETS)


# The processing order should be from least to most dependent
# That's to ensure that any needed CMake variables are set in the correct
# order, and that ordering of source files (and hence object files)
# is correct.
# NB: Order may need reviewing because may need least dependent object
# files last because of the statics...
# NB2: The use of the COFU idiom should solve most of potential issues...

# - Core
set(BAYEUX_SUBMODULES
  datatools
  cuts
  materials
  mygsl
  brio
  dpp
  geomtools
  emfield
  genbb_help
  genvtx
  mctools
  )

# - Optional
if(BAYEUX_WITH_LAHAGUE)
  list(APPEND BAYEUX_SUBMODULES lahague)
endif()

# - Base build dir for resource and example files
set(MODULE_RESOURCE_ROOT_BASE "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATADIR}/${BAYEUX_RESOURCE_DIR}")
set(MODULE_EXAMPLE_ROOT_BASE "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATADIR}/${BAYEUX_EXAMPLE_DIR}")

# - Configure Bayeux submodule first
set(MODULE_HEADER_ROOT    "${BAYEUX_BUILDPRODUCT_DIR}/include")
set(MODULE_RESOURCE_ROOT  "${MODULE_RESOURCE_ROOT_BASE}/bayeux")
set(MODULE_EXAMPLE_ROOT   "${MODULE_EXAMPLE_ROOT_BASE}/bayeux")
include(bxbayeux_module.cmake)
set(Bayeux_HEADERS ${bayeux_MODULE_HEADERS})
set(Bayeux_SOURCES ${bayeux_MODULE_SOURCES})
set(Bayeux_TESTS)
set(Bayeux_RESOURCES ${bayeux_MODULE_RESOURCES})
list(APPEND Bayeux_APPS ${bayeux_MODULE_APPS})

foreach(_m ${BAYEUX_SUBMODULES})
  set(MODULE_HEADER_ROOT   "${BAYEUX_BUILDPRODUCT_DIR}/include/bayeux")
  set(MODULE_RESOURCE_ROOT "${MODULE_RESOURCE_ROOT_BASE}/${_m}")
  set(MODULE_EXAMPLE_ROOT  "${MODULE_EXAMPLE_ROOT_BASE}/${_m}")
  include(bx${_m}_module.cmake)
  list(APPEND Bayeux_HEADERS ${${_m}_MODULE_HEADERS})
  list(APPEND Bayeux_SOURCES ${${_m}_MODULE_SOURCES})
  list(APPEND Bayeux_RESOURCES ${${_m}_MODULE_RESOURCES})
  list(APPEND Bayeux_APPS ${${_m}_MODULE_APPS})
  if (DEFINED ${_m}_GEANT4_MODULE_APPS)
    list(APPEND Bayeux_GEANT4_APPS ${${_m}_GEANT4_MODULE_APPS})
  endif()
  if (DEFINED ${_m}_MCNP_MODULE_APPS)
    list(APPEND Bayeux_MCNP_APPS ${${_m}_MNCP_MODULE_APPS})
  endif()
  if (DEFINED ${_m}_MODULE_EXAMPLES)
    list(APPEND Bayeux_EXAMPLES ${${_m}_MODULE_EXAMPLES})
  endif()
endforeach()

# - We must put this at the end because it needs static code from
#   datatools, cuts, materials...
list(APPEND Bayeux_SOURCES ${bayeux_ENDING_MODULE_SOURCES})

#-----------------------------------------------------------------------
# Build/Link the library
#
add_library(Bayeux SHARED ${Bayeux_HEADERS} ${Bayeux_SOURCES})
target_compile_features(Bayeux PUBLIC ${BAYEUX_CXX_COMPILE_FEATURES})
target_include_directories(Bayeux
  PUBLIC
   $<BUILD_INTERFACE:${BAYEUX_BUILDPRODUCT_DIR}/include>
   $<BUILD_INTERFACE:${BAYEUX_BUILDPRODUCT_DIR}/include/bayeux>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bayeux>
  PRIVATE
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}/bxbayeux
   )
# Also need to add, for now, include directories for deps
# that are not supplied as full imported targets
target_include_directories(Bayeux SYSTEM
  PUBLIC
   # ROOT
   ${ROOT_INCLUDE_DIRS}
   # CLHEP
   ${CLHEP_INCLUDE_DIRS}
   )
target_link_libraries(Bayeux
  ${CMAKE_DL_LIBS}
  ${Bayeux_Boost_LIBRARIES}
  ${CAMP_LIBRARIES}
  GSL::gsl
  ${CLHEP_LIBRARIES}
  ${ROOT_LIBRARIES}
  )
if(BAYEUX_WITH_QT_GUI)
  target_link_libraries(Bayeux Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Svg)
endif()

# - Use IWYU, if available
if(BAYEUX_WITH_IWYU_CHECK AND IWYU_EXECUTABLE)
  set_target_properties(Bayeux PROPERTIES CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXECUTABLE})
endif()

# - Set RPATH as needed
set_target_properties(Bayeux PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# - Use BinReloc
set_target_properties(Bayeux PROPERTIES COMPILE_DEFINITIONS ENABLE_BINRELOC)

if(UNIX AND NOT APPLE)
  set_target_properties(Bayeux
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

#-----------------------------------------------------------------------
# Build/Link the executables - can be switched off when embedded
#
set(BAYEUX_ALL_APPS ${Bayeux_APPS})

if(BAYEUX_WITH_GEANT4_MODULE)
  list(APPEND BAYEUX_ALL_APPS ${Bayeux_GEANT4_APPS})
endif()

if(BAYEUX_WITH_MCNP_MODULE)
  list(APPEND BAYEUX_ALL_APPS ${Bayeux_MCNP_APPS})
endif()

foreach(_bxapp IN LISTS BAYEUX_ALL_APPS)
  get_filename_component(_app_basename ${_bxapp} NAME_WE)
  add_executable(bx${_app_basename} ${_bxapp})

  # For relocation...
  target_include_directories(bx${_app_basename}
    PRIVATE
     ${CMAKE_CURRENT_BINARY_DIR}/bxbayeux
     )

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
      BUILD_WITH_INSTALL_RPATH 1
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
      )
  endif()

  if(BAYEUX_WITH_GEANT4_MODULE)
    list(FIND Bayeux_GEANT4_APPS ${_bxapp} _g4app_found)
    if (_g4app_found GREATER -1)
      target_link_libraries(bx${_app_basename} Bayeux_mctools_geant4)
    endif()
  endif()

  if(BAYEUX_WITH_MCNP_MODULE)
    list(FIND Bayeux_MCNP_APPS ${_bxapp} _mcnpapp_found)
    if (_mcnpapp_found GREATER -1)
      target_link_libraries(bx${_app_basename} Bayeux_mctools_mcnp)
    endif()
  endif()

  target_link_libraries(bx${_app_basename} Bayeux)

  # - Use Readline
  if(DATATOOLS_WITH_READLINE AND NOT READLINE_ALREADY_LINKED_TO_BAYEUX)
    include_directories(${Readline_INCLUDE_DIR})
    target_link_libraries(Bayeux ${Readline_LIBRARIES})
    set(READLINE_ALREADY_LINKED_TO_BAYEUX 1)
  endif()

  if("${_app_basename}" STREQUAL "geomtools_inspector")
    if(GEOMTOOLS_WITH_READLINE)
      include_directories(${Readline_INCLUDE_DIR})
      target_link_libraries(bx${_app_basename} ${Readline_LIBRARIES})
    endif()
  endif()

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
install(DIRECTORY ${BAYEUX_BUILDPRODUCT_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

#-----------------------------------------------------------------------
# Configure/Install support files
# Done here so that external package variables are visible
#
# - Targets
export(TARGETS Bayeux ${Bayeux_ADDON_TARGETS} FILE ${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}/BayeuxTargets.cmake)
install(EXPORT BayeuxTargets DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG})

# - Versioning (build and install trees)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxConfigVersion.cmake.in
  ${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfigVersion.cmake
  @ONLY
  )

# - Config (build and install trees *at present*)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxConfig.cmake.in
  ${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfig.cmake
  @ONLY
  )

install(
  FILES
    ${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfigVersion.cmake
    ${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfig.cmake
  DESTINATION
    ${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}
  )

#-----------------------------------------------------------------------
# Build unit tests if required
# - NB assumes each test .cxx file generates a single executable
# - We also output the executables to a dedicated directory
if(BAYEUX_ENABLE_TESTING)
  foreach(_m ${BAYEUX_SUBMODULES} bayeux)
    foreach(_testsource ${${_m}_MODULE_TESTS})
      get_filename_component(_testname "${_testsource}" NAME_WE)
      set(_testname "${_m}-${_testname}")

      add_executable(${_testname} ${_testsource})

      # Output to testing subdirectory
      set_property(TARGET ${_testname}
        PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bxtests"
        )

      # Handle CMake version difference temporarily
      set_property(TARGET ${_testname}
        APPEND PROPERTY INCLUDE_DIRECTORIES ${${_m}_test_dir}
        )

      ### G4 specific link for some test progs
      if(BAYEUX_WITH_GEANT4_MODULE)
        string(REGEX MATCH "mctools-test_g4_*" _test_g4_res ${_testname})
        # message( STATUS "    _test_g4_res = '${_test_g4_res}'")
        if(NOT "x${_test_g4_res}" STREQUAL "x" )
          message( STATUS "Special link to the Bayeux Geant4 module for test program '${_testname}'")
          target_link_libraries(${_testname} Bayeux_mctools_geant4)
        endif()
      endif()

      ### MCNP specific link for some test progs
      if(BAYEUX_WITH_MCNP_MODULE)
        string(REGEX MATCH "mctools-test_mcnp_*" _test_mcnp_res ${_testname})
        # message( STATUS "    _test_mcnp_res = '${_test_mcnp_res}'")
        if (NOT "x${_test_mcnp_res}" STREQUAL "x" )
          message( STATUS "Special link to the Bayeux MCNP module for test program '${_testname}'")
          target_link_libraries(${_testname} Bayeux_mctools_mcnp)
        endif()
      endif()

      target_link_libraries(${_testname} Bayeux)

      add_test(NAME ${_testname} COMMAND ${_testname})
      set_property(TEST ${_testname}
        APPEND PROPERTY ENVIRONMENT ${${_m}_TEST_ENVIRONMENT}
        )
    endforeach()
  endforeach()
endif()

#-----------------------------------------------------------------------
# Install examples if required
foreach(_m ${BAYEUX_SUBMODULES})
  foreach(_example ${${_m}_MODULE_EXAMPLES})
    install(DIRECTORY ${_example}/
      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${BAYEUX_EXAMPLE_DIR}/${_m}
      PATTERN ".svn" EXCLUDE
      )
  endforeach()
endforeach()
