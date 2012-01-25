# - Top level CMakeLists.txt for datatoolsproject
# -*- mode: cmake; -*-
#
# ----------------------------------------------------------------------------
# Copyright (c) 2011-2012, François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# - Enforce an out-of-source build.
# - Should be the first action in the CMakeLists.txt
#
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "datatools requires an out-of-source build." )
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
project ( datatools )

set ( datatools_PACKAGE_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "The name of the package" FORCE )
message ( STATUS "Package name         : '${datatools_PACKAGE_NAME}'" )

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
include(Config)
include(InstallDirs)
include(PackageFilename)


# ----------------------------------------------------------------------------
# - Dependencies :
#

### EXPERIMENTAL...
# set ( _cadfael_dir "" )
# if ( DEFINED CADFAEL_CMAKE_CONFIG_DIR )
#    set ( _cadfael_dir "${CADFAEL_CMAKE_CONFIG_DIR}" )
# else
#   if ( DEFINED cadfael_DIR )
#    set ( _cadfael_dir "${cadfael_DIR}" )
#   endif ()
# endif ()
#
# if ( EXISTS ${_cadfael_dir} )
#    message ( STATUS "About to use Cadfael CMake configuration directory : '${_cadfael_dir}' ")
#    set ( _cadfael_required 0 )
#    if ( DEFINED CADFAEL_REQUIRED AND CADFAEL_REQUIRED )
#      set ( _cadfael_required 1 )
#    endif ()
#    set ( _cadfael_required_label "QUIET" )
#    if ( _cadfael_required )
#      set ( _cadfael_required_label "REQUIRED" )
#    endif ()
#    find_package ( cadfael ${_cadfael_required_label} COMPONENTS Boost CLHEP)
# endif ()

# - Depends on Boost:
set ( _boost_version 1.47.0 )
if ( NOT "$ENV{BOOST_ROOT}" STREQUAL "")  
  message ( STATUS "Environment BOOST_ROOT exists : '$ENV{BOOST_ROOT}' ")
  set ( BOOST_ROOT $ENV{BOOST_ROOT} )
else ()
  message ( WARNING  "Environment BOOST_ROOT is not set !" )
endif ()

string ( REGEX REPLACE ".[0-9]+$" "" _boost_short_version "${_boost_version}" )
message ( STATUS "Boost short version is : '${_boost_short_version}'" )
set ( Boost_ADDITIONAL_VERSIONS "${_boost_short_version}" "${_boost_version}" )
set ( Boost_NO_SYSTEM_PATHS ON )
set ( _boost_requested_components filesystem serialization iostreams system 
      CACHE STRING "List of requested Boost components" FORCE )

if ( DATATOOLS_WITH_PYTHON_WRAPPER )
  set ( _boost_requested_components ${_boost_requested_components} python CACHE STRING "List of requested Boost components" FORCE )
endif ()

find_package ( Boost ${_boost_version} REQUIRED COMPONENTS ${_boost_requested_components} )
if ( Boost_FOUND )
     message ( STATUS "Found Boost ${_boost_version}")
     message ( STATUS "  Boost_VERSION          = ${Boost_VERSION}" )
     message ( STATUS "  Boost_LIB_VERSION      = ${Boost_LIB_VERSION}" )
     message ( STATUS "  Boost_INCLUDE_DIRS     = ${Boost_INCLUDE_DIRS}" )
     message ( STATUS "  Boost_LIBRARIES        = ${Boost_LIBRARIES}" )
     include_directories ( ${Boost_INCLUDE_DIRS} )
else ()
     message ( FATAL_ERROR "Cannot find Boost ${_boost_version} library !" )
endif ()

# - Depends on CLHEP:
set ( _clhep_version 2.1.0.1 )
find_package ( CLHEP ${_clhep_version} REQUIRED )
if ( CLHEP_FOUND )
     message ( STATUS "Found CLHEP ${_clhep_version}" )
     include_directories ( ${CLHEP_INCLUDE_DIRS} )
     message ( STATUS "CLHEP_LIBRARIES : ${CLHEP_LIBRARIES}" )
else ()
     message ( FATAL_ERROR "Cannot find CLHEP ${_clhep_version} library !" )
endif ()

# ----------------------------------------------------------------------
# - Options :

option ( DATATOOLS_WITH_DEBUG
         "Build the datatools project using debugging code"
         OFF )

option ( DATATOOLS_WITH_BIO 
         "Generate an auxiliary DLL with Boost I/O code" 
 	 ON )

option ( DATATOOLS_WITH_PYTHON_WRAPPER 
         "Build the datatools Python wrapper module" 
 	 OFF )

option ( DATATOOLS_WITH_DOC 
         "Build the datatools documentation" 
 	 ON )

option ( DATATOOLS_WITH_TEST 
         "Build the datatools test material" 
 	 ON )

if (DATATOOLS_WITH_DEBUG)
  set (DATATOOLS_WITH_DEBUG 1)
endif ()

if (DATATOOLS_WITH_BIO)
  set (DATATOOLS_WITH_BIO 1)
endif ()

if (DATATOOLS_WITH_PYTHON_WRAPPER)
  set (DATATOOLS_WITH_PYTHON_WRAPPER 1)
endif ()

if (DATATOOLS_WITH_DOC)
  set (DATATOOLS_WITH_DOC 1)
endif ()

if (DATATOOLS_WITH_TEST)
  set (DATATOOLS_WITH_TEST 1)
endif ()

if ( UNIX OR APPLE )
  option ( DATATOOLS_WITH_CONFIG_SCRIPT 
           "Build the datatools-config script" 
     	   ON )
  option ( DATATOOLS_WITH_PKGCONFIG 
           "Build the pkg-config configuration script (broken)" 
     	   OFF )
endif ()

set ( _datatools_cmake_config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} ) 

if ( DATATOOLS_WITH_PKGCONFIG )
  set ( _datatools_pkgconfig_install_dir ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
  if ( DEFINED DATATOOLS_PKGCONFIG_INSTALL_DIR)
    set ( _datatools_pkgconfig_install_dir ${DATATOOLS_PKGCONFIG_INSTALL_DIR} )
  endif ()
endif ()

option ( DATATOOLS_WITH_CMAKE_FIND_MODULE 
         "Build the datatools' CMake find module"
 	 OFF )

set ( _find_module_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/Modules ) 
if ( DATATOOLS_WITH_CMAKE_FIND_MODULE )
  if ( DEFINED DATATOOLS_FIND_MODULE_INSTALL_DIR )
    set ( _find_module_install_dir ${DATATOOLS_FIND_MODULE_INSTALL_DIR} ) 
    message ( STATUS "DATATOOLS_FIND_MODULE_INSTALL_DIR : " "${DATATOOLS_FIND_MODULE_INSTALL_DIR}" )
  endif ()
  message ( STATUS "datatools CMake find module install dir = '${_find_module_install_dir}'" )
endif ( DATATOOLS_WITH_CMAKE_FIND_MODULE )

message ( STATUS "DATATOOLS_WITH_DEBUG           : " "${DATATOOLS_WITH_DEBUG}" )
message ( STATUS "DATATOOLS_WITH_BIO             : " "${DATATOOLS_WITH_BIO}" )
message ( STATUS "DATATOOLS_WITH_PYTHON_WRAPPER  : " "${DATATOOLS_WITH_PYTHON_WRAPPER}" )
if (UNIX OR APPLE)
  message ( STATUS "DATATOOLS_WITH_CONFIG_SCRIPT   : " "${DATATOOLS_WITH_CONFIG_SCRIPT}" )
  message ( STATUS "DATATOOLS_WITH_PKGCONFIG       : " "${DATATOOLS_WITH_PKGCONFIG}" )
  message ( STATUS "datatools pkgconfig install dir:  ${_datatools_pkgconfig_install_dir}" )
endif (UNIX OR APPLE)
message ( STATUS "datatools CMake config install dir:  ${_datatools_cmake_config_install_dir}" )
message ( STATUS "DATATOOLS_WITH_DOC             : " "${DATATOOLS_WITH_DOC}" )
message ( STATUS "DATATOOLS_WITH_TEST            : " "${DATATOOLS_WITH_TEST}" )
message ( STATUS "CMAKE_INSTALL_PREFIX           : " "${CMAKE_INSTALL_PREFIX}" )
message ( STATUS "CMAKE_INSTALL_PYTHONDIR        : " "${CMAKE_INSTALL_PYTHONDIR}" )

# ----------------------------------------------------------------------
# - datatools API :
#

set (KWSYS_NAMESPACE datatools_sys)
message ( STATUS "Using embedded kwsys with namespace '${KWSYS_NAMESPACE}'" )
set (KWSYS_BUILD_SHARED ON)
set (KWSYS_INSTALL_EXPORT_NAME datatools-libraries)
set (KWSYS_HEADER_ROOT ${PROJECT_BINARY_DIR}/include )
set (KWSYS_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR} )
set (KWSYS_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} )
set (KWSYS_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} )
set (KWSYS_INSTALL_DOC_DIR ${CMAKE_INSTALL_DATAROOTDIR}/doc )
set (KWSYS_INSTALL_COMPONENT_NAME_RUNTIME Runtime)
set (KWSYS_INSTALL_COMPONENT_NAME_DEVELOPMENT Development)
message ( STATUS "Trigger the '${KWSYS_NAMESPACE}::DynamicLoader' class" )
set (KWSYS_USE_DynamicLoader 1) # Enable DynamicLoader class.
message ( STATUS "Add subdirectory : " "'kwsys'" )
add_subdirectory (kwsys) 

message ( STATUS "Add subdirectory : " "'src'" )
add_subdirectory ( src )   

# ----------------------------------------------------------------------
# - Configuration files :
#

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools_config.h.in"
                 "${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h" @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/datatools/
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools_setup.sh.in"
                 "${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/datatools_setup.sh
          DESTINATION ${CMAKE_INSTALL_ETCDIR}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        )

# -------------------------------------------------------------------------------------------
# - pkgconfig file :
#

if ( DATATOOLS_WITH_PKGCONFIG )

  set ( _boost_libs "date_time filesystem iostreams program_options serialization system thread" )
  set ( _datatools_pkgconfig_Requires 
        "Requires: CLHEP >= ${_clhep_version},Boost/date_time >= ${_boost_version},Boost/iostreams,Boost/filesystem,Boost/program_options,Boost/serialization,Boost/system Boost/thread" 
    )
  if ( DATATOOLS_WITH_PYTHON_WRAPPER )
     set ( _datatools_pkgconfig_Requires "${_datatools_pkgconfig_Requires},Boost/python" )
     set ( _boost_libs "${_boost_libs} python" )
  endif ( )

  set ( _datatools_pkgconfig_Libs 
        "-ldatatools_sys" 
        #"-ldatatools_sys_c" 
      )

  set ( _datatools_pkgconfig_Cflags ${CLHEP_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} )
  set ( _datatools_pkgconfig_Libs "-ldatatools" ${_datatools_pkgconfig_Libs} )

  set ( _datatools_pkgconfig_Libs 
        ${_datatools_pkgconfig_Libs} 
        ${CLHEP_LIBRARIES} 
        ${Boost_LIBRARIES}  
      )
  set ( _datatools_pkgconfig_Libs 
        ${_datatools_pkgconfig_Libs} 
        -lm -ldl 
      )

  if ( DATATOOLS_WITH_PYTHON_WRAPPER )
     set ( _datatools_pkgconfig_Cflags
           ${_datatools_pkgconfig_Cflags} 
           ${PYTHON_INCLUDE_DIRS} 
         )
     set ( _datatools_pkgconfig_Libs 
           ${_datatools_pkgconfig_Libs} 
           ${PYTHON_LIBRARIES}
         )
  endif ()

  if ( DATATOOLS_WITH_BIO )
    set ( _datatools_pkgconfig_Libs -ldatatools_bio ${_datatools_pkgconfig_Libs} )
  endif ()

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools.pc.in"
                   "${PROJECT_BINARY_DIR}/cmake/datatools.pc" @ONLY 
                 )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/datatools.pc
            DESTINATION ${_datatools_pkgconfig_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
          )
endif ()  

if ( DATATOOLS_WITH_CMAKE_FIND_MODULE )

  configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Modules/FindDatatools.cmake.in"
                   "${PROJECT_BINARY_DIR}/cmake/Modules/FindDatatools.cmake" @ONLY )

  install ( FILES ${PROJECT_BINARY_DIR}/cmake/Modules/FindDatatools.cmake
            DESTINATION ${_find_module_install_dir}
            COMPONENT utils
           )

  set ( DATATOOLS_CMAKE_MODULE_INSTALL_DIR ${_find_module_install_dir} 
        CACHE PATH "The CMake Module subdirectory" FORCE )

  message ( STATUS "DATATOOLS_CMAKE_MODULE_INSTALL_DIR = '${DATATOOLS_CMAKE_MODULE_INSTALL_DIR}' " )
endif ()

if ( UNIX OR APPLE )
  if ( DATATOOLS_WITH_CONFIG_SCRIPT )

    configure_file ( "${PROJECT_SOURCE_DIR}/cmake/templates/datatools-config.in"
                     "${PROJECT_BINARY_DIR}/cmake/datatools-config" @ONLY )

    install ( FILES ${PROJECT_BINARY_DIR}/cmake/datatools-config
              DESTINATION ${CMAKE_INSTALL_BINDIR}
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
                          GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
             )

    install ( FILES ${PROJECT_SOURCE_DIR}/pkgtools.d/mkuniqueflags.py
              DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgtools
              PERMISSIONS OWNER_READ OWNER_EXECUTE 
	                  GROUP_READ GROUP_EXECUTE 
                          WORLD_READ WORLD_EXECUTE
            )

  endif ()
endif ()  

install ( FILES README.txt COPYING.txt LICENSE.GPL3.txt
          DESTINATION ${CMAKE_INSTALL_DATADIR}
         )

# ----------------------------------------------------------------------------
# - Add Python wrapping.
#

if ( DATATOOLS_WITH_PYTHON_WRAPPER )
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
     set ( DATATOOLS_WITH_PYTHON_WRAPPER OFF CACHE BOOL "Enable Python wrapping of datatools, requires Boost.Python" FORCE)
  endif ()
endif () 
  
set ( CMAKE_INSTALL_PYTHONDIR "" )
message ( STATUS "CMAKE_INSTALL_PYTHONDIR = '${CMAKE_INSTALL_PYTHONDIR}' " )

if ( DATATOOLS_WITH_PYTHON_WRAPPER )
    message ( STATUS "Add subdirectory 'wrapping/python' " )
    add_subdirectory ( wrapping/python )
endif () 

# ----------------------------------------------------------------------------
# - Add documentation.
#
if ( DATATOOLS_WITH_DOC )
  message ( STATUS "Add subdirectory 'doc' " )
  add_subdirectory ( doc )
endif ()

# ----------------------------------------------------------------------------
# - Add testing - always for now.
#
if ( DATATOOLS_WITH_TEST )
  message ( STATUS "Add subdirectory 'testing' " )
  enable_testing () 
  add_subdirectory ( testing )
endif ()

# ----------------------------------------------------------------------------
# - Packaging
#
include ( ${DATATOOLS_SOURCE_DIR}/cmake/Packaging.cmake OPTIONAL )


# ----------------------------------------------------------------------------
# - Generate Build Tree configuration files
# - CMake
# Export targets from the build tree
# Done by hand for now because we know the targets exactly

set ( _datatools_export_append "" )
message ( STATUS "Exporting datatools_sys... " )
export ( TARGETS 
         datatools_sys
         #datatools_sys_c
         FILE ${PROJECT_BINARY_DIR}/datatools-libraries.cmake
       )
set ( _datatools_export_append "APPEND" )

message ( STATUS "Exporting datatools... " )
export(TARGETS datatools 
  ${_datatools_export_append} FILE ${PROJECT_BINARY_DIR}/datatools-libraries.cmake
  )

if(TARGET datatools_bio)
  message ( STATUS "Exporting datatools_bio... " )
  export(TARGETS datatools_bio 
    APPEND FILE ${PROJECT_BINARY_DIR}/datatools-libraries.cmake
    )
endif()

# Configure the build tree CMake Config mode file
# Set needed variables
set(datatools_INCLUDE_DIR 
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/include
  )
# Configure file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/templates/datatools-config.cmake.in
  ${PROJECT_BINARY_DIR}/datatools-config.cmake
  @ONLY
  )

# Configure the build tree versioning file
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/templates/datatools-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/datatools-config-version.cmake
  @ONLY
  )


#----------------------------------------------------------------------------
# - Generate Install Tree configuration files
# - CMake
# Export targets from the install tree
# Just need to install the named export to a suitable location
if(WIN32)
  set(DATATOOLS_CMAKE_CONFIG_DIR datatools-${datatools_VERSION_STR} )
else()
  if(DATATOOLS_IS_FRAMEWORK)
    # Mac Framework location - may not be needed.
  else()
    # Standard Unix Tool
    set(DATATOOLS_CMAKE_CONFIG_DIR ${_datatools_cmake_config_install_dir} )
  endif()
endif()

install(EXPORT datatools-libraries
  DESTINATION ${DATATOOLS_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )

# Configure the install tree CMake Config mode file
# Set variables
set(datatools_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
# Configure file
# This is different from the build tree file, so store in dedicated directory
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/templates/datatools-config.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/datatools-config.cmake
  @ONLY
  )


# Configure the install tree versioning file
# N.B. it *probably* does not differ from the build tree file, but configuring
# separately allows easy changes should we need this.
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/templates/datatools-config-version.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/datatools-config-version.cmake
  @ONLY
  )

# Install the CMake Config mode files into the install tree
install(FILES
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/datatools-config.cmake
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/datatools-config-version.cmake
  DESTINATION ${DATATOOLS_CMAKE_CONFIG_DIR}
  COMPONENT devel
  )


# - end of top level CMakeLists.txt for datatools project.
