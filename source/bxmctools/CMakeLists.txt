# - Top level CMakeLists.txt for genvtx project
#-----------------------------------------------------------------------
# Copyright (c) 2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "mctools requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# Project configuration
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(mctools)

# - Versioning
set(mctools_VERSION_MAJOR 1)
set(mctools_VERSION_MINOR 0)
set(mctools_PATCH_VERSION 0)
set(mctools_VERSION "${mctools_VERSION_MAJOR}.${mctools_VERSION_MINOR}.${mctools_PATCH_VERSION}")
math(EXPR _mctools_VERSION_NUMERIC "${mctools_VERSION_MAJOR}*100000+${mctools_VERSION_MINOR}*100+${mctools_PATCH_VERSION}")

# - Fixup default install prefix on Mac
if(APPLE AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/Library/Frameworks" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif()

# - Load custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(InstallDirs)

set( _ld_library_path_env "LD_LIBRARY_PATH")
if(APPLE)
  set(_ld_library_path_env "DYLD_LIBRARY_PATH")
endif()

#-----------------------------------------------------------------------
# Build options
#
option(MCTOOLS_WITH_DEBUG "Build debug stuff" OFF)
option(MCTOOLS_WITH_BIO  "Build auxiliary Boost I/O library" ON)
option(MCTOOLS_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" ON)
option(MCTOOLS_WITH_DOC  "Build the mctools documentation" ON)
option(MCTOOLS_WITH_TEST "Build the mctools test suite"    ON)
option(MCTOOLS_WITH_G4   "Build the Geant4 interface"     ON)
option(MCTOOLS_WITH_G4VIS_USE  "Build the Geant4 visualization interface" ON)

if (MCTOOLS_WITH_DEBUG)
  set (MCTOOLS_WITH_DEBUG 1)
else()
  set (MCTOOLS_WITH_DEBUG 0)
endif ()

#-----------------------------------------------------------------------
# Find external packages needed to build
#

set(Boost_COMPONENTS program_options thread)
find_package(Boost 1.47 QUIET COMPONENTS ${Boost_COMPONENTS} NO_MODULE)
find_package(Boost 1.47 REQUIRED ${Boost_COMPONENTS})

# - dpp
find_package(dpp 1.0.0 REQUIRED)

# - genbb_help
find_package(genbb_help 5.0.0 REQUIRED)

# - genvtx
find_package(genvtx 4.0.0 REQUIRED)

# - emfield
find_package(emfield 1.0.0 REQUIRED)

if(MCTOOLS_WITH_G4)
if ( NOT DEFINED Geant4_DIR )
set ( Geant4_DIR $ENV{GEANT4_ROOT_DIR} )
endif()
find_package(Geant4 REQUIRED NO_MODULE
             PATHS ${Geant4_DIR} PATH_SUFFIXES lib64 lib)
message( STATUS "${PROJECT_NAME}: Found Geant4 :" )
message( STATUS "${PROJECT_NAME}:  o Geant4_VERSION       : ${Geant4_VERSION}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_INCLUDE_DIRS  : ${Geant4_INCLUDE_DIRS}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_USE_FILE      : ${Geant4_USE_FILE}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_gdml_FOUND    : ${Geant4_gdml_FOUND}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_ui_tcsh_FOUND : ${Geant4_ui_tcsh_FOUND}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_qt_FOUND      : ${Geant4_qt_FOUND}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_motif_FOUND   : ${Geant4_motif_FOUND}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_vis_opengl_x11_FOUND : ${Geant4_vis_opengl_x11_FOUND}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_motif_FOUND   : ${Geant4_motif_FOUND}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_DEFINITIONS   : ${Geant4_DEFINITIONS}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_USE_FILE      : ${Geant4_USE_FILE}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_CXX_FLAGS     : ${Geant4_CXX_FLAGS}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_gdml_LIBRARY  : ${Geant4_gdml_LIBRARY}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_vis_opengl_x11_LIBRARY : ${Geant4_vis_opengl_x11_LIBRARY}" )
message( STATUS "${PROJECT_NAME}:  o Geant4_LIBRARIES     : ${Geant4_LIBRARIES}" )

if(MCTOOLS_WITH_G4_G4VIS_USE)
  set(G4VIS_USE 1)
  if (Geant4_qt_FOUND)
    set(G4VIS_USE_OPENGLQT 1)
  endif()
  if (Geant4_vis_opengl_x11_FOUND)
    set(G4VIS_USE_OPENGLX 1)
    set(G4VIS_USE_OPENGLXM 1)
  endif()
endif(MCTOOLS_WITH_G4_G4VIS_USE)

endif(MCTOOLS_WITH_G4)

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory.
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(MCTOOLS_FRAMEWORK_NAME mctools)
  set(MCTOOLS_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${MCTOOLS_FRAMEWORK_NAME}.framework/Versions/${mctools_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)

if(MCTOOLS_WITH_G4)
add_subdirectory(programs/g4)
endif()

if(MCTOOLS_WITH_DOC)
  set (MCTOOLS_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set (MCTOOLS_WITH_DOC 0)
endif()

if(MCTOOLS_WITH_TEST)
  set (MCTOOLS_WITH_TEST 1)
  enable_testing()
  add_subdirectory(testing)
else()
  set (MCTOOLS_WITH_TEST 0)
endif()

#-----------------------------------------------------------------------
# Build the support files
#-----------------------------------------------------------------------
# On UNIX platforms, configure and install the shell support scripts
#
if(UNIX)
  # - Normalize exported boolean variables
  foreach(_ebv BIO REFLECTION G4)
    if(MCTOOLS_WITH_${_ebv})
      set(MCTOOLS_WITH_${_ebv} 1)
    else()
      set(MCTOOLS_WITH_${_ebv} 0)
    endif()
  endforeach()

  set(_ld_library_path_env "LD_LIBRARY_PATH")
  if(APPLE)
    set(_ld_library_path_env "DYLD_LIBRARY_PATH")
  endif()

  configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/mctools-config.in
    ${PROJECT_BINARY_DIR}/cmake/mctools-config
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/mctools-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS
      OWNER_READ OWNER_EXECUTE
    	GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/mctools_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/mctools_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/mctools_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
    )
endif()

#----------------------------------------------------------------
# - Install public resources :

# - Examples :
set(_geomtools_examples_FULL_LOCAL_PATH
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/examples"
   )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples
        DESTINATION ${CMAKE_INSTALL_DATADIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                         GROUP_READ GROUP_WRITE
                         WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                              GROUP_READ GROUP_WRITE GROUP_EXECUTE
                              WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE
	PATTERN "*~" EXCLUDE
       )

#-----------------------------------------------------------------------
# Package
#
include(MctoolsCPack)

# - end of top level CMakeLists.txt for mctools project.
