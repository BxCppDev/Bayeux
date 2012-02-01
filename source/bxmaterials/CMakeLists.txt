# - Top level CMakeLists.txt for materials project
# -*- mode: cmake; -*-
#
# ----------------------------------------------------------------------------
# Copyright (c) 2012, François Mauger <mauger@lpccaen.in2p3.fr>
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "materials requires an out-of-source build." )
  message ( STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:" )
  message ( STATUS "  CMakeCache.txt" )
  message ( STATUS "  CMakeFiles" )
  message ( STATUS "Once these files are removed, create a separate directory" )
  message ( STATUS "and run CMake from there, pointing it to:" )
  message ( STATUS "  ${CMAKE_SOURCE_DIR}" )
  message ( FATAL_ERROR "in-source build detected" )
endif ()

# ----------------------------------------------------------------------------
# Setup CMake requirements and project's name
#
cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( materials )

set ( materials_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${materials_PACKAGE_NAME}'" )

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
	
#----------------------------------------------------------------------------
# Load any required modules
#
include ( Config )
include ( InstallDirs )
include ( PackageFilename )

# ----------------------------------------------------------------------
# Options :
#

option ( MATERIALS_WITH_DEBUG
         "Build the materials project using debugging code"
         0 )

option ( MATERIALS_WITH_DOC 
         "Build the materials documentation" 
 	 1 )

option ( MATERIALS_WITH_TEST 
         "Build the materials test material" 
 	 1 )

set ( MATERIALS_WITH_BIO_SUPPORT 0 )

set ( MATERIALS_WITH_PYTHON_WRAPPER_SUPPORT 0 )

option ( MATERIALS_WITH_CMAKE_FIND_MODULE 
         "Build the materials' CMake find module"
 	 0 )

# if ( MATERIALS_WITH_BIO_SUPPORT )
#   set ( _depends_on_datatools 1 )
# endif ()
#
# if ( MATERIALS_WITH_PYTHON_WRAPPER_SUPPORT )
#   set ( _depends_on_datatools 1 )
# endif ()


set ( _default_bio_activation 0 )
set ( _default_python_wrapper_activation 0 )

# ----------------------------------------------------------------------
# Dependencies :
#

# - Depends on geomtools:

if ( DEFINED geomtools_DIR )
  message ( STATUS "geomtools_DIR is : '${geomtools_DIR}' " )
else ()
  if ( NOT "$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}" STREQUAL "" )
    message ( STATUS "Environment GEOMTOOLS_CMAKE_CONFIG_DIR exists : '$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}' " )
    set ( geomtools_DIR "$ENV{GEOMTOOLS_CMAKE_CONFIG_DIR}" )
  endif ()
endif ()
set ( _geomtools_min_version 3.0 )
find_package ( geomtools ${_geomtools_min_version} REQUIRED NO_MODULE )
if ( DEFINED geomtools_CONFIG )
     message ( STATUS "Found geomtools >= ${_geomtools_min_version}" )
     message ( STATUS "  geomtools_CONFIG              : ${geomtools_CONFIG}" )
     message ( STATUS "  geomtools_VERSION             : ${geomtools_VERSION}" )
     message ( STATUS "  geomtools_LIBRARIES           : ${geomtools_LIBRARIES}" )
     message ( STATUS "  geomtools_INCLUDE_DIRS        : ${geomtools_INCLUDE_DIRS}" )
     message ( STATUS "  geomtools_WITH_DEBUG          : ${geomtools_WITH_DEBUG}" )
     message ( STATUS "  geomtools_WITH_BIO            : ${geomtools_WITH_BIO}" )
     message ( STATUS "  geomtools_WITH_PYTHON_WRAPPER : ${geomtools_WITH_PYTHON_WRAPPER}" )
else ()
     message ( FATAL_ERROR "Cannot find geomtools >= ${_geomtools_min_version} library !" )
endif ()

# if ( MATERIALS_WITH_BIO_SUPPORT )
#   if ( geomtools_WITH_BIO )
#     set ( _default_bio_activation 1 )
#   endif ()
# endif ()
# if ( MATERIALS_WITH_PYTHON_WRAPPER_SUPPORT )
#   if ( geomtools_WITH_PYTHON_WRAPPER )
#     set ( _default_python_wrapper_activation 1 )
#    endif ()
# endif ()
#
# if ( MATERIALS_WITH_BIO_SUPPORT )
#   option ( MATERIALS_WITH_BIO 
#            "Build the materials with Boost I/O serialization" 
#       	   ${_default_bio_activation} )
# endif ()
#
# if ( MATERIALS_WITH_PYTHON_WRAPPER_SUPPORT )
#   option ( MATERIALS_WITH_PYTHON_WRAPPER
#            "Build the materials Boost/Python wrapper module" 
#            ${_default_python_wrapper_activation} )
# endif ()

# ---------------------------------------------------

if (MATERIALS_WITH_DEBUG)
  set (MATERIALS_WITH_DEBUG 1)
endif ()

# if ( MATERIALS_WITH_BIO_SUPPORT AND MATERIALS_WITH_BIO )
#   set (MATERIALS_WITH_BIO 1)
# endif ()

# if ( MATERIALS_WITH_PYTHON_WRAPPER_SUPPORT AND MATERIALS_WITH_PYTHON_WRAPPER )
#   set (MATERIALS_WITH_PYTHON_WRAPPER 1)
# endif ()

if (MATERIALS_WITH_DOC)
  set (MATERIALS_WITH_DOC 1)
endif ()

if (MATERIALS_WITH_TEST)
  set (MATERIALS_WITH_TEST 1)
endif ()
# ---------------------------------------------------


if ( UNIX OR APPLE )
  option ( MATERIALS_WITH_CONFIG_SCRIPT 
           "Build the materials-config script" 
     	   1 )
  option ( MATERIALS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   0 )
endif ()
# ---------------------------------------------------

option ( MATERIALS_WITH_CMAKE_FIND_MODULE 
         "Build the materials CMake find module"
 	 OFF )

# ----------------------------------------------------------------------

message ( STATUS "MATERIALS_WITH_DEBUG           : " "${MATERIALS_WITH_DEBUG}" )
# if ( MATERIALS_WITH_BIO_SUPPORT )
#   message ( STATUS "MATERIALS_WITH_BIO             : " "${MATERIALS_WITH_BIO}" )
# endif ()
# if ( MATERIALS_WITH_PYTHON_WRAPPER_SUPPORT )
#   message ( STATUS "MATERIALS_WITH_PYTHON_WRAPPER  : " "${MATERIALS_WITH_PYTHON_WRAPPER}" )
# endif ()
if ( UNIX OR APPLE )
  message ( STATUS "MATERIALS_WITH_CONFIG_SCRIPT   : " "${MATERIALS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "MATERIALS_WITH_PKGCONFIG       : " "${MATERIALS_WITH_PKGCONFIG}" )
endif ()
message ( STATUS "MATERIALS_WITH_DOC             : " "${MATERIALS_WITH_DOC}" )
message ( STATUS "MATERIALS_WITH_TEST            : " "${MATERIALS_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX    : " "${CMAKE_INSTALL_PREFIX}" )

# ----------------------------------------------------------------------

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# ----------------------------------------------------------------------
# materials API
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# Configuration files :
#

# - CMake Config file:
set ( materials_cmake_config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} )

# - Pkgconfig file:
if ( MATERIALS_WITH_PKGCONFIG )
  set ( _pkgconfig_install_dir ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
  if ( DEFINED MATERIALS_PKGCONFIG_INSTALL_DIR)
    set ( _pkgconfig_install_dir ${MATERIALS_PKGCONFIG_INSTALL_DIR} )
  endif ()
endif ()

# - CMake Module file (Find) :
if ( MATERIALS_WITH_CMAKE_FIND_MODULE )
  set ( materials_cmake_find_module_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/Modules ) 
  if ( DEFINED MATERIALS_CMAKE_FIND_MODULE_INSTALL_DIR )
    set ( materials_cmake_find_module_install_dir ${MATERIALS_CMAKE_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "MATERIALS_CMAKE_FIND_MODULE_INSTALL_DIR : " "${MATERIALS_CMAKE_FIND_MODULE_INSTALL_DIR}" )
  endif ()
  message ( STATUS "materials CMake find module install dir = '${materials_cmake_find_module_install_dir}'" )
endif ()

# - C++ configuration header file :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/materials_config.h.in"
                 "${PROJECT_BINARY_DIR}/include/materials/materials_config.h" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/include/materials/materials_config.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/materials
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Bash setup script :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/materials_setup.sh.in"
                 "${PROJECT_BINARY_DIR}/cmake/materials_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/materials_setup.sh
          DESTINATION ${CMAKE_INSTALL_ETCDIR}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# - Pkgconfig configuration file :
if ( MATERIALS_WITH_PKGCONFIG )

  set ( _materials_pkgconfig_Requires "Requires: " )

  set ( _materials_pkgconfig_Requires "${_materials_pkgconfig_Requires}geomtools >= ${_geomtools_min_version}" )

  set ( _materials_pkgconfig_Libs "-lmaterials" )

  set ( _materials_pkgconfig_Cflags 
        ${geomtools_INCLUDE_DIRS} 
      )
 
  set ( _materials_pkgconfig_Libs 
        ${_materials_pkgconfig_Libs} 
        ${geomtools_LIBRARIES} 
      ) 

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/materials.pc.in"
                   "${PROJECT_BINARY_DIR}/cmake/materials.pc" @ONLY 
                 )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/materials.pc
            DESTINATION ${_pkgconfig_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
          )
endif ()

if ( MATERIALS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindMaterials.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindMaterials.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindMaterials.cmake
            DESTINATION ${_cmake_find_module_install_dir}
            COMPONENT utils
           )

endif ()

if ( UNIX OR APPLE )
  if ( MATERIALS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/materials-config.in"
                     "${PROJECT_BINARY_DIR}/cmake/materials-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/materials-config
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
# - Add resources data.
#

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/resources/data
          DESTINATION ${CMAKE_INSTALL_DATADIR}
          FILE_PERMISSIONS OWNER_READ 
                           GROUP_READ 
                           WORLD_READ 
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                                GROUP_READ GROUP_EXECUTE 
                                WORLD_READ WORLD_EXECUTE 
          PATTERN "*~" EXCLUDE 
         )

# ----------------------------------------------------------------------------
# - Add Python wrapping.
#

if ( MATERIALS_WITH_PYTHON_WRAPPER_SUPPORT AND MATERIALS_WITH_PYTHON_WRAPPER )
  set ( CMAKE_USE_PYTHON_VERSION "2.7" "2.6" )
  find_package ( PythonInterp REQUIRED )
  set ( CMAKE_USE_PYTHON_VERSION "2.7" "2.6" )
  find_package ( PythonLibs REQUIRED )

  # We need to force the cache to disable Python if we didn't find any
  # of the needed components
  message ( STATUS "Boost_PYTHON_LIBRARY = ${Boost_PYTHON_LIBRARY}" )
  message ( STATUS "PYTHON_INCLUDE_DIRS  = ${PYTHON_INCLUDE_DIRS}" )
  message ( STATUS "PYTHON_EXECUTABLE    = ${PYTHON_EXECUTABLE}" )
  message ( STATUS "PYTHON_LIBRARIES     = ${PYTHON_LIBRARIES}" )
  message ( STATUS "PYTHON_DEBUG_LIBRARIES = ${PYTHON_DEBUG_LIBRARIES}" )

  if (NOT Boost_PYTHON_LIBRARY OR NOT PYTHON_INCLUDE_DIRS OR NOT PYTHON_EXECUTABLE)
     message ( STATUS "Missing Python stuff " )
     set ( MATERIALS_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of __PackName_, requires Boost.Python" FORCE)
  endif ()
endif () 
  
#set ( CMAKE_INSTALL_PYTHONDIR "" )
#message ( STATUS "CMAKE_INSTALL_PYTHONDIR = '${CMAKE_INSTALL_PYTHONDIR}' " )

if ( MATERIALS_WITH_PYTHON_WRAPPER )
    message ( STATUS "Add subdirectory 'wrapping/python' " )
    add_subdirectory ( wrapping/python )
endif () 

# 
# ----------------------------------------------------------------------------
# Add documentation.
#
if ( MATERIALS_WITH_DOC )
  add_subdirectory ( doc )
endif ()

# ----------------------------------------------------------------------------
# Add testing - always for now.
#
if ( MATERIALS_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

# ----------------------------------------------------------------------------
# packaging
#

include ( ${MATERIALS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# ----------------------------------------------------------------------------
# - Generate Build Tree configuration files
# - CMake
# Export targets from the build tree
# Done by hand for now because we know the targets exactly
export(TARGETS materials 
  FILE ${PROJECT_BINARY_DIR}/materials-libraries.cmake
  )
# if ( MATERIALS_WITH_BIO_SUPPORT AND MATERIALS_WITH_BIO )
#   export(TARGETS materials_bio 
#     APPEND FILE ${PROJECT_BINARY_DIR}/materials-libraries.cmake
#     )
# endif()

# Configure the build tree CMake Config mode file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/materials-config.cmake.in
  ${PROJECT_BINARY_DIR}/materials-config.cmake
  @ONLY
  )

# Configure the build tree versioning file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/materials-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/materials-config-version.cmake
  @ONLY
  )



#----------------------------------------------------------------------------
# - Generate Install Tree configuration files
# - CMake
# Export targets from the install tree
# Just need to install the named export to a suitable location
if(WIN32)
  set(MATERIALS_CMAKE_CONFIG_DIR materials-${materials_VERSION_STR} )
else()
  if(MATERIALS_IS_FRAMEWORK)
    # Mac Framework location - may not be needed.
  else()
    # Standard Unix Tool
    set(MATERIALS_CMAKE_CONFIG_DIR ${materials_cmake_config_install_dir} )
  endif()
endif()

install(EXPORT materials-libraries
  DESTINATION ${MATERIALS_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )

# Configure the install tree CMake Config mode file
# Set variables
set(materials_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(materials_LIB_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
# Configure file
# This is different from the build tree file, so store in dedicated directory
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/materials-config.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/materials-config.cmake
  @ONLY
  )


# Configure the install tree versioning file
# N.B. it *probably* does not differ from the build tree file, but configuring
# separately allows easy changes should we need this.
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/Templates/materials-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/materials-config-version.cmake
  @ONLY
  )

# Install the CMake Config mode files into the install tree
install ( FILES
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/materials-config.cmake
          ${PROJECT_BINARY_DIR}/InstallTreeFiles/materials-config-version.cmake
          DESTINATION ${MATERIALS_CMAKE_CONFIG_DIR}
          COMPONENT devel
        )

# - end of top level CMakeLists.txt for materials project.
