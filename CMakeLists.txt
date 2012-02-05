# - Top level CMakeLists.txt for Bayeux Project
#
# ----------------------------------------------------------------------------
# Copyright (c) 2011-2012 Ben Morgan <Ben.Morgan@warwick.ac.uk>
# Copyright (c) 2011 University of Warwick
#
# Copyright (c) 2012 François Mauger <mauger@lpccaen.in2p3.fr>
# Copyright (c) 2012 LPC Caen, Université de Caen Basse-Normandie
#
# All rights reserved.
#
#----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message( STATUS "Bayeux requires an out-of-source build." )
    message( STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:" )
    message( STATUS "    CMakeCache.txt" )
    message( STATUS "    CMakeFiles" )
    message( STATUS "Once these files are removed, create a separate directory" )
    message( STATUS "and run CMake from there, pointing it to:" )
    message( STATUS "    ${CMAKE_SOURCE_DIR}" )
    message( FATAL_ERROR "in-source build detected" )
endif( )


#----------------------------------------------------------------------------
# Project setup
#
cmake_minimum_required( VERSION 2.8 )
project ( Bayeux )

#----------------------------------------------------------------------------
# Prepend this project's custom module path( s ) to CMAKE_MODULE_PATH
#
file(TO_CMAKE_PATH "$ENV{CMAKE_MODULE_PATH}" __env_cmake_module_path)
set ( CMAKE_MODULE_PATH
      ${PROJECT_SOURCE_DIR}/cmake
      ${PROJECT_SOURCE_DIR}/cmake/Components
      ${__env_cmake_module_path}
      ${CMAKE_MODULE_PATH}
    )

include ( Config )
include ( InstallDirs )

set ( _is_apple 0 )
if ( APPLE )
   set ( _is_apple 1 )
endif ()

#----------------------------------------------------------------------------
# Load needed custom and builtin modules
#
# - Versioning
set ( Bayeux_VERSION ${Bayeux_VERSION_STR} )
message ( STATUS "Bayeux version : ${Bayeux_VERSION}" )

# - Processor count, and configure parallel jobs
include ( SetParallelJobs )

option ( BAYEUX_ALT_WORK_DIR "Alternative working/build directory" OFF )

# - Packaging
include ( BayeuxPackaging )

# - Support for dependent options
include ( CMakeDependentOption )

if ( NOT DEFINED BAYEUX_WORK_DIR )
  if ( BAYEUX_ALT_WORK_DIR )
    set ( BAYEUX_WORK_DIR ${CMAKE_INSTALL_PREFIX}/Work )
  else ()
    set ( BAYEUX_WORK_DIR ${PROJECT_BINARY_DIR}/Work )
  endif ()
endif ()

if ( NOT DEFINED BAYEUX_DOWNLOAD_DIR )
  set ( BAYEUX_DOWNLOAD_DIR ${BAYEUX_WORK_DIR}/Downloads
        CACHE PATH "Directory where to store some downloaded files (tarballs)" )
endif ()

if ( NOT DEFINED BAYEUX_BUILD_DIR )
  set ( BAYEUX_BUILD_DIR ${BAYEUX_WORK_DIR}/Bayeux-build )
endif ()

if ( NOT DEFINED BAYEUX_EP_BASE )
  set ( BAYEUX_EP_BASE ${BAYEUX_BUILD_DIR} )
endif ()

# -----------------------------------------------

if (DEFINED CADFAEL_DIR AND NOT "x${CADFAEL_DIR}" STREQUAL "x" )
  if (NOT EXISTS ${CADFAEL_DIR} )
    message ( FATAL_ERROR "Cadfael directory '${CADFAEL_DIR}' does not exist !" )
  endif ()   
  set ( Cadfael_DIR ${CADFAEL_DIR} )
  find (Cadfael REQUIRED)
  if (Cadfael_FOUND)
    message ( STATUS "Cadfael was found at '${CADFAEL_DIR}'" )
    message ( STATUS " o Cadfael install prefix      : '${Cadfael_ROOT_DIR}'" )
    message ( STATUS " o Cadfael version             : '${CADFAEL_VERSION}'" )
    message ( STATUS " o Cadfael components          : '${CADFAEL_COMPONENTS}'" )
    message ( STATUS " o Cadfael bash setup script   : '${CADFAEL_BASH_SETUP_SCRIPT}'" )
    message ( STATUS " o Cadfael bash setup function : '${CADFAEL_BASH_SETUP_FUNCTION}'" )
    message ( STATUS " o Cadfael CMake module paths  : '${Cadfael_CMAKE_MODULE_PATH}'" )
  endif ()
else ()
   message (STATUS "Do not use explicit Cadfael installation." )
endif ()

# -----------------------------------------------

set ( BAYEUX_KNOWN_COMPONENTS 
      	"datatools" 
	"brio" 
	"mygsl" 
	"geomtools" 
	"cuts" 
	"materials" 
	"genvtx" 
	"genbb_help" 
	)

set ( BAYEUX_KNOWN_CONFIGS 
       "trunk"
       "${Bayeux_VERSION}"
       "last_tags"
       "user"
       "default"
    )  

set ( DEFAULT_BAYEUX_CONFIG "default" )

if ( NOT DEFINED BAYEUX_CONFIG OR "x${BAYEUX_CONFIG}" STREQUAL "x" )
   set ( BAYEUX_CONFIG "${DEFAULT_BAYEUX_CONFIG}" )
endif () 

# -----------------------------------------------
# Optional components :
#

set ( BAYEUX_COMPONENTS_SVN_BASE_URL https://nemo.lpc-caen.in2p3.fr/svn/ )
set ( BAYEUX_COMPONENTS_SVN_TRUNK_SUBDIR    "trunk"    )
set ( BAYEUX_COMPONENTS_SVN_TAGS_SUBDIR     "tags"     )
set ( BAYEUX_COMPONENTS_SVN_BRANCHES_SUBDIR "branches" )

set    ( BAYEUX_WITH_DATATOOLS  1 )
option ( BAYEUX_WITH_BRIO       1 )
option ( BAYEUX_WITH_MYGSL      1 )
option ( BAYEUX_WITH_GEOMTOOLS  1 )
option ( BAYEUX_WITH_CUTS       1 )
option ( BAYEUX_WITH_MATERIALS  1 )
option ( BAYEUX_WITH_GENVTX     1 )
option ( BAYEUX_WITH_GENBB_HELP 1 )

option ( BAYEUX_WITH_BIO        ON )
option ( BAYEUX_WITH_DOCS       ON )
option ( BAYEUX_WITH_TESTS      ON )
option ( BAYEUX_WITH_VALIDATION OFF )

#----------------------------------------------------------------------------
# Configure external projects to build under a single root directory in our
# build directory or some alternative directory ( optional )
include ( ExternalProject )

message ( STATUS "Bayeux use an alternative base directory to build external projects: ${BAYEUX_ALT_EP_BASE}" )

set_directory_properties ( PROPERTIES EP_BASE ${BAYEUX_EP_BASE} )

message ( STATUS "Bayeux installation prefix : ${CMAKE_INSTALL_PREFIX}" )
message ( STATUS "Bayeux work dir     : '${BAYEUX_WORK_DIR}'" )
message ( STATUS "Bayeux download dir : '${BAYEUX_DOWNLOAD_DIR}'" )
message ( STATUS "Bayeux build dir    : '${BAYEUX_BUILD_DIR}'" )
message ( STATUS "Bayeux EP base dir  : '${BAYEUX_EP_BASE}'" )


# ---------------------------------------------------------
# Bayeux can only handle its components from some
# tag minimal version :
#
set ( BAYEUX_DATATOOLS_MINIMAL_TAG_VERSION  "4.0.0"  )
set ( BAYEUX_BRIO_MINIMAL_TAG_VERSION       "3.0.0"  )
set ( BAYEUX_MYGSL_MINIMAL_TAG_VERSION      "3.0.0"  )
set ( BAYEUX_GEOMTOOLS_MINIMAL_TAG_VERSION  "3.0.0"  )
set ( BAYEUX_CUTS_MINIMAL_TAG_VERSION       "3.0.0"  )
set ( BAYEUX_MATERIALS_MINIMAL_TAG_VERSION  "3.0.0"  )
set ( BAYEUX_GENVTX_MINIMAL_TAG_VERSION     "3.0.0"  )
set ( BAYEUX_GENBB_HELP_MINIMAL_TAG_VERSION "4.0.0"  )


# Default configuration (1.0.0) :
if ( BAYEUX_CONFIG STREQUAL "default" OR BAYEUX_CONFIG STREQUAL ${Bayeux_VERSION} )
  message ( STATUS "Bayeux 'default/${Bayeux_VERSION}' configuration...") 

  set ( BAYEUX_DATATOOLS_VERSION  "4.0.0"  )
  set ( BAYEUX_DATATOOLS_PATH     "branches/frc_cmake_0.1"  )
  set ( BAYEUX_BRIO_VERSION       "3.0.0"  )
  set ( BAYEUX_BRIO_PATH          "branches/frc_cmake_0.1"  )
  set ( BAYEUX_MYGSL_VERSION      "3.0.0"  )
  set ( BAYEUX_MYGSL_PATH         "branches/frc_cmake_0.1"  )
  set ( BAYEUX_GEOMTOOLS_VERSION  "3.0.0"  )
  set ( BAYEUX_GEOMTOOLS_PATH     "branches/frc_cmake_0.1"  )
  set ( BAYEUX_CUTS_VERSION       "3.0.0"  )
  set ( BAYEUX_CUTS_PATH          "branches/frc_cmake_0.1"  )
  set ( BAYEUX_MATERIALS_VERSION  "3.0.0"  )
  set ( BAYEUX_MATERIALS_PATH     "branches/frc_cmake_0.1"  )
  set ( BAYEUX_GENVTX_VERSION     "3.0.0"  )
  set ( BAYEUX_GENVTX_PATH        "branches/frc_cmake_0.1"  )
  set ( BAYEUX_GENBB_HELP_VERSION "4.0.0"  )
  set ( BAYEUX_GENBB_HELP_PATH    "branches/frc_cmake_0.1"  )

elseif ( BAYEUX_CONFIG STREQUAL "last_tags" )
  ###message ( STATUS "Bayeux '${BAYEUX_CONFIG}' configuration is not supported yet !") 

  foreach ( component ${BAYEUX_KNOWN_COMPONENTS} )
     string ( TOUPPER "${component}" component_upper )
     execute_process(COMMAND svn ls ${BAYEUX_COMPONENTS_SVN_BASE_URL}/${component}/${BAYEUX_COMPONENTS_SVN_TAGS_SUBDIR}/
                     TIMEOUT 15
                     RESULT_VARIABLE _${component}_list_of_tags_error_code
                     OUTPUT_VARIABLE _${component}_list_of_tags_out
                     ERROR_VARIABLE  _${component}_list_of_tags_err
                     ) 
     if ( NOT ${_${component}_list_of_tags_error_code} EQUAL 0 )
       message ( FATAL_ERROR "Cannot find the list of available tags for the Bayeux component : '${component}' !") 
     endif ()
     ###message ( STATUS "List of available tags for '${component}' : \n${_${component}_list_of_tags_out}" ) 
     execute_process(COMMAND echo ${_${component}_list_of_tags_out} 
                     COMMAND sort 
                     COMMAND tail -1 
                     COMMAND tr -d '/' 
                     COMMAND tr -d '[[:space:]]' 
                     RESULT_VARIABLE _${component}_last_tag_out_error_code
                     OUTPUT_VARIABLE _${component}_last_tag_out
                    )
     if ( NOT ${_${component}_last_tag_out_error_code} EQUAL 0 )
       message ( FATAL_ERROR "Cannot find the last available tag for the Bayeux component : '${component}' !") 
     endif ()
     if ( "x${_${component}_last_tag_out}" STREQUAL "x" )
       message ( FATAL_ERROR "Cannot determine the last available tag for the Bayeux component : '${component}' !") 
     endif ()
     message ( STATUS "  Last available ${component} tag is : '${_${component}_last_tag_out}' " ) 
     message ( STATUS "  Minimal supported ${component} tag version is : '${component}-${BAYEUX_${component_upper}_MINIMAL_TAG_VERSION}' " ) 
     if ( "${_${component}_last_tag_out}" STRLESS "${component}-${BAYEUX_${component_upper}_MINIMAL_TAG_VERSION}" )
       message ( FATAL_ERROR "Unsupported tag '${_${component}_last_tag_out}' for the Bayeux component : '${component}' !")       
     endif ()  
     set ( BAYEUX_${component_upper}_PATH    "tags/${_${component}_last_tag_out}"  )
     message ( STATUS "Using last tag '${_${component}_last_tag_out}' for the Bayeux component : '${component}'")       
  endforeach  ()

elseif ( BAYEUX_CONFIG STREQUAL "trunk" )
  message ( STATUS "Bayeux 'trunk' configuration...") 
  message ( FATAL_ERROR "Bayeux '${BAYEUX_CONFIG}' configuration is not supported yet !") 

  set ( BAYEUX_DATATOOLS_PATH  "trunk" )
  set ( BAYEUX_BRIO_PATH       "trunk" )
  set ( BAYEUX_MYGSL_PATH      "trunk" )
  set ( BAYEUX_GEOMTOOLS_PATH  "trunk" )
  set ( BAYEUX_CUTS_PATH       "trunk" )
  set ( BAYEUX_MATERIALS_PATH  "trunk" )
  set ( BAYEUX_GENVTX_PATH     "trunk" )
  set ( BAYEUX_GENBB_HELP_PATH "trunk" )

elseif ( BAYEUX_CONFIG STREQUAL "user" )
  message ( STATUS "Bayeux 'user' configuration...") 

  if (DEFINED BAYEUX_USER_CONFIG_FILE AND NOT "x${BAYEUX_USER_CONFIG_FILE}" STREQUAL "x" )
    message ( STATUS "User configuration file...") 
    if ( NOT EXISTS ${BAYEUX_USER_CONFIG_FILE} )
      message ( FATAL_ERROR "User configuration file '${BAYEUX_USER_CONFIG_FILE}' does not exist !") 
    endif  ()
    message ( STATUS "Parsing user configuration file...") 
    foreach ( component ${BAYEUX_KNOWN_COMPONENTS} )
      string ( TOUPPER "${component}" component_upper )
      file ( STRINGS ${BAYEUX_USER_CONFIG_FILE} _component_directive
             REGEX "^${component}@"
           )
      message ( STATUS "User directive for component '${component}' : '${_component_directive}' ") 
      if ( _component_directive STREQUAL "" )
        set ( BAYEUX_WITH_${component_upper} 0 )
      else () 
        if ( NOT DEFINED BAYEUX_WITH_${component_upper} OR BAYEUX_WITH_${component_upper} )
          set ( BAYEUX_WITH_${component_upper} 1 )
	  string ( REPLACE "${component}@" ""
                   _component_path 
	           ${_component_directive} 
	         )
          message ( STATUS "Parsed ${component} path : ${_component_path}") 
	       
          if ( "${_component_directive}" STREQUAL "${_component_path}" )
             message ( FATAL_ERROR "Invalid directive for component '${component}'" ) 
          endif ()  
          set ( BAYEUX_${component_upper}_PATH ${_component_path} )
        endif ()
      endif ()
      message ( STATUS "Bayeux user configuration mode : BAYEUX_WITH_${component_upper}") 
    endforeach ()
  else ()
    message ( FATAL_ERROR "Missing user configuration file ! Please use : '-DBAYEUX_USER_CONFIG_FILE:PATH=<somefile>' !") 
  endif ()
  ###if ( NOT DEFINED BAYEUX_DATATOOLS_PATH )
  ###  message ( FATAL_ERROR "Missing SVN path directive for datatools" ) 
  ###endif () 
endif ()

foreach ( component ${BAYEUX_KNOWN_COMPONENTS} )
  message ( STATUS "Processing component '${component}'"... )
  string ( TOUPPER "${component}" component_upper )
  set ( _bayeux_component_with_key    "BAYEUX_WITH_${component_upper}" )
  set ( _bayeux_component_path_key    "BAYEUX_${component_upper}_PATH" )
  set ( _bayeux_component_version_key "BAYEUX_${component_upper}_VERSION" )
  message ( STATUS "  component with key     = '${_bayeux_component_with_key}'"    )
  message ( STATUS "  component path key     = '${_bayeux_component_path_key}'"    )
  message ( STATUS "  component version key  = '${_bayeux_component_version_key}'" )
  set ( _bayeux_install_component 1 )
  if ( NOT ${_bayeux_component_with_key} )
     set ( _bayeux_install_component 0 )
  endif ()
  if ( NOT DEFINED ${_bayeux_component_path_key} )
     set ( _bayeux_install_component 0 )
  endif ()
  message ( STATUS "  component installation = '${_bayeux_install_component}'" )
  if ( _bayeux_install_component )
     if ( NOT "${_bayeux_component_version_key}" STREQUAL "" )
       set ( _bayeux_component_version ${_bayeux_component_version_key} )
     else ()
       set ( _bayeux_component_version ${_bayeux_component_path_key} )
     endif ()  
     ###message ( STATUS "This configuration will install '${component}' version '${${_bayeux_component_version}}'" )
     message ( STATUS "This configuration will install '${component}' from '${${_bayeux_component_path_key}}'" )
  endif ()
  set ( BAYEUX_WITH_${component_upper} ${_bayeux_install_component} )
endforeach ()

set ( _bayeux_special_1 OFF )

if ( _bayeux_special_1 )
  foreach ( component ${BAYEUX_KNOWN_COMPONENTS} )
    string ( TOUPPER "${component}" component_upper )
    if ( DEFINED BAYEUX_${component_upper}_PATH )
      set ( _${component}_path "${BAYEUX_${component_upper}_PATH}" )
      message ( STATUS "${component} path label = '${_${component}_path}'" )
      if ( ${_${component}_path} STREQUAL "trunk" )
	set ( _${component}_versioned_path "${component}-trunk" )
      else ()
	string ( REGEX REPLACE "/" "__"
          _${component}_versioned_path
          ${_${component}_path}
          )
	message ( STATUS "_${component}_versioned_path = '${_${component}_versioned_path}'" )
	string (SUBSTRING ${_${component}_path} 0 5 _prefix_path )
	if ( "${_prefix_path}" STREQUAL "tags/" )
          string (REGEX REPLACE "^tags/" ""
            _${component}_versioned_path
            ${_${component}_path}
            )
	endif ()
	string (SUBSTRING ${_${component}_path} 0 9 _prefix_path )
	if ( "${_prefix_path}" STREQUAL "branches/" )
          string (REGEX REPLACE "branches/" "${component}-branch-"
            _${component}_versioned_path
            ${_${component}_path}
            )
	endif ()
      endif ()
    endif ()
    message ( STATUS "${component} version path = '${_${component}_versioned_path}'" )
  endforeach ()
endif ()

if ( NOT DEFINED SVN_NEMO_LOGIN )
  message ( FATAL_ERROR "No SVN user login is provided !" )
endif ()
if ( NOT DEFINED SVN_NEMO_PASSWORD )
  message ( FATAL_ERROR "No SVN user password is provided !" )
endif ()

set ( _svn_username "${SVN_NEMO_LOGIN}" )
set ( _svn_password "${SVN_NEMO_PASSWORD}" )
set ( _install_prefix ${CMAKE_INSTALL_PREFIX} )
 
# -------------------------------------------------------------------------
# - Ensure proper dependencies between Bayeux components :
#

if    ( BAYEUX_WITH_MATERIALS )
  set ( BAYEUX_WITH_GEOMTOOLS 1 )
endif ( BAYEUX_WITH_MATERIALS )

if    ( BAYEUX_WITH_GENVTX )
  set ( BAYEUX_WITH_GEOMTOOLS 1 )
endif ( BAYEUX_WITH_GENVTX )

if    ( BAYEUX_WITH_GENBB_HELP )
  set ( BAYEUX_WITH_GEOMTOOLS 1 )
endif ( BAYEUX_WITH_GENBB_HELP )

if    ( BAYEUX_WITH_GEOMTOOLS )
  set ( BAYEUX_WITH_MYGSL 1 )
endif ( BAYEUX_WITH_GEOMTOOLS )
 
# -------------------------------------------------------------------------
# -- NOW ADD EXTERNAL PACKAGES --
# -------------------------------------------------------------------------

message ( STATUS "Bayeux Components :" )
message ( STATUS " o BAYEUX_WITH_DATATOOLS  = ${BAYEUX_WITH_DATATOOLS}"  )
message ( STATUS " o BAYEUX_WITH_MYGSL      = ${BAYEUX_WITH_MYGSL}"      )
message ( STATUS " o BAYEUX_WITH_BRIO       = ${BAYEUX_WITH_BRIO}"       )
message ( STATUS " o BAYEUX_WITH_CUTS       = ${BAYEUX_WITH_CUTS}"       )
message ( STATUS " o BAYEUX_WITH_GEOMTOOLS  = ${BAYEUX_WITH_GEOMTOOLS}"  )
message ( STATUS " o BAYEUX_WITH_MATERIALS  = ${BAYEUX_WITH_MATERIALS}"  )
message ( STATUS " o BAYEUX_WITH_GENVTX     = ${BAYEUX_WITH_GENVTX}"     )
message ( STATUS " o BAYEUX_WITH_GENBB_HELP = ${BAYEUX_WITH_GENBB_HELP}" )

set ( BAYEUX_COMPONENTS "" )

foreach ( component ${BAYEUX_KNOWN_COMPONENTS} )
    string ( TOUPPER "${component}" component_upper )
  
    if ( BAYEUX_WITH_${component_upper} )
        list (APPEND BAYEUX_COMPONENTS ${component} )
    endif ()
endforeach ()
message ( STATUS " BAYEUX_COMPONENTS : '${BAYEUX_COMPONENTS}'" )

include ( BayeuxDatatools  )
include ( BayeuxBrio       )
include ( BayeuxMygsl      )
include ( BayeuxCuts       )
include ( BayeuxGeomtools  )
include ( BayeuxMaterials  )
include ( BayeuxGenvtx     )
include ( BayeuxGenbb_help )

# -------------------------------------------------------------------------
# - Bash setup script :
#
message ( STATUS " BAYEUX_COMPONENTS : '${BAYEUX_COMPONENTS}'" )

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/bayeux_setup.sh.in"
                 "${PROJECT_BINARY_DIR}/cmake/bayeux_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/bayeux_setup.sh
          DESTINATION ${CMAKE_INSTALL_ETCDIR}
          PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        )

# -------------------------------------------------------------------------
# - Cleaning :
#

unset ( _svn_username )
unset ( _svn_password )
unset ( _install_prefix )
unset ( _is_apple )

# - end of Top level CmakeLists.txt for Bayeux Project
