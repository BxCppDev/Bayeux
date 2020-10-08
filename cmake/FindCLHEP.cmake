# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindCLHEP
---------

Find the native CLHEP includes and libraries.

The CLHEP Library is a numerical library for C++
programmers. 

Imported Targets
^^^^^^^^^^^^^^^^

If CLHEP is found, this module defines the following :prop_tgt:`IMPORTED`
targets::

 CLHEP::CLHEP  - The CLHEP library.

Result Variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project::

 CLHEP_FOUND          - True if CLHEP found on the local system
 CLHEP_INCLUDE_DIRS   - Location of CLHEP header files.
 CLHEP_LIBRARIES      - The CLHEP libraries.
 CLHEP_VERSION        - The version of the discovered CLHEP install.

Hints
^^^^^

Set ``CLHEP_ROOT_DIR`` to a directory that contains a CLHEP installation.

This script expects to find libraries at ``$CLHEP_ROOT_DIR/lib`` and the CLHEP
headers at ``$CLHEP_ROOT_DIR/include/CLHEP``.  The library directory may
optionally provide Release and Debug folders. 
For Unix-like systems, this script will use ``$CLHEP_ROOT_DIR/bin/clhep-config``
(if found) to aid in the discovery of CLHEP.

Cache Variables
^^^^^^^^^^^^^^^

This module may set the following variables depending on platform and type
of CLHEP installation discovered.  These variables may optionally be set to
help this module find the correct files::

 CLHEP_LIBRARY            - Location of the CLHEP library.
 CLHEP_CONFIG_EXECUTABLE  - Location of the ``clhep-config`` script (if any).
 
#]=======================================================================]

include(FindPackageHandleStandardArgs)

#=============================================================================
# If the user has provided ``CLHEP_ROOT_DIR``, use it!  Choose items found
# at this location over system locations.
if( EXISTS "$ENV{CLHEP_ROOT_DIR}" )
  file( TO_CMAKE_PATH "$ENV{CLHEP_ROOT_DIR}" CLHEP_ROOT_DIR )
  set( CLHEP_ROOT_DIR "${CLHEP_ROOT_DIR}" CACHE PATH "Prefix for CLHEP installation." )
endif()
if( NOT EXISTS "${CLHEP_ROOT_DIR}" )
  set( CLHEP_USE_PKGCONFIG ON )
endif()

#=============================================================================
# As a first try, use the PkgConfig module.  This will work on many
# *NIX systems.  See :module:`findpkgconfig`
# This will return ``CLHEP_INCLUDEDIR`` and ``CLHEP_LIBDIR`` used below.
if( CLHEP_USE_PKGCONFIG )
  find_package(PkgConfig)
  pkg_check_modules( CLHEP QUIET clhep )

  if( EXISTS "${CLHEP_INCLUDEDIR}" )
    get_filename_component( CLHEP_ROOT_DIR "${CLHEP_INCLUDEDIR}" DIRECTORY CACHE)
  endif()
endif()

#=============================================================================
# Set CLHEP_INCLUDE_DIRS and CLHEP_LIBRARIES. If we skipped the PkgConfig step, try
# to find the libraries at $CLHEP_ROOT_DIR (if provided) or in standard system
# locations.  These find_library and find_path calls will prefer custom
# locations over standard locations (HINTS).  If the requested file is not found
# at the HINTS location, standard system locations will be still be searched
# (/usr/lib64 (Redhat), lib/i386-linux-gnu (Debian)).

find_path( CLHEP_INCLUDE_DIR
  NAMES CLHEP/ClhepVersion.h
  HINTS ${CLHEP_ROOT_DIR}/include ${CLHEP_INCLUDEDIR}
)
find_library( CLHEP_LIBRARY
  NAMES CLHEP
  HINTS ${CLHEP_ROOT_DIR}/lib ${CLHEP_LIBDIR}
  PATH_SUFFIXES Release Debug
)
set( CLHEP_INCLUDE_DIRS ${CLHEP_INCLUDE_DIR} )
set( CLHEP_LIBRARIES ${CLHEP_LIBRARY} )

# If we didn't use PkgConfig, try to find the version via clhep-config or by
# reading clhep_version.h.
if( NOT CLHEP_VERSION )
  # If clhep-config exists, query for the version.
  find_program( CLHEP_CONFIG_EXECUTABLE
    NAMES clhep-config
    HINTS "${CLHEP_ROOT_DIR}/bin"
    )
  if( EXISTS "${CLHEP_CONFIG_EXECUTABLE}" )
    execute_process(
      COMMAND bash "-c" "${CLHEP_CONFIG_EXECUTABLE} --version | cut -d' ' -f2"
      OUTPUT_VARIABLE CLHEP_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE )
  endif()
else()
  message( FATAL "Cannot determine CLHEP version")
endif()

#=============================================================================
# handle the QUIETLY and REQUIRED arguments and set CLHEP_FOUND to TRUE if all
# listed variables are TRUE
find_package_handle_standard_args( CLHEP
  FOUND_VAR
    CLHEP_FOUND
  REQUIRED_VARS
    CLHEP_INCLUDE_DIR
    CLHEP_LIBRARY
  VERSION_VAR
    CLHEP_VERSION
    )

mark_as_advanced( CLHEP_ROOT_DIR CLHEP_VERSION CLHEP_LIBRARY CLHEP_INCLUDE_DIR
  CLHEP_USE_PKGCONFIG CLHEP_CONFIG )

#=============================================================================
# Register imported libraries:
# 1. If we can find a Windows .dll file (or if we can find both Debug and
#    Release libraries), we will set appropriate target properties for these.
# 2. However, for most systems, we will only register the import location and
#    include directory.

# Look for dlls, or Release and Debug libraries.
if(WIN32)
  string( REPLACE ".lib" ".dll" CLHEP_LIBRARY_DLL       "${CLHEP_LIBRARY}" )
endif()

if( CLHEP_FOUND AND NOT TARGET CLHEP::CLHEP )
  if( EXISTS "${CLHEP_LIBRARY_DLL}" AND EXISTS "${CLHEP_CBLAS_LIBRARY_DLL}")

    # Windows systems with dll libraries.
    add_library( CLHEP::CLHEP      SHARED IMPORTED )

    # Windows with dlls, but only Release libraries.
    set_target_properties( CLHEP::CLHEP PROPERTIES
      IMPORTED_LOCATION_RELEASE         "${CLHEP_LIBRARY_DLL}"
      IMPORTED_IMPLIB                   "${CLHEP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES     "${CLHEP_INCLUDE_DIRS}"
      IMPORTED_CONFIGURATIONS           Release
      IMPORTED_LINK_INTERFACE_LANGUAGES "C++")

  else()

    # For all other environments (ones without dll libraries), create
    # the imported library targets.
    add_library( CLHEP::CLHEP      UNKNOWN IMPORTED )
    set_target_properties( CLHEP::CLHEP PROPERTIES
      IMPORTED_LOCATION                 "${CLHEP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES     "${CLHEP_INCLUDE_DIRS}"
      IMPORTED_LINK_INTERFACE_LANGUAGES "C++")
  endif()
endif()
