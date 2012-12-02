# -*- mode: cmake; -*-
# - Top level CMakeLists.txt for mygsl project
#
# ----------------------------------------------------------------------------
# Copyright (c) 2012, François Mauger <mauger@lpccaen.in2p3.fr>
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "${PROJECT_NAME}: mygsl requires an out-of-source build." )
  message ( STATUS "${PROJECT_NAME}: Please remove these files from ${CMAKE_BINARY_DIR} first:" )
  message ( STATUS "${PROJECT_NAME}:   CMakeCache.txt" )
  message ( STATUS "${PROJECT_NAME}:   CMakeFiles" )
  message ( STATUS "${PROJECT_NAME}: Once these files are removed, create a separate directory" )
  message ( STATUS "${PROJECT_NAME}: and run CMake from there, pointing it to:" )
  message ( STATUS "${PROJECT_NAME}:   ${CMAKE_SOURCE_DIR}" )
  message ( FATAL_ERROR "${PROJECT_NAME}: in-source build detected" )
endif ()

# ----------------------------------------------------------------------------
# Setup CMake requirements and project's name
#
cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( mygsl )

set ( mygsl_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "${PROJECT_NAME}: Package name         : '${mygsl_PACKAGE_NAME}'" )

#----------------------------------------------------------------------------
# Prepend this project's custom module path(s) to CMAKE_MODULE_PATH
# Append any path from the environment
# This allows us to customize, add, and factor out functionality
# NB: if our custom modules include others that we don't supply, those in the
# base path will be used, so watch for incompatibilities!!
#
file(TO_CMAKE_PATH "$ENV{CMAKE_MODULE_PATH}" __env_cmake_module_path)

set(CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
  ${PROJECT_SOURCE_DIR}/cmake/Modules
  ${__env_cmake_module_path}
  ${CMAKE_MODULE_PATH}
  )

if (DEFINED ADDITIONAL_CMAKE_MODULE_PATH AND NOT "x${ADDITIONAL_CMAKE_MODULE_PATH}" STREQUAL "x" )
  message ( STATUS "${PROJECT_NAME}: Using additional CMAKE_MODULE_PATH : '${ADDITIONAL_CMAKE_MODULE_PATH}'" )
  set(CMAKE_MODULE_PATH
    ${ADDITIONAL_CMAKE_MODULE_PATH}
    ${CMAKE_MODULE_PATH}
    )
endif ()

#----------------------------------------------------------------------------
# Load any required modules
#
include ( Config )
include ( InstallDirs )
include ( PackageFilename )

# ----------------------------------------------------------------------
# Options :
#

option ( MYGSL_WITH_DEBUG
  "Build the mygsl project using debugging code"
  0 )

option ( MYGSL_WITH_DOC
  "Build the mygsl documentation"
  1 )

option ( MYGSL_WITH_TEST
  "Build the mygsl test material"
  1 )

set ( MYGSL_WITH_BIO_SUPPORT 1 )

set ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT 0 )

# ----------------------------------------------------------------------
# Dependencies :
#

# -----------------------------------------------------
# - Initial list of third party software dependencies (Python, Boost, CLHEP...) :
set ( mygsl_list_of_third_party_dependencies GSL:>=1.14 )

# -----------------------------------------------------
# - Initial list of home-made package dependencies (datatools, mygsl...) :
set ( mygsl_list_of_dependencies datatools:>=4.2.0 )

# Activate the automatic treatment of dependencies through the CMake flow :
set (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL OFF)

set (AUTOMATIC_CMAKE_DEPENDENCIES 1)
#if ( MYGSL_WITH_BIO_SUPPORT )
#  list (APPEND mygsl_list_of_dependencies "datatools:4")
#endif ()

if (AUTOMATIC_CMAKE_DEPENDENCIES)

  #########################################
  #  Automatic treatment of dependencies  #
  #########################################

  message ( STATUS "${PROJECT_NAME}: ************************************************* " )
  message ( STATUS "${PROJECT_NAME}: *                                               * " )
  message ( STATUS "${PROJECT_NAME}: *      Automatic processing of dependencies     * " )
  message ( STATUS "${PROJECT_NAME}: *                                               * " )
  message ( STATUS "${PROJECT_NAME}: ************************************************* " )

  # - Load macros for checking third party software dependencies :
  list( LENGTH mygsl_list_of_third_party_dependencies _mygsl_number_of_third_party_dependencies )
  if ( _mygsl_number_of_third_party_dependencies GREATER 0 )
    message ( STATUS "${PROJECT_NAME}: Including 'mygsl_ThirdPartyDependencies' macros... " )
    include ( mygsl_ThirdPartyDependencies )
    message ( STATUS "${PROJECT_NAME}: List of third party software dependencies : '${mygsl_list_of_third_party_dependencies}' " )
    set ( _mygsl_list_of_third_party_dependencies_infos )
    mygsl_third_party_dependencies_check_list (
      "${mygsl_list_of_third_party_dependencies}"
      _mygsl_list_of_third_party_dependencies_infos
      )
    message ( STATUS "${PROJECT_NAME}: List of third party software dependencies' infos : '${_mygsl_list_of_third_party_dependencies_infos}' " )
  else ()
    message ( STATUS "${PROJECT_NAME}: No third party software dependency." )
  endif()

  # - Load macros for checking package dependencies :
  list(LENGTH mygsl_list_of_dependencies _mygsl_number_of_dependencies)
  if (_mygsl_number_of_dependencies GREATER 0 )
    message ( STATUS "${PROJECT_NAME}: Including 'mygsl_HomeMadePackageDependencies' macros... " )
    include ( mygsl_HomeMadePackageDependencies )
    message ( STATUS "${PROJECT_NAME}: "
      "List of home made package dependencies : "
      "'${mygsl_list_of_dependencies}' "
      )
    set ( _mygsl_list_of_dependencies_infos )
    mygsl_home_made_package_dependencies_check_list (
      "${mygsl_list_of_dependencies}"
      _mygsl_list_of_dependencies_infos
      )
    message ( STATUS "${PROJECT_NAME}: List of dependencies' infos : '${_mygsl_list_of_dependencies_infos}' " )
  else ()
    message ( STATUS "${PROJECT_NAME}: No home made package dependency." )
  endif()

  message ( STATUS "${PROJECT_NAME}: ************************************************* " )
  message ( STATUS "${PROJECT_NAME}: *                                               * " )
  message ( STATUS "${PROJECT_NAME}: *  End of automatic processing of dependencies  * " )
  message ( STATUS "${PROJECT_NAME}: *                                               * " )
  message ( STATUS "${PROJECT_NAME}: ************************************************* " )

endif (AUTOMATIC_CMAKE_DEPENDENCIES)

# ----------------------------------------------------------------------
#

set ( _mygsl_default_bio_activation 0 )
set ( _mygsl_default_python_wrapper_activation 0 )

if ( MYGSL_WITH_BIO_SUPPORT )
  if ( datatools_WITH_BIO
      ### OR Yyy_WITH_BIO
      )
    set ( _mygsl_default_bio_activation 1 )
  endif ()
endif ()

if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT )
  if ( datatools_WITH_PYTHON_WRAPPER
      ### OR Yyy_WITH_PYTHON_WRAPPER
      )
    set ( _mygsl_default_python_wrapper_activation 1 )
  endif ()
endif ()

if ( MYGSL_WITH_BIO_SUPPORT )
  option ( MYGSL_WITH_BIO
    "Build the mygsl Boost I/O DLL"
    ${_mygsl_default_bio_activation} )
endif ()

if ( MYGSL_WITH_BIO_SUPPORT )
  option ( MYGSL_WITH_PYTHON_WRAPPER
    "Build the mygsl Boost/Python wrapper module"
    ${_mygsl_default_python_wrapper_activation} )
endif ()

# ---------------------------------------------------

if (MYGSL_WITH_DEBUG)
  set (MYGSL_WITH_DEBUG 1)
endif ()

if ( MYGSL_WITH_BIO_SUPPORT AND MYGSL_WITH_BIO )
  set (MYGSL_WITH_BIO 1)
endif ()

if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT AND MYGSL_WITH_PYTHON_WRAPPER )
  set (MYGSL_WITH_PYTHON_WRAPPER 1)
endif ()

if (MYGSL_WITH_DOC)
  set (MYGSL_WITH_DOC 1)
endif ()

if (MYGSL_WITH_TEST)
  set (MYGSL_WITH_TEST 1)
endif ()

# ---------------------------------------------------

if ( UNIX OR APPLE )
  option ( MYGSL_WITH_CONFIG_SCRIPT
    "Build the mygsl-config script"
    1 )
endif ()

# ----------------------------------------------------------------------

message ( STATUS "${PROJECT_NAME}: MYGSL_WITH_DEBUG           : " "${MYGSL_WITH_DEBUG}" )
if ( MYGSL_WITH_BIO_SUPPORT )
  message ( STATUS "${PROJECT_NAME}: MYGSL_WITH_BIO             : " "${MYGSL_WITH_BIO}" )
endif ()
if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT )
  message ( STATUS "${PROJECT_NAME}: MYGSL_WITH_PYTHON_WRAPPER  : " "${MYGSL_WITH_PYTHON_WRAPPER}" )
endif ()
if ( UNIX OR APPLE )
  message ( STATUS "${PROJECT_NAME}: MYGSL_WITH_CONFIG_SCRIPT   : " "${MYGSL_WITH_CONFIG_SCRIPT}" )
endif ()
message ( STATUS "${PROJECT_NAME}: MYGSL_WITH_DOC             : " "${MYGSL_WITH_DOC}" )
message ( STATUS "${PROJECT_NAME}: MYGSL_WITH_TEST            : " "${MYGSL_WITH_TEST}" )
message ( STATUS "${PROJECT_NAME}: CMAKE_INSTALL_PREFIX : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# ----------------------------------------------------------------------
# mygsl API
#

message ( STATUS "${PROJECT_NAME}: Add subdirectory : 'src'" )
add_subdirectory ( src )

# ----------------------------------------------------------------------
# Configuration files :
#

# - CMake Config file:
set ( _cmake_config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} )

# -------------------------------------------------------------------------------------

# - C++ configuration header file :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl_config.h.in"
  "${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  )

if ( UNIX OR APPLE )
  if ( MYGSL_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config.in"
      "${PROJECT_BINARY_DIR}/cmake/mygsl-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/mygsl-config
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      PERMISSIONS OWNER_READ OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
      )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/mkuniqueflags.py
      DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgtools
      PERMISSIONS OWNER_READ OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
      )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/libpkgtools.sh
      DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgtools/
      PERMISSIONS OWNER_READ OWNER_WRITE
      GROUP_READ
      WORLD_READ
      )

  endif ()
endif ()

install ( FILES README.txt COPYING.txt LICENSE.GPL3.txt
  DESTINATION ${CMAKE_INSTALL_DATADIR}
  )

# ----------------------------------------------------------------------------
# - Add resources.
#

if (EXISTS ${PROJECT_SOURCE_DIR}/resources )
  install ( DIRECTORY ${PROJECT_SOURCE_DIR}/resources
    DESTINATION ${CMAKE_INSTALL_DATADIR}
    FILE_PERMISSIONS OWNER_READ
    GROUP_READ
    WORLD_READ
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    PATTERN "*.svn" EXCLUDE
    PATTERN "*~" EXCLUDE
    )
endif ()

# ----------------------------------------------------------------------------
# - Add Python wrapping.
#

if ( MYGSL_WITH_PYTHON_WRAPPER_SUPPORT
    AND MYGSL_WITH_PYTHON_WRAPPER
    AND EXISTS ${PROJECT_SOURCE_DIR}/wrapping/python )
  set ( CMAKE_USE_PYTHON_VERSION "2.7" "2.6" )
  find_package ( PythonInterp REQUIRED )
  set ( CMAKE_USE_PYTHON_VERSION "2.7" "2.6" )
  find_package ( PythonLibs REQUIRED )

  # We need to force the cache to disable Python if we didn't find any
  # of the needed components
  message ( STATUS "${PROJECT_NAME}: Boost_PYTHON_LIBRARY = ${Boost_PYTHON_LIBRARY}" )
  message ( STATUS "${PROJECT_NAME}: PYTHON_INCLUDE_DIRS  = ${PYTHON_INCLUDE_DIRS}" )
  message ( STATUS "${PROJECT_NAME}: PYTHON_EXECUTABLE    = ${PYTHON_EXECUTABLE}" )
  message ( STATUS "${PROJECT_NAME}: PYTHON_LIBRARIES     = ${PYTHON_LIBRARIES}" )
  message ( STATUS "${PROJECT_NAME}: PYTHON_DEBUG_LIBRARIES = ${PYTHON_DEBUG_LIBRARIES}" )

  if (NOT Boost_PYTHON_LIBRARY OR NOT PYTHON_INCLUDE_DIRS OR NOT PYTHON_EXECUTABLE)
    message ( STATUS "${PROJECT_NAME}: Missing Python stuff " )
    set ( MYGSL_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of mygsl, requires Boost.Python" FORCE)
  endif ()
  message ( STATUS "${PROJECT_NAME}: Add subdirectory : 'wrapping/python' " )
  add_subdirectory ( wrapping/python )
endif ()

# ----------------------------------------------------------------------------
# Add documentation.
#
if (EXISTS ${PROJECT_SOURCE_DIR}/doc AND EXISTS ${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt )
  if ( MYGSL_WITH_DOC )
    message ( STATUS "${PROJECT_NAME}: Add subdirectory : 'doc' " )
    add_subdirectory ( doc )
  endif ()
endif ()

# ----------------------------------------------------------------------------
# Add programs.
if (EXISTS ${PROJECT_SOURCE_DIR}/programs AND EXISTS ${PROJECT_SOURCE_DIR}/programs/CMakeLists.txt )
  message ( STATUS "${PROJECT_NAME}: Add subdirectory : 'programs' " )
  add_subdirectory ( programs )
endif ()

# ----------------------------------------------------------------------------
# Add testing - always for now.
#
if (EXISTS ${PROJECT_SOURCE_DIR}/testing AND EXISTS ${PROJECT_SOURCE_DIR}/testing/CMakeLists.txt )
  if ( MYGSL_WITH_TEST )
    message ( STATUS "${PROJECT_NAME}: Add subdirectory : 'testing' " )
    enable_testing ()
    add_subdirectory ( testing )
  endif ()
endif ()

# ----------------------------------------------------------------------------
# Add validation.
#
if (EXISTS ${PROJECT_SOURCE_DIR}/validation AND EXISTS ${PROJECT_SOURCE_DIR}/validation/CMakeLists.txt )
  message ( STATUS "${PROJECT_NAME}: WARNING: validation is skipped for now ! " )
  ###message ( STATUS "${PROJECT_NAME}: Add subdirectory : 'validation' " )
  ###add_subdirectory ( validation )
endif ()

# ----------------------------------------------------------------------------
# - Generate Build Tree configuration files
# - CMake
# Export targets from the build tree
# Done by hand for now because we know the targets exactly
export(TARGETS mygsl
  FILE ${PROJECT_BINARY_DIR}/mygsl-libraries.cmake
  )
if ( MYGSL_WITH_BIO_SUPPORT AND MYGSL_WITH_BIO )
  export(TARGETS mygsl_bio
    APPEND FILE ${PROJECT_BINARY_DIR}/mygsl-libraries.cmake
    )
endif()

# Configure the build tree CMake Config mode file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config.cmake.in
  ${PROJECT_BINARY_DIR}/mygsl-config.cmake
  @ONLY
  )

# Configure the build tree versioning file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/mygsl-config-version.cmake
  @ONLY
  )

#----------------------------------------------------------------------------
# - Generate Install Tree configuration files
# - CMake
# Export targets from the install tree
# Just need to install the named export to a suitable location
if(WIN32)
  set ( MYGSL_CMAKE_CONFIG_DIR mygsl-${mygsl_VERSION_STR} )
else()
  if ( MYGSL_IS_FRAMEWORK)
    # Mac Framework location - may not be needed.
  else()
    # Standard Unix Tool
    set ( MYGSL_CMAKE_CONFIG_DIR ${_cmake_config_install_dir} )
  endif()
endif()

if(NOT IS_ABSOLUTE ${MYGSL_CMAKE_CONFIG_DIR} )
  set(MYGSL_CMAKE_CONFIG_FULL_DIR "${CMAKE_INSTALL_PREFIX}/${MYGSL_CMAKE_CONFIG_DIR}")
else()
  set(MYGSL_CMAKE_CONFIG_FULL_DIR "${MYGSL_CMAKE_CONFIG_DIR}")
  set(CMAKE_INSTALL_IS_NONRELOCATABLE 1)
endif()
message ( STATUS "${PROJECT_NAME}: NOTICE: "
  "MYGSL_CMAKE_CONFIG_DIR='${MYGSL_CMAKE_CONFIG_DIR}'" )
message ( STATUS "${PROJECT_NAME}: NOTICE: "
  "MYGSL_CMAKE_CONFIG_FULL_DIR='${MYGSL_CMAKE_CONFIG_FULL_DIR}'" )

# - Bash setup script :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl_setup.sh.in"
  "${PROJECT_BINARY_DIR}/cmake/mygsl_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/mygsl_setup.sh
  DESTINATION ${CMAKE_INSTALL_ETCDIR}
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  )

install ( EXPORT mygsl-libraries
  DESTINATION ${MYGSL_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )

# --------------------------------------------------------------------------
# Configure-  the install tree CMake Config mode file
# Set variables :
set(mygsl_INCLUDE_DIR ${CMAKE_INSTALL_FULL_INCLUDEDIR} )
set(mygsl_LIB_DIR     ${CMAKE_INSTALL_FULL_LIBDIR}     )

# --------------------------------------------------------------------------
# - Configure file
# This is different from the build tree file, so store in dedicated directory
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config.cmake
  @ONLY
  )

# --------------------------------------------------------------------------
# - Configure the install tree versioning file
# N.B. it *probably* does not differ from the build tree file, but configuring
# separately allows easy changes should we need this.
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/mygsl-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config-version.cmake
  @ONLY
  )

# Install the CMake Config mode files into the install tree
install ( FILES
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config.cmake
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/mygsl-config-version.cmake
  DESTINATION ${MYGSL_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )

if (AUTOMATIC_CMAKE_DEPENDENCIES)
  install ( FILES
    ${PROJECT_SOURCE_DIR}/cmake/mygsl_ThirdPartyDependencies.cmake
    ${PROJECT_SOURCE_DIR}/cmake/mygsl_HomeMadePackageDependencies.cmake
    DESTINATION ${MYGSL_CMAKE_CONFIG_DIR}
    COMPONENT devel
  )
endif (AUTOMATIC_CMAKE_DEPENDENCIES)

# ----------------------------------------------------------------------------
# packaging
#

include ( ${MYGSL_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# ----------------------------------------------------
# - cleaning :

unset ( _mygsl_default_bio_activation)
unset ( _mygsl_default_python_wrapper_activation)
unset ( _ld_library_path_env )
unset ( _cmake_config_install_dir )

if (AUTOMATIC_CMAKE_DEPENDENCIES)
  unset ( _mygsl_list_of_dependencies_infos )
  unset ( _mygsl_list_of_third_party_dependencies_infos )
  unset ( _mygsl_number_of_third_party_dependencies )
  unset ( _mygsl_number_of_dependencies )
endif (AUTOMATIC_CMAKE_DEPENDENCIES)

# - end of top level CMakeLists.txt for mygsl project.
