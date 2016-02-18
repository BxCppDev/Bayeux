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
find_package(Boost 1.48.0
  REQUIRED
  filesystem
  system
  serialization
  iostreams
  program_options
  # thread
  NO_MODULE
  )
set(Bayeux_Boost_LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_SERIALIZATION_LIBRARY}
  ${Boost_IOSTREAMS_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  # ${Boost_THREAD_LIBRARY}
  )

# - Camp
find_package(CAMP 0.7.1 REQUIRED NO_MODULE)

# - CLHEP
find_package(CLHEP 2.1.3.1 REQUIRED NO_MODULE)

# - GSL
find_package(GSL 1.15 QUIET NO_MODULE)
find_package(GSL 1.15 REQUIRED)

# - ROOT
if(NOT Bayeux_USE_LEGACY_ROOT)
  set(CMAKE_MODULE_PATH_PREROOT ${CMAKE_MODULE_PATH})
endif()
find_package(ROOT 5.34.00 REQUIRED Core Hist RIO Tree)
if(NOT Bayeux_USE_LEGACY_ROOT)
  include(RootNewMacros)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_PREROOT})
endif()

if (BAYEUX_WITH_QT_GUI)
  find_package(Qt4 REQUIRED QtCore QtGui QtSvg)
endif()

# - Geant4 if building the MCTools extension library
# We do this here because we may need to configure files in MCTools
# using the results of the search
if(Bayeux_BUILD_GEANT4_MODULE)
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
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include)
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include/bayeux)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(${CLHEP_INCLUDE_DIRS})
include_directories(${GSL_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${CAMP_INCLUDE_DIRS})

if (Qt4_FOUND)
  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})
endif()

#-----------------------------------------------------------------------
# Set variable to hold addons for later export
#
set(Bayeux_ADDON_TARGETS)

#-----------------------------------------------------------------------
# - The LPC Modules

if (Bayeux_WITH_BRIO)
  set(BAYEUX_WITH_BRIO 1)
endif()

if (Bayeux_WITH_CUTS)
  set(BAYEUX_WITH_CUTS 1)
endif()

if (Bayeux_WITH_MYGSL)
  set(BAYEUX_WITH_MYGSL 1)
endif()

if (Bayeux_WITH_DPP)
  set(BAYEUX_WITH_DPP 1)
endif()

if (Bayeux_WITH_MATERIALS)
  set(BAYEUX_WITH_MATERIALS 1)
endif()

if (Bayeux_WITH_GEOMTOOLS)
  set(BAYEUX_WITH_GEOMTOOLS 1)
endif()

if (Bayeux_WITH_EMFIELD)
  set(BAYEUX_WITH_EMFIELD 1)
endif()

if (Bayeux_WITH_GENBB_HELP)
  set(BAYEUX_WITH_GENBB_HELP 1)
endif()

if (Bayeux_WITH_GENVTX)
  set(BAYEUX_WITH_GENVTX 1)
endif()

if (Bayeux_WITH_MCTOOLS)
  set(BAYEUX_WITH_MCTOOLS 1)
endif()

if (Bayeux_BUILD_GEANT4_MODULE)
  set(BAYEUX_WITH_GEANT4_MODULE 1)
else()
  set(BAYEUX_WITH_GEANT4_MODULE 0)
endif()

if (Bayeux_BUILD_MCNP_MODULE)
  set(BAYEUX_WITH_MCNP_MODULE 1)
else()
  set(BAYEUX_WITH_MCNP_MODULE 0)
endif()

if (Bayeux_WITH_LAHAGUE)
  set(BAYEUX_WITH_LAHAGUE 1)
else()
  set(BAYEUX_WITH_LAHAGUE 0)
endif()

# - Process dependencies

if (BAYEUX_WITH_LAHAGUE)
  set(BAYEUX_WITH_MCTOOLS     1)
endif()

if (BAYEUX_WITH_GEANT4_MODULE)
  set(BAYEUX_WITH_MCTOOLS     1)
endif()

if (BAYEUX_WITH_MCNP_MODULE)
  set(BAYEUX_WITH_MCTOOLS     1)
endif()

if (BAYEUX_WITH_MCTOOLS)
  set(BAYEUX_WITH_GENVTX     1)
  set(BAYEUX_WITH_GENBB_HELP 1)
  set(BAYEUX_WITH_GEOMTOOLS  1)
  set(BAYEUX_WITH_EMFIELD    1)
  set(BAYEUX_WITH_MYGSL      1)
  set(BAYEUX_WITH_DPP        1)
endif()

if (BAYEUX_WITH_GENVTX)
  set(BAYEUX_WITH_GEOMTOOLS 1)
  set(BAYEUX_WITH_MYGSL     1)
endif()

if (BAYEUX_WITH_GENBB_HELP)
  set(BAYEUX_WITH_GEOMTOOLS 1)
  set(BAYEUX_WITH_MYGSL     1)
endif()

if (BAYEUX_WITH_EMFIELD)
  set(BAYEUX_WITH_GEOMTOOLS 1)
endif()

if (BAYEUX_WITH_GEOMTOOLS)
  set(BAYEUX_WITH_MYGSL     1)
  set(BAYEUX_WITH_MATERIALS 1)
endif()

if (BAYEUX_WITH_DPP)
  set(BAYEUX_WITH_CUTS      1)
  set(BAYEUX_WITH_MYGSL     1)
  set(BAYEUX_WITH_BRIO      1)
endif()

# The processing order should be from least to most dependent
# That's to ensure that any needed CMake variables are set in the correct
# order, and that ordering of source files (and hence object files)
# is correct.
# NB: Order may need reviewing because may need least dependent object
# files last because of the statics...
# NB2: The use of the COFU idiom should solve most of potential issues...

# - Bayeux/datatools is mandatory because it contains some core functionalities used by the Bayeux library:
set(Bayeux_SUBMODULES
  datatools
)
message(STATUS "Bayeux/datatools core library module will be built")

# - Activate other library modules on demand:
if (BAYEUX_WITH_CUTS)
  list(APPEND Bayeux_SUBMODULES cuts)
  message( STATUS "Bayeux/cuts library module will be built")
endif()

if (BAYEUX_WITH_MATERIALS)
  list(APPEND Bayeux_SUBMODULES materials)
  message( STATUS "Bayeux/materials library module will be built")
endif()

if (BAYEUX_WITH_MYGSL)
  list(APPEND Bayeux_SUBMODULES mygsl)
  message( STATUS "Bayeux/mygsl library module will be built")
endif()

if (BAYEUX_WITH_BRIO)
  list(APPEND Bayeux_SUBMODULES brio)
  message( STATUS "Bayeux/brio library module will be built")
endif()

if (BAYEUX_WITH_DPP)
  list(APPEND Bayeux_SUBMODULES dpp)
  message( STATUS "Bayeux/dpp library module will be built")
endif()

if (BAYEUX_WITH_GEOMTOOLS)
  list(APPEND Bayeux_SUBMODULES geomtools)
  message( STATUS "Bayeux/geomtools library module will be built")
endif()

if (BAYEUX_WITH_EMFIELD)
  list(APPEND Bayeux_SUBMODULES emfield)
  message( STATUS "Bayeux/emfield library module will be built")
endif()

if (BAYEUX_WITH_GENBB_HELP)
  list(APPEND Bayeux_SUBMODULES genbb_help)
  message( STATUS "Bayeux/genbb_help library module will be built")
endif()

if (BAYEUX_WITH_GENVTX)
  list(APPEND Bayeux_SUBMODULES genvtx)
  message( STATUS "Bayeux/genvtx library module will be built")
endif()

if (BAYEUX_WITH_MCTOOLS)
  list(APPEND Bayeux_SUBMODULES mctools)
  message( STATUS "Bayeux/mctools library module will be built")
endif()

if (BAYEUX_WITH_LAHAGUE)
  list(APPEND Bayeux_SUBMODULES lahague)
  message( STATUS "Bayeux/lahague library module will be built")
endif()

# - Base build dir for resource and example files
set(MODULE_RESOURCE_ROOT_BASE "${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATADIR}/${Bayeux_RESOURCE_DIR}")
set(MODULE_EXAMPLE_ROOT_BASE "${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATADIR}/${Bayeux_EXAMPLE_DIR}")

# - Configure Bayeux submodule first
set(MODULE_HEADER_ROOT    "${Bayeux_BUILDPRODUCT_DIR}/include")
set(MODULE_RESOURCE_ROOT  "${MODULE_RESOURCE_ROOT_BASE}/bayeux")
set(MODULE_EXAMPLE_ROOT   "${MODULE_EXAMPLE_ROOT_BASE}/bayeux")
include(bxbayeux_module.cmake)
set(Bayeux_HEADERS ${bayeux_MODULE_HEADERS})
set(Bayeux_SOURCES ${bayeux_MODULE_SOURCES})
set(Bayeux_TESTS)
set(Bayeux_RESOURCES ${bayeux_MODULE_RESOURCES})
list(APPEND Bayeux_APPS ${bayeux_MODULE_APPS})

foreach(_m ${Bayeux_SUBMODULES})
  set(MODULE_HEADER_ROOT   "${Bayeux_BUILDPRODUCT_DIR}/include/bayeux")
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

# Hack for datatools headers
#include_directories(${datatools_include_dir})

if (BAYEUX_WITH_GEOMTOOLS)
  # Hack for geomtools private headers
  include_directories(${geomtools_include_dir})
endif()

if (BAYEUX_WITH_GENBB_HELP)
  # Hack for genbb_help private headers
  include_directories(${genbb_help_include_dir})
endif()

if (BAYEUX_WITH_MCTOOLS)
  # Hack for mctools private/generated headers
  include_directories(${mctools_include_dir})
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#-----------------------------------------------------------------------
# Build/Link the library
#
add_library(Bayeux SHARED ${Bayeux_HEADERS} ${Bayeux_SOURCES})
target_link_libraries(Bayeux ${CMAKE_DL_LIBS})
target_link_libraries(Bayeux ${Bayeux_Boost_LIBRARIES})
target_link_libraries(Bayeux ${CAMP_LIBRARIES})
target_link_libraries(Bayeux ${GSL_LIBRARIES})
target_link_libraries(Bayeux ${CLHEP_LIBRARIES})
target_link_libraries(Bayeux ${ROOT_LIBRARIES})
if (BAYEUX_WITH_QT_GUI)
  target_link_libraries(Bayeux ${QT_LIBRARIES})
endif()

# - Set RPATH as needed
set_target_properties(Bayeux PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# - Use BinReloc
set_target_properties(Bayeux PROPERTIES COMPILE_DEFINITIONS ENABLE_BINRELOC)

if(UNIX AND NOT APPLE)
  set_target_properties(Bayeux
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(Bayeux
    PROPERTIES
      INSTALL_NAME_DIR "@rpath"
      LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

#-----------------------------------------------------------------------
# Build/Link the executables - can be switched off when embedded
#
if(NOT Bayeux_EMBEDDED OR (Bayeux_EMBEDDED AND Bayeux_BUILD_TOOLS))

set(Bayeux_ALL_APPS ${Bayeux_APPS})

if (BAYEUX_WITH_GEANT4_MODULE)
  list(APPEND Bayeux_ALL_APPS ${Bayeux_GEANT4_APPS})
endif()

if (BAYEUX_WITH_MCNP_MODULE)
  list(APPEND Bayeux_ALL_APPS ${Bayeux_MCNP_APPS})
endif()

# message(STATUS "**** source/CMakeLists: Bayeux_ADDON_TARGETS=${Bayeux_ADDON_TARGETS}")

foreach(_bxapp IN LISTS Bayeux_ALL_APPS)
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
        BUILD_WITH_INSTALL_RPATH 1
        INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
        LINK_FLAGS "-Wl,-rpath,@loader_path/../${CMAKE_INSTALL_LIBDIR} -undefined dynamic_lookup"
      )
  endif()

  if (BAYEUX_WITH_MCTOOLS)
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
  endif()
  target_link_libraries(bx${_app_basename} Bayeux)

  # - Use Readline
  if(DATATOOLS_WITH_READLINE)
    include_directories(${Readline_INCLUDE_DIR})
    target_link_libraries(Bayeux ${Readline_LIBRARIES})
  endif()

  if(BAYEUX_WITH_GEOMTOOLS)
    if ("${_app_basename}" STREQUAL "geomtools_inspector")
      if(GEOMTOOLS_WITH_READLINE)
	include_directories(${Readline_INCLUDE_DIR})
	target_link_libraries(bx${_app_basename} ${Readline_LIBRARIES})
      endif()
    endif()
  endif()

  # - Hack to get program_options in without linking it to libBayeux
  target_link_libraries(bx${_app_basename} ${Boost_program_options_LIBRARY})

  list(APPEND Bayeux_ADDON_TARGETS bx${_app_basename})
endforeach()
endif()

# message(STATUS "**** source/CMakeLists: Bayeux_ADDON_TARGETS=${Bayeux_ADDON_TARGETS}")

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

#-----------------------------------------------------------------------
# Configure/Install support files
# Done here so that external package variables are visible
#
# - Targets
export(TARGETS Bayeux ${Bayeux_ADDON_TARGETS} FILE ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG}/BayeuxTargets.cmake)
install(EXPORT BayeuxTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG})

# - Versioning (build and install trees)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxConfigVersion.cmake.in
  ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG}/BayeuxConfigVersion.cmake
  @ONLY
  )

# - Config (build and install trees *at present*)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxConfig.cmake.in
  ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG}/BayeuxConfig.cmake
  @ONLY
  )

install(
  FILES
    ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG}/BayeuxConfigVersion.cmake
    ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG}/BayeuxConfig.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG}
  )

#-----------------------------------------------------------------------
# Build unit tests if required
# - NB assumes each test .cxx file generates a single executable
# - We also output the executables to a dedicated directory
if(Bayeux_ENABLE_TESTING)
  foreach(_m ${Bayeux_SUBMODULES} bayeux)
    foreach(_testsource ${${_m}_MODULE_TESTS})
      get_filename_component(_testname "${_testsource}" NAME_WE)
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
      endif()
      include_directories(BEFORE ${${_m}_test_dir})

      if (BAYEUX_WITH_MCTOOLS)
	# message( STATUS "Checking special linking for test program = '${_testname}'...")

	### G4 specific link for some test progs
	if(BAYEUX_WITH_GEANT4_MODULE)
	  string(REGEX MATCH "mctools-test_g4_*" _test_g4_res ${_testname})
	  # message( STATUS "    _test_g4_res = '${_test_g4_res}'")
	  if (NOT "x${_test_g4_res}" STREQUAL "x" )
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

#-----------------------------------------------------------------------
# Install examples if required
if(BAYEUX_WITH_EXAMPLES)
  foreach(_m ${Bayeux_SUBMODULES})
    foreach(_example ${${_m}_MODULE_EXAMPLES})
      install(DIRECTORY ${_example}/
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${Bayeux_EXAMPLE_DIR}/${_m}
        PATTERN ".svn" EXCLUDE
	      )
    endforeach()
  endforeach()
endif()
