# - CMake script for building Bayeux from its submodules
#

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
# Copyright 2022 François Mauger <mauger@lpccaen.in2p3.fr>
# Copyright 2022 Université Caen Normandie
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
  )

# - Special 'EOS portable archive' for Boost >= 1.69
if (Bayeux_USE_EPA_ARCHIVES)
  message(STATUS "Using special EPA archives for Boost version ${Boost_VERSION}")
  list(APPEND BAYEUX_SUBMODULES epa)
endif()

if(Bayeux_WITH_CUTS)
  list(APPEND BAYEUX_SUBMODULES cuts)
endif()

if(Bayeux_WITH_MATERIALS)
  list(APPEND BAYEUX_SUBMODULES materials)
endif()

if(Bayeux_WITH_MYGSL)
  list(APPEND BAYEUX_SUBMODULES mygsl)
endif()

if(Bayeux_WITH_BRIO)
  list(APPEND BAYEUX_SUBMODULES brio)
endif()

if(Bayeux_WITH_DPP)
  list(APPEND BAYEUX_SUBMODULES dpp)
endif()

if(Bayeux_WITH_GEOMTOOLS)
  list(APPEND BAYEUX_SUBMODULES geomtools)
endif()

if(Bayeux_WITH_EMFIELD)
  list(APPEND BAYEUX_SUBMODULES emfield)
endif()

if(Bayeux_WITH_GENBB)
  list(APPEND BAYEUX_SUBMODULES genbb_help)
endif()

if(Bayeux_WITH_GENVTX)
  list(APPEND BAYEUX_SUBMODULES genvtx)
endif()

if(Bayeux_WITH_MCTOOLS)
  list(APPEND BAYEUX_SUBMODULES mctools)
  # if(Bayeux_WITH_GEANT4_MODULE)
  #   message(STATUS "***************** Add mctools in BAYEUX_SUBMODULES")
  #   list(APPEND BAYEUX_SUBMODULES "mctools_geant4")
  # endif()
endif()

if(Bayeux_WITH_LAHAGUE)
  list(APPEND BAYEUX_SUBMODULES lahague)
endif()

#-----------------------------------------------------------------------
# - Base build dir for resource and example files
set(MODULE_RESOURCE_ROOT_BASE "${BAYEUX_BUILD_DATAROOTDIR}/${BAYEUX_RESOURCE_DIR}")
set(MODULE_EXAMPLE_ROOT_BASE "${BAYEUX_BUILD_DATAROOTDIR}/${BAYEUX_EXAMPLE_DIR}")

# - Configure Bayeux submodule first
set(MODULE_HEADER_ROOT    "${BAYEUX_BUILD_INCLUDEDIR}")
set(MODULE_RESOURCE_ROOT  "${MODULE_RESOURCE_ROOT_BASE}/bayeux")
set(MODULE_EXAMPLE_ROOT   "${MODULE_EXAMPLE_ROOT_BASE}/bayeux")
include(bxbayeux_module.cmake)
set(Bayeux_HEADERS ${bayeux_MODULE_HEADERS})
set(Bayeux_SOURCES ${bayeux_MODULE_SOURCES})
set(Bayeux_TESTS)
set(Bayeux_RESOURCES ${bayeux_MODULE_RESOURCES})
list(APPEND Bayeux_APPS ${bayeux_MODULE_APPS})

foreach(_m ${BAYEUX_SUBMODULES})
  message( STATUS "Building Bayeux module '${_m}'...")
  set(MODULE_HEADER_ROOT   "${BAYEUX_BUILD_INCLUDEDIR}/bayeux")
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

add_library(Bayeux SHARED ${Bayeux_HEADERS} ${Bayeux_SOURCES})
target_compile_features(Bayeux PUBLIC ${BAYEUX_CXX_COMPILE_FEATURES})
target_include_directories(Bayeux
  PUBLIC
  $<BUILD_INTERFACE:${BAYEUX_BUILD_INCLUDEDIR}>
  $<BUILD_INTERFACE:${BAYEUX_BUILD_INCLUDEDIR}/bayeux>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bayeux>
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/bxbayeux
  )

set(_root_include_dirs)
set(_root_libraries)
if(Bayeux_WITH_ROOT)
  set(_root_include_dirs ${ROOT_INCLUDE_DIRS})
  set(_root_libraries ${ROOT_LIBRARIES})
endif()
 
# Also need to add, for now, include directories for deps
# that are not supplied as full imported targets
target_include_directories(Bayeux SYSTEM
  PUBLIC
   ${_root_include_dirs}
   ${CLHEP_INCLUDE_DIRS}
   )
target_link_libraries(Bayeux
  PUBLIC
    ${Bayeux_Boost_LIBRARIES}
    camp::camp
    GSL::gsl
    ${CLHEP_LIBRARIES}
    ${_root_libraries}
  PRIVATE
    ${CMAKE_DL_LIBS}
  )
if(Bayeux_WITH_QT_GUI)
  target_link_libraries(Bayeux PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Svg)
endif()
# # Split Qt SVG from other Qt components (not sure it is useful)
# if(Bayeux_WITH_QT_SVG)
#   target_link_libraries(Bayeux PUBLIC Qt5::Svg)
# endif()
 
message(STATUS "BxDecay0_LIBRARIES='${BxDecay0_LIBRARIES}'")
if(Bayeux_WITH_BXDECAY0)
  target_link_libraries(Bayeux PUBLIC ${BxDecay0_LIBRARIES})
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

if(Bayeux_WITH_GEANT4_MODULE)
  list(APPEND BAYEUX_ALL_APPS ${Bayeux_GEANT4_APPS})
endif()

# if(Bayeux_WITH_MCNP_MODULE)
#   list(APPEND BAYEUX_ALL_APPS ${Bayeux_MCNP_APPS})
# endif()

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

  if(Bayeux_WITH_GEANT4_MODULE)
    list(FIND Bayeux_GEANT4_APPS ${_bxapp} _g4app_found)
    if (_g4app_found GREATER -1)
      set_target_properties(bx${_app_basename}
        PROPERTIES COMPILE_DEFINITIONS "${Bayeux_Geant4_DEFINITIONS}"
      )
      target_link_libraries(bx${_app_basename} Bayeux_mctools_geant4)
    endif()
  endif()

  # if(Bayeux_WITH_MCNP_MODULE)
  #   list(FIND Bayeux_MCNP_APPS ${_bxapp} _mcnpapp_found)
  #   if (_mcnpapp_found GREATER -1)
  #     target_link_libraries(bx${_app_basename} Bayeux_mctools_mcnp)
  #   endif()
  # endif()

  target_link_libraries(bx${_app_basename} Bayeux)

  # - Use Readline
  if(DATATOOLS_WITH_READLINE AND NOT READLINE_ALREADY_LINKED_TO_BAYEUX)
    include_directories(${Readline_INCLUDE_DIR})
    target_link_libraries(Bayeux PRIVATE ${Readline_LIBRARIES})
    set(READLINE_ALREADY_LINKED_TO_BAYEUX 1)
  endif()

  if (Bayeux_WITH_GEOMTOOLS)
    if("${_app_basename}" STREQUAL "geomtools_inspector")
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

#-----------------------------------------------------------------------
# Apply VERSION/SOVERSION to library targets
#
set_target_properties(Bayeux ${Bayeux_ADDON_TARGETS}
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    )

#-----------------------------------------------------------------------
# Install targets and resources
#
install(TARGETS Bayeux ${Bayeux_ADDON_TARGETS}
  EXPORT  BayeuxTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
install(DIRECTORY ${BAYEUX_BUILD_INCLUDEDIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

### 2019-02-20, FM+RC: remove this hack after broken PR #28
# if(Bayeux_WITH_ROOTDICT)
#   # Special install of some brio header files used by ROOT dictionary:
#   install(DIRECTORY ${BAYEUX_BUILD_LIBDIR}/bayeux
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/
#     )
# endif()

#-----------------------------------------------------------------------
# Configure/Install support files
# Done here so that external package variables are visible
#
include(CMakePackageConfigHelpers)

# - Versioning file is the same for the build and install trees
write_basic_package_version_file(
  ${BAYEUX_BUILD_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfigVersion.cmake
  VERSION ${Bayeux_VERSION}
  COMPATIBILITY SameMajorVersion
  )

# - Config file is also the same in build/install trees as we use same layout
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/BayeuxConfig.cmake.in
  ${BAYEUX_BUILD_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_TAG}
  PATH_VARS
    CMAKE_INSTALL_BINDIR
    CMAKE_INSTALL_LIBDIR
    CMAKE_INSTALL_INCLUDEDIR
    CMAKE_INSTALL_DATAROOTDIR
    )

# - Targets (build tree)
export(EXPORT BayeuxTargets
  NAMESPACE Bayeux::
  FILE ${BAYEUX_BUILD_CMAKEDIR}/${BAYEUX_TAG}/BayeuxTargets.cmake
  )

# - Targets (install tree)
install(EXPORT BayeuxTargets
  NAMESPACE Bayeux::
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}
  )

# - Installation of, well, install tree files
install(
  FILES
    ${PROJECT_BUILD_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfigVersion.cmake
    ${PROJECT_BUILD_CMAKEDIR}/${BAYEUX_TAG}/BayeuxConfig.cmake
  DESTINATION
    ${CMAKE_INSTALL_CMAKEDIR}/${BAYEUX_TAG}
  )

#-----------------------------------------------------------------------
# Build unit tests if required
# - NB assumes each test .cxx file generates a single executable
# - We also output the executables to a dedicated directory
if(BAYEUX_ENABLE_TESTING)
  # Configure precompiled Catch main
  # Use a STATIC lib for convenience (could use OBJECT library,
  # but that doesn't transmit usage requirements like include paths
  # to clients)
  #add_library(BXCatch STATIC bxcatch/catch.hpp bxcatch/bxcatch-main.cpp)
  #target_include_directories(BXCatch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bxcatch)

  # All submodules...
  foreach(_m ${BAYEUX_SUBMODULES} bayeux)
    # # - CATCH TESTS
    # foreach(_catchtestsource ${${_m}_MODULE_TESTS_CATCH})
    #   get_filename_component(_testname "${_catchtestsource}" NAME_WE)
    #   set(_testname "bx${_m}-${_testname}")
    #   add_executable(${_testname} ${_catchtestsource})
    #   # Output to testing subdirectory
    #   set_property(TARGET ${_testname}
    #     PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bxtests"
    #     )
    #   target_link_libraries(${_testname} Bayeux BXCatch)
    #   add_test(NAME ${_testname} COMMAND ${_testname})
    #   set_property(TEST ${_testname}
    #     APPEND PROPERTY ENVIRONMENT ${${_m}_TEST_ENVIRONMENT}
    #     )
    # endforeach()

    # - OLDSTYLE TESTS
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
      if(Bayeux_WITH_GEANT4_MODULE)
        string(REGEX MATCH "mctools-test_g4_*" _test_g4_res ${_testname})
        if(NOT "x${_test_g4_res}" STREQUAL "x" )
          set_target_properties(${_testname} PROPERTIES COMPILE_DEFINITIONS "${Bayeux_Geant4_DEFINITIONS}")
          target_link_libraries(${_testname} Bayeux_mctools_geant4)
        endif()
      endif()

      ### MCNP specific link for some test progs
      if(Bayeux_WITH_MCNP_MODULE)
        string(REGEX MATCH "mctools-test_mcnp_*" _test_mcnp_res ${_testname})
        if (NOT "x${_test_mcnp_res}" STREQUAL "x" )
          target_link_libraries(${_testname} Bayeux_mctools_mcnp)
        endif()
      endif()
      target_link_libraries(${_testname} Bayeux)
      add_test(NAME ${_testname} COMMAND ${_testname})
      set_property(TEST ${_testname}
        APPEND PROPERTY ENVIRONMENT ${${_m}_TEST_ENVIRONMENT}
        )

      ### Require that brio-reader tests run after writer ones...
      if(_testname MATCHES ".*brio_reader.*")
        string(REPLACE "reader" "writer" _writer_dep ${_testname})
        set_property(TEST ${_testname} PROPERTY DEPENDS ${_writer_dep})
      endif()

      if(_testname MATCHES "brio-test_reader*")
        set_property(TEST ${_testname} PROPERTY DEPENDS "brio-test_writer;brio-test_writer_1;brio-test_writer_2")

      endif()

    endforeach()
  endforeach()
endif()

#-----------------------------------------------------------------------
# Install examples if required
foreach(_m ${BAYEUX_SUBMODULES})
  foreach(_example ${${_m}_MODULE_EXAMPLES})
    if (EXISTS "${_example}" AND IS_DIRECTORY "${_example}")
      install(DIRECTORY ${_example}/
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${BAYEUX_EXAMPLE_DIR}/${_m}
	PATTERN ".svn" EXCLUDE
	)
    endif()
  endforeach()
endforeach()

if(Bayeux_WITH_MCTOOLS)
  add_subdirectory(bxmctools/validation)
endif()


# - end
