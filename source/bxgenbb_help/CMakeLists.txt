# - Top level CMakeLists.txt for genbb_help project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2013, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "genbb_help requires an out-of-source build.")
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
project(genbb_help C CXX Fortran )

# - Versioning
set(genbb_help_VERSION_MAJOR 5)
set(genbb_help_VERSION_MINOR 0)
set(genbb_help_PATCH_VERSION 0)
set(genbb_help_VERSION "${genbb_help_VERSION_MAJOR}.${genbb_help_VERSION_MINOR}.${genbb_help_PATCH_VERSION}")
math(EXPR _genbb_help_VERSION_NUMERIC "${genbb_help_VERSION_MAJOR}*100000+${genbb_help_VERSION_MINOR}*100+${genbb_help_PATCH_VERSION}")

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

option(GENBB_HELP_WITH_FORTRAN "Build the original Fortran decay0 library and associated tools" OFF)

# ----------------------------------------------------------------------------
# Fortran/C/C++ stuff
# See : http://www.cmake.org/cmake/help/cmake-2-8-docs.html#module:FortranCInterface
if (GENBB_HELP_WITH_FORTRAN)
  include(FortranCInterface)
  FortranCInterface_VERIFY( CXX QUIET )
  if (NOT FortranCInterface_VERIFIED_CXX )
    message ( FATAL_ERROR "${PROJECT_NAME}: No Fortran/C++ compatible interface was found !" )
  else ()
    message ( STATUS "${PROJECT_NAME}: Fortran/C++ compatible interface was found !" )
  endif ()
  message ( STATUS "${PROJECT_NAME}: FortranCInterface_GLOBAL_FOUND=${FortranCInterface_GLOBAL_FOUND}" )
  message ( STATUS "${PROJECT_NAME}: FortranCInterface_MODULE_FOUND=${FortranCInterface_MODULE_FOUND}" )
else()
  message ( STATUS "${PROJECT_NAME}: Do not build the Fortran genbb/decay0 library !" )
endif()

#-----------------------------------------------------------------------
# Build options
#
option(GENBB_HELP_WITH_DEBUG "Build debug stuff" OFF)
set ( GENBB_HELP_WITH_BIO_SUPPORT 1 )
option(GENBB_HELP_WITH_BIO  "Build auxiliary Boost I/O library" ON)
option(GENBB_HELP_WITH_REFLECTION  "Build auxiliary CAMP-based reflection library" ON)
option(GENBB_HELP_WITH_DOC  "Build the genbb_help documentation" ON)
option(GENBB_HELP_WITH_TEST "Build the genbb_help test suite"    ON)

if (GENBB_HELP_WITH_DEBUG)
  set (GENBB_HELP_WITH_DEBUG 1)
else()
  set (GENBB_HELP_WITH_DEBUG 0)
endif ()

if(GENBB_HELP_WITH_BIO)
  set (GENBB_HELP_WITH_BIO 1)
else()
  set (GENBB_HELP_WITH_BIO 0)
endif ()

if(GENBB_HELP_WITH_REFLECTION)
  set (GENBB_HELP_WITH_REFLECTION 1)
  if (NOT geomtools_WITH_REFLECTION)
    message ( FATAL "Cannot build genbb_help with reflection DLL because geomtools was not so !")
  endif()
else()
  set (GENBB_HELP_WITH_REFLECTION 0)
endif ()

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - geomtools
find_package(geomtools 4.0.0 REQUIRED)

#-----------------------------------------------------------------------
# On Apple, we fixup the install prefix to handle installing documentation,
# support files and so on under the framework's "Resources" directory.
# This follows the pattern used in CMake to fixup their app bundle
# NB: THIS REQUIRES SOME FIXES IN CPACKAGING (see CMake's Qt dialog
# packaging).
if(APPLE)
  set(GENBB_HELP_FRAMEWORK_NAME genbb_help)
  set(GENBB_HELP_FRAMEWORK_LOCATION "${CMAKE_INSTALL_PREFIX}")
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}/${GENBB_HELP_FRAMEWORK_NAME}.framework/Versions/${genbb_help_VERSION}/Resources")
endif()

#-----------------------------------------------------------------------
# Build the submodules as required
#
add_subdirectory(src)

add_subdirectory(programs)

if(GENBB_HELP_WITH_DOC)
  set (GENBB_HELP_WITH_DOC 1)
  add_subdirectory(doc)
else()
  set (GENBB_HELP_WITH_DOC 0)
endif()

if(GENBB_HELP_WITH_TEST)
  set (GENBB_HELP_WITH_TEST 1)
  enable_testing()
  add_subdirectory(testing)
else()
  set (GENBB_HELP_WITH_TEST 0)
endif()


install ( FILES ${PROJECT_SOURCE_DIR}/scripts/genbb
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

#-----------------------------------------------------------------------
# Build the support files
#-----------------------------------------------------------------------
# On UNIX platforms, configure and install the shell support scripts
#
if(UNIX)
  # - Normalize exported boolean variables
  foreach(_ebv BIO REFLECTION)
    if(GENBB_HELP_WITH_${_ebv})
      set(GENBB_HELP_WITH_${_ebv} 1)
    else()
      set(GENBB_HELP_WITH_${_ebv} 0)
    endif()
  endforeach()

  set(_ld_library_path_env "LD_LIBRARY_PATH")
  if(APPLE)
    set(_ld_library_path_env "DYLD_LIBRARY_PATH")
  endif()

  configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/genbb_help-config.in
    ${PROJECT_BINARY_DIR}/cmake/genbb_help-config
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/genbb_help-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS
      OWNER_READ OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/genbb_help_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/genbb_help_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/genbb_help_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
    )
endif()

# ----------------------------------------------------------------------------
# Install resources :
#

set ( _genbb_help_RESOURCES_FULL_LOCAL_PATH
      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/resources"
    )
message ( STATUS "${PROJECT_NAME} : genbb_help resources full local path = '${_genbb_help_RESOURCES_FULL_LOCAL_PATH}' " )

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/resources
          DESTINATION ${CMAKE_INSTALL_DATADIR}
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                           GROUP_READ GROUP_WRITE
                           WORLD_READ
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                GROUP_READ GROUP_WRITE  GROUP_EXECUTE
                                WORLD_READ WORLD_EXECUTE
          PATTERN 		".svn" EXCLUDE
          PATTERN "*~" EXCLUDE
         )

# - Examples :

set(_genbb_help_examples_FULL_LOCAL_PATH
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
include(GenbbHelpCPack)

# - end of top level CMakeLists.txt for genbb_help project.
