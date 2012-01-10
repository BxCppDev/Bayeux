# - Top level CMakeLists.txt for materials project
# -*- mode: cmake; -*-
#
# ----------------------------------------------------------------------------
# Copyright (c) 2012, François Mauger <mauger@lpccaen.in2p3.fr>
#                     Ben Morgan <Ben.Morgan@warwick.ac.uk>
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# - Enforce an out-of-source build.
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
# - Setup CMake requirements and project's name
#
cmake_minimum_required ( VERSION 2.8 FATAL_ERROR )
project ( materials )

set ( materials_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${materials_PACKAGE_NAME}'" )

# include project configuration
include ( ${CMAKE_SOURCE_DIR}/cmake/Config.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/PackageFilename.cmake )

# ----------------------------------------------------------------------------
# - Prepend our own CMake Modules to the search path
# This allows us to customize, add, and factor out functionality
# NB: if our custom modules include others that we don't supply, those in the
# base path will be used, so watch for incompatibilities!!
#
message ( STATUS "Module path: ${CMAKE_MODULE_PATH} ${CMAKE_ROOT}" )

if ( NOT "$ENV{CMAKE_MODULE_PATH}" STREQUAL "" )
  message ( STATUS "Environment CMAKE_MODULE_PATH exists : '$ENV{CMAKE_MODULE_PATH}' " )
  set (__env_cmake_module_path "$ENV{CMAKE_MODULE_PATH}" )
  message ( STATUS "Environment CMAKE_MODULE_PATH : '$ENV{CMAKE_MODULE_PATH}'" )
  string ( REGEX REPLACE ":" ";" __env_cmake_module_path "${__env_cmake_module_path}" )
  set ( CMAKE_MODULE_PATH "${__env_cmake_module_path}" ${CMAKE_MODULE_PATH} )
endif ()
set ( CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake/Modules
    ${CMAKE_MODULE_PATH} )
message ( STATUS "CMAKE_ROOT        : '${CMAKE_ROOT}'" )
message ( STATUS "CMAKE_MODULE_PATH : '${CMAKE_MODULE_PATH}'" )

# ----------------------------------------------------------------------------
# - Dependencies :
#

message ( STATUS "materials depends on " ) 

# - Depends on geomtools:
set ( _geomtools_min_version 3.0 )
find_package ( Geomtools ${_geomtools_min_version} )
if ( GEOMTOOLS_FOUND )
     message ( STATUS "Found geomtools >= ${_geomtools_min_version}" )
     message ( STATUS "  GEOMTOOLS_VERSION             : ${GEOMTOOLS_VERSION}" )
     message ( STATUS "  GEOMTOOLS_INCLUDE_DIRS        : ${GEOMTOOLS_INCLUDE_DIRS}" )
     message ( STATUS "  GEOMTOOLS_LIBRARIES           : ${GEOMTOOLS_LIBRARIES}" )
     message ( STATUS "  GEOMTOOLS_INCLUDE_DIRS_DEPEND : ${GEOMTOOLS_INCLUDE_DIRS_DEPEND}" )
     message ( STATUS "  GEOMTOOLS_LIBRARIES_DEPEND    : ${GEOMTOOLS_LIBRARIES_DEPEND}" )
     #include_directories ( ${GEOMTOOLS_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find geomtools >= ${_geomtools_min_version} library !" )
endif ()

# ----------------------------------------------------------------------
# - Options :
#

option ( MATERIALS_WITH_DEBUG
         "Build the materials project using debugging code"
         OFF )

option ( MATERIALS_WITH_DOC 
         "Build the materials documentation" 
 	 ON )

option ( MATERIALS_WITH_TEST 
         "Build the materials test material" 
 	 ON )

if ( UNIX OR APPLE )
  option ( MATERIALS_WITH_CONFIG_SCRIPT 
           "Build the materials-config script" 
     	   ON )
  option ( MATERIALS_WITH_PKGCONFIG 
           "Build the pkg-config script" 
     	   ON )
endif ()

option ( MATERIALS_WITH_CMAKE_FIND_MODULE 
         "Build the materials' CMake find module"
 	 ON )

option ( MATERIALS_INSTALL_JEFF_DATA
         "Installation of JEFF 3.11 data"
         OFF
       )

message ( STATUS "MATERIALS_WITH_DEBUG           : " "${MATERIALS_WITH_DEBUG}" )
if (UNIX OR APPLE)
  message ( STATUS "MATERIALS_WITH_CONFIG_SCRIPT   : " "${MATERIALS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "MATERIALS_WITH_PKGCONFIG       : " "${MATERIALS_WITH_PKGCONFIG}" )
endif (UNIX OR APPLE)
message ( STATUS "MATERIALS_WITH_CMAKE_FIND_MODULE : " "${MATERIALS_WITH_CMAKE_FIND_MODULE}" )
message ( STATUS "MATERIALS_WITH_DOC             : " "${MATERIALS_WITH_DOC}" )
message ( STATUS "MATERIALS_WITH_TEST            : " "${MATERIALS_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX      : " "${CMAKE_INSTALL_PREFIX}" )


# ----------------------------------------------------------------------
# - materials API :
#

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# - Configuration files :
#

set ( _find_module_install_dir ${INSTALL_MISC_DIR}/cmake/Modules ) 
if ( DEFINED MATERIALS_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${MATERIALS_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "MATERIALS_FIND_MODULE_INSTALL_DIR : " "${MATERIALS_FIND_MODULE_INSTALL_DIR}" )
endif ()
message ( STATUS "_find_module_install_dir : " "${_find_module_install_dir}" )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/materials_config.h.in.cmake"
                 "${PROJECT_BINARY_DIR}/include/materials/materials_config.h" @ONLY 
               )

install ( FILES ${PROJECT_BINARY_DIR}/include/materials/materials_config.h
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/materials/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/materials_setup.sh.in.cmake"
                 "${PROJECT_BINARY_DIR}/cmake/materials_setup.sh" @ONLY 
               )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/materials_setup.sh
          DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

set ( _materials_pkgconfig_Requires 
     "Requires: geomtools >= ${_geomtools_min_version}" 
    )

set ( _materials_pkgconfig_Libs "-lmaterials" )

set ( _materials_pkgconfig_Libs 
      ${_materials_pkgconfig_Libs} 
      ${GEOMTOOLS_LIBRARIES} 
    ) 

if ( MATERIALS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindMaterials.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindMaterials.cmake" @ONLY 
                 )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindMaterials.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
          )

endif ()

if ( UNIX OR APPLE )
  if ( MATERIALS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/materials-config.in.cmake"
                     "${PROJECT_BINARY_DIR}/cmake/materials-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/materials-config
              DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
                          GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
             )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/mkuniqueflags.py
              DESTINATION ${INSTALL_MISC_DIR}/pkgtools/
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
	                  GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
            )

    install ( FILES ${PROJECT_SOURCE_DIR}/cmake/Tools/libpkgtools.sh
              DESTINATION ${INSTALL_MISC_DIR}/pkgtools/
              PERMISSIONS OWNER_READ OWNER_WRITE 
                          GROUP_READ
                          WORLD_READ
            )

  endif ()
endif ()  

install ( DIRECTORY ${PROJECT_SOURCE_DIR}/resources/data
          DESTINATION ${INSTALL_RESOURCES_DIR}
          FILE_PERMISSIONS OWNER_READ 
                           GROUP_READ 
                           WORLD_READ 
          DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                                GROUP_READ GROUP_EXECUTE 
                                WORLD_READ WORLD_EXECUTE 
          PATTERN "*~" EXCLUDE 
         )

install ( FILES ${PROJECT_SOURCE_DIR}/resources/isotopic_compositions_nist.dat
          DESTINATION ${INSTALL_RESOURCES_DIR}
          PERMISSIONS OWNER_READ 
                          GROUP_READ
                          WORLD_READ
         )

install ( FILES ${PROJECT_SOURCE_DIR}/resources/mass.mas03
          DESTINATION ${INSTALL_RESOURCES_DIR}
          PERMISSIONS OWNER_READ 
                          GROUP_READ
                          WORLD_READ
         )

if ( MATERIALS_INSTALL_JEFF_DATA )
  install ( FILES ${PROJECT_SOURCE_DIR}/resources/JEFF311RDD_ALL.OUT
            DESTINATION ${INSTALL_RESOURCES_DIR}
            PERMISSIONS OWNER_READ 
                            GROUP_READ
                            WORLD_READ
           )
endif () 

install ( FILES README.txt COPYING.txt LICENSE.GPL3.txt
          DESTINATION ${INSTALL_MISC_DIR}
         )

# ----------------------------------------------------------------------------
# - Add documentation :
#
if ( MATERIALS_WITH_DOC )
  add_subdirectory ( doc )
endif ()

# ----------------------------------------------------------------------------
# - Add testing - always for now :
#
if ( MATERIALS_WITH_TEST )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

# ----------------------------------------------------------------------------
# - Packaging
#

include ( ${MATERIALS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )

# - end of top level CMakeLists.txt for materials project.
