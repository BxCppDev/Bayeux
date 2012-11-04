# -*- mode: cmake; -*-
# mygsl_ThirdPartyDependencies.cmake 
#
# Sample code for 'checking' usual third-party libraries as defined in Cadfael:
#
# Here we are forced to use macros because we need global scope to retrieve
# xxx_INCLUDE_DIRS and xxx_LIBRARIES values. Unfortunately, we also need many
# intermediate variables within these macros and they all have global scope too 
# so we end up to mess ALL these variables from several macros ! 
# Argh !!! The only hack we can use is to prefix all these variables with some
# long prefix to avoid or limit name collision.

# - The list of currently known/supported third-party software that can be handled
#   by the automatic processing of dependencies, getting cflags/ldflags for the 
#   CMake build stage:
set ( mygsl_THIRD_PARTY_KNOWN_SOFTWARE 
  Python 
  GSL 
  CLHEP 
  Gnuplot 
  XercesC 
  Boost 
  ROOT 
  Geant4 
  SystemC
  )

# To be added :
# - MySQL
# - CAMP 


#  -------------------------------------------------------------------------
# Python - mygsl_third_party_dependencies_check_Python :
#
macro (mygsl_third_party_dependencies_check_Python infos_ min_version_ max_version_ exact_version_ )
  set ( __mygsl_TPDC_Python_min_version_   ${min_version_}   )
  set ( __mygsl_TPDC_Python_max_version_   ${max_version_}   )
  set ( __mygsl_TPDC_Python_exact_version_ ${exact_version_} )
  set ( __mygsl_TPDC_Python_requested_version_ ${__mygsl_TPDC_Python_min_version_} )
  if ( NOT "x${__mygsl_TPDC_Python_exact_version_}" STREQUAL "x-" )
    set ( __mygsl_TPDC_Python_requested_version_ ${__mygsl_TPDC_Python_exact_version_} )
  endif ()

  set ( CMAKE_USE_PYTHON_VERSION "2.7" "2.6" )
  find_package ( PythonInterp ${__mygsl_TPDC_Python_requested_version_} REQUIRED )
  if ( PYTHONINTERP_FOUND )
    message ( STATUS "${PROJECT_NAME}: Found PYTHON_EXECUTABLE='${PYTHON_EXECUTABLE}'")
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} --version
      ERROR_VARIABLE __mygsl_TPDC_Python_current_version__info
      OUTPUT_QUIET
      ERROR_STRIP_TRAILING_WHITESPACE
      )
    EXECUTE_PROCESS(COMMAND echo ${__mygsl_TPDC_Python_current_version__info}
      COMMAND tr -d "[[:alpha:]][[:space:]]"
      OUTPUT_VARIABLE __mygsl_TPDC_Python_current_version_
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    set ( Python_VERSION ${__mygsl_TPDC_Python_min_version_} )
      if ( DEFINED __mygsl_TPDC_Python_current_version_ AND NOT "x${__mygsl_TPDC_Python_current_version_}" STREQUAL "x" )
      set ( Python_VERSION ${__mygsl_TPDC_Python_current_version_} )
    endif ()
    message ( STATUS "${PROJECT_NAME}: Python_VERSION='${Python_VERSION}'")

    if ( DEFINED Python_VERSION )
      if ( NOT "x${__mygsl_TPDC_Python_exact_version_}" STREQUAL "x-"
	  AND NOT Python_VERSION VERSION_EQUAL __mygsl_TPDC_Python_exact_version_ 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Python interpreter ${Python_VERSION} != ${__mygsl_TPDC_Python_exact_version_} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_Python_max_version_}" STREQUAL "x-"
	  AND Python_VERSION VERSION_GREATER __mygsl_TPDC_Python_max_version_ 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Python interpreter ${Python_VERSION} > ${__mygsl_TPDC_Python_max_version_} interpreter !" )
      endif ()
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found Python interpreter :")
    if ( DEFINED Python_VERSION )
      message ( STATUS "${PROJECT_NAME}:  o Python_VERSION    = ${Python_VERSION}" )
    endif ()
    message ( STATUS "${PROJECT_NAME}:  o PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}" )
    set ( Python_EXECUTABLE ${PYTHON_EXECUTABLE} )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find Python >= ${__mygsl_TPDC_Python_min_version_} interpreter !" )
  endif ()

  find_package ( PythonLibs ${__mygsl_TPDC_Python_requested_version_} REQUIRED )
  if ( PYTHONLIBS_FOUND )
    message ( STATUS "${PROJECT_NAME}: Found Python library :")
    message ( STATUS "${PROJECT_NAME}:  o PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}" )
    message ( STATUS "${PROJECT_NAME}:  o PYTHON_LIBRARIES    = ${PYTHON_LIBRARIES}" )
    set ( Python_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} )
    set ( Python_LIBRARIES    ${PYTHON_LIBRARIES} )
    set ( ${infos_} "Python:${Python_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find Python >= ${__mygsl_TPDC_Python_min_version_} library !" )
  endif ()

  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_Python_current_version__info )
  unset ( __mygsl_TPDC_Python_current_version_ )
  unset ( __mygsl_TPDC_Python_requested_version_ )
  unset ( __mygsl_TPDC_Python_exact_version_ )
  unset ( __mygsl_TPDC_Python_min_version_ )
  unset ( __mygsl_TPDC_Python_max_version_ )
endmacro (mygsl_third_party_dependencies_check_Python)


# -------------------------------------------------------------------------
# GSL - mygsl_third_party_dependencies_check_GSL :
#
macro (mygsl_third_party_dependencies_check_GSL infos_ min_version_ max_version_ exact_version_)
  set ( __mygsl_TPDC_GSL_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_GSL_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_GSL_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_GSL_requested_version ${__mygsl_TPDC_GSL_min_version} )
  if ( NOT "x${__mygsl_TPDC_GSL_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_GSL_requested_version ${__mygsl_TPDC_GSL_exact_version} )
  endif ()
 
  find_package ( GSL ${__mygsl_TPDC_GSL_requested_version} REQUIRED )
  if ( GSL_FOUND )

    if ( DEFINED GSL_VERSION )
      if ( NOT "x${__mygsl_TPDC_GSL_exact_version}" STREQUAL "x-"
	  AND NOT GSL_VERSION VERSION_EQUAL __mygsl_TPDC_GSL_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching GSL ${GSL_VERSION} != ${__mygsl_TPDC_GSL_exact_version} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_GSL_max_version}" STREQUAL "x-"
	  AND GSL_VERSION VERSION_GREATER __mygsl_TPDC_GSL_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching GSL ${GSL_VERSION} > ${__mygsl_TPDC_GSL_max_version} interpreter !" )
      endif ()
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found GSL :" )
    message ( STATUS "${PROJECT_NAME}:  o GSL_VERSION      : ${GSL_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o GSL_INCLUDE_DIRS : ${GSL_INCLUDE_DIRS}" )
    message ( STATUS "${PROJECT_NAME}:  o GSL_LIBRARIES    : ${GSL_LIBRARIES}" )
    set ( ${infos_} "GSL:${GSL_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find GSL >= ${__mygsl_TPDC_GSL_min_version} library !" )
  endif ()
  
  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_GSL_min_version )
  unset ( __mygsl_TPDC_GSL_max_version )
  unset ( __mygsl_TPDC_GSL_exact_version )
  unset ( __mygsl_TPDC_GSL_requested_version )
endmacro (mygsl_third_party_dependencies_check_GSL)


# -------------------------------------------------------------------------
# Gnuplot - mygsl_third_party_dependencies_check_Gnuplot :
#
macro (mygsl_third_party_dependencies_check_Gnuplot infos_ min_version_ max_version_ exact_version_)
  set ( __mygsl_TPDC_Gnuplot_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_Gnuplot_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_Gnuplot_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_Gnuplot_requested_version ${__mygsl_TPDC_Gnuplot_min_version} )
  if ( NOT "x${__mygsl_TPDC_Gnuplot_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_Gnuplot_requested_version ${__mygsl_TPDC_Gnuplot_exact_version} )
  endif ()
  
  find_package ( Gnuplot ${__mygsl_TPDC_Gnuplot_requested_version} REQUIRED )
  if ( GNUPLOT_FOUND )
    EXECUTE_PROCESS(COMMAND ${GNUPLOT_EXECUTABLE} --version 
      COMMAND sed "s/patchlevel/./g" 
      COMMAND tr -d "[[:alpha:]][[:space:]]"
      OUTPUT_VARIABLE __mygsl_TPDC_Gnuplot_current_version
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if ( DEFINED __mygsl_TPDC_Gnuplot_current_version AND NOT "x${__mygsl_TPDC_Gnuplot_current_version}" STREQUAL "x" )
      set ( GNUPLOT_VERSION ${__mygsl_TPDC_Gnuplot_current_version} )
      set ( Gnuplot_VERSION ${__mygsl_TPDC_Gnuplot_current_version} )
      set ( Gnuplot_EXECUTABLE ${GNUPLOT_EXECUTABLE} )
    endif () 

    if ( DEFINED Gnuplot_VERSION )
      if ( NOT "x${__mygsl_TPDC_Gnuplot_exact_version}" STREQUAL "x-"
	  AND NOT Gnuplot_VERSION VERSION_EQUAL __mygsl_TPDC_Gnuplot_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Gnuplot ${Gnuplot_VERSION} != ${__mygsl_TPDC_Gnuplot_exact_version} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_Gnuplot_max_version}" STREQUAL "x-"
	  AND Gnuplot_VERSION VERSION_GREATER __mygsl_TPDC_Gnuplot_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Gnuplot ${Gnuplot_VERSION} > ${__mygsl_TPDC_Gnuplot_max_version} interpreter !" )
      endif ()
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found Gnuplot :" )
    if ( DEFINED Gnuplot_VERSION )
      message ( STATUS "${PROJECT_NAME}:  o Gnuplot_VERSION     : ${Gnuplot_VERSION}" )
    endif ()
    message ( STATUS "${PROJECT_NAME}:  o Gnuplot_EXECUTABLE : ${Gnuplot_EXECUTABLE}" )
    set ( ${infos_} "Gnuplot:${Gnuplot_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find Gnuplot >= ${__mygsl_TPDC_Gnuplot_min_version} library !" )
  endif ()
  
  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_Gnuplot_min_version )
  unset ( __mygsl_TPDC_Gnuplot_max_version )
  unset ( __mygsl_TPDC_Gnuplot_exact_version )
  unset ( __mygsl_TPDC_Gnuplot_requested_version )
  unset ( __mygsl_TPDC_Gnuplot_current_version )
endmacro (mygsl_third_party_dependencies_check_Gnuplot)


# -------------------------------------------------------------------------
# CLHEP - mygsl_third_party_dependencies_check_CLHEP :
#
macro (mygsl_third_party_dependencies_check_CLHEP infos_ min_version_ max_version_ exact_version_)
  set ( __mygsl_TPDC_CLHEP_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_CLHEP_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_CLHEP_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_CLHEP_requested_version ${__mygsl_TPDC_CLHEP_min_version} )
  if ( NOT "x${__mygsl_TPDC_CLHEP_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_CLHEP_requested_version ${__mygsl_TPDC_CLHEP_exact_version} )
  endif ()
  
  find_package ( CLHEP ${__mygsl_TPDC_CLHEP_requested_version} REQUIRED )
  if ( CLHEP_FOUND )

    if ( DEFINED CLHEP_VERSION )
      if ( NOT "x${__mygsl_TPDC_CLHEP_exact_version}" STREQUAL "x-"
	  AND NOT CLHEP_VERSION VERSION_EQUAL __mygsl_TPDC_CLHEP_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching CLHEP ${CLHEP_VERSION} != ${__mygsl_TPDC_CLHEP_exact_version} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_CLHEP_max_version}" STREQUAL "x-"
	  AND CLHEP_VERSION VERSION_GREATER __mygsl_TPDC_CLHEP_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching CLHEP ${CLHEP_VERSION} > ${__mygsl_TPDC_CLHEP_max_version} interpreter !" )
      endif ()
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found CLHEP :" )
    message ( STATUS "${PROJECT_NAME}:  o CLHEP_VERSION      : ${CLHEP_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o CLHEP_INCLUDE_DIRS : ${CLHEP_INCLUDE_DIRS}" )
    message ( STATUS "${PROJECT_NAME}:  o CLHEP_LIBRARIES    : ${CLHEP_LIBRARIES}" )
    set ( ${infos_} "CLHEP:${CLHEP_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find CLHEP >= ${__mygsl_TPDC_CLHEP_min_version} library !" )
  endif ()
  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_CLHEP_min_version )
  unset ( __mygsl_TPDC_CLHEP_max_version )
  unset ( __mygsl_TPDC_CLHEP_exact_version )
  unset ( __mygsl_TPDC_CLHEP_requested_version )
endmacro (mygsl_third_party_dependencies_check_CLHEP)


# -------------------------------------------------------------------------
# Xerces-C++ - mygsl_third_party_dependencies_check_XercesC :
#
macro (mygsl_third_party_dependencies_check_XercesC infos_ min_version_ max_version_ exact_version_)
  set ( __mygsl_TPDC_XercesC_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_XercesC_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_XercesC_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_XercesC_requested_version ${__mygsl_TPDC_XercesC_min_version} )
  if ( NOT "x${__mygsl_TPDC_XercesC_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_XercesC_requested_version ${__mygsl_TPDC_XercesC_exact_version} )
  endif ()
  
  find_package ( XercesC ${__mygsl_TPDC_XercesC_requested_version} REQUIRED )
  if ( XERCESC_FOUND )

    if ( DEFINED XercesC_VERSION )
      if ( NOT "x${__mygsl_TPDC_XercesC_exact_version}" STREQUAL "x-"
	  AND NOT XercesC_VERSION VERSION_EQUAL __mygsl_TPDC_XercesC_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching XercesC ${XercesC_VERSION} != ${__mygsl_TPDC_XercesC_exact_version} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_XercesC_max_version}" STREQUAL "x-"
	  AND XercesC_VERSION VERSION_GREATER __mygsl_TPDC_XercesC_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching XercesC ${XercesC_VERSION} > ${__mygsl_TPDC_XercesC_max_version} interpreter !" )
      endif ()
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found XercesC :" )
    message ( STATUS "${PROJECT_NAME}:  o XercesC_VERSION      : ${XercesC_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o XercesC_INCLUDE_DIRS : ${XercesC_INCLUDE_DIRS}" )
    message ( STATUS "${PROJECT_NAME}:  o XercesC_LIBRARIES    : ${XercesC_LIBRARIES}" )
    set ( ${infos_} "XercesC:${XercesC_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find XercesC >= ${__mygsl_TPDC_XercesC_min_version} library !" )
  endif ()

  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_XercesC_min_version )
  unset ( __mygsl_TPDC_XercesC_max_version )
  unset ( __mygsl_TPDC_XercesC_exact_version )
  unset ( __mygsl_TPDC_XercesC_requested_version )
endmacro (mygsl_third_party_dependencies_check_XercesC)


# -------------------------------------------------------------------------
# Boost - mygsl_third_party_dependencies_check_Boost :
#
macro (mygsl_third_party_dependencies_check_Boost infos_ min_version_ max_version_ exact_version_ )
  set ( __mygsl_TPDC_Boost_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_Boost_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_Boost_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_Boost_requested_version ${__mygsl_TPDC_Boost_min_version} )
  if ( NOT "x${__mygsl_TPDC_Boost_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_Boost_requested_version ${__mygsl_TPDC_Boost_exact_version} )
  endif ()

  string ( REGEX REPLACE "[0-9]" "" __mygsl_TPDC_Boost_boost_check_version "${__mygsl_TPDC_Boost_min_version}" )
  if ( ${__mygsl_TPDC_Boost_boost_check_version} STREQUAL "." )
    set ( __mygsl_TPDC_Boost_min_version "${min_version_}.0" )
  elseif (NOT ${__mygsl_TPDC_Boost_boost_check_version} STREQUAL ".." )
    set ( __mygsl_TPDC_Boost_min_version "1.47.0" )
  endif()
  if ( NOT "x${max_version_}" STREQUAL "x" )
    set ( __mygsl_TPDC_Boost_max_version ${max_version_} )
  endif ()
  # if ( NOT "x${requested_components_}" STREQUAL "x" )
  #   set ( __boost_requested_components "" ) ###${requested_components_} )
  # endif ()
  # if ( "x${requested_components_}" STREQUAL "x" )
  # endif ()
  # if (DEFINED BOOST_COMPONENTS )
  #   set ( __boost_requested_components ${BOOST_COMPONENTS} )
  # else ()
  #   set ( __boost_requested_components filesystem serialization iostreams python system program_options )
  # endif ()
  set ( __mygsl_TPDC_Boost_boost_requested_components filesystem serialization iostreams python system program_options thread )

  # first try :
  if ( NOT DEFINED BOOST_ROOT )  
    if ( NOT "$ENV{BOOST_ROOT_DIR}" STREQUAL "" )  
      message ( STATUS "${PROJECT_NAME}: Environment BOOST_ROOT_DIR exists : '$ENV{BOOST_ROOT_DIR}' " )
      set ( BOOST_ROOT $ENV{BOOST_ROOT_DIR} )
     else ()
      message ( STATUS  "${PROJECT_NAME}: WARNING: Environment BOOST_ROOT_DIR is not set !" )
    endif ()
  endif ()
  # second chance :
  if ( NOT DEFINED BOOST_ROOT )  
    if ( NOT "$ENV{BOOST_ROOT}" STREQUAL "" )  
      message ( STATUS "${PROJECT_NAME}: Environment BOOST_ROOT exists : '$ENV{BOOST_ROOT}' " )
      set ( BOOST_ROOT $ENV{BOOST_ROOT} )
    else ()
      message ( STATUS  "${PROJECT_NAME}: WARNING: Environment BOOST_ROOT is not set !" )
    endif ()
  endif ()
  
  string ( REGEX REPLACE ".[0-9]+$" "" __mygsl_TPDC_Boost_boost_short_version "${__mygsl_TPDC_Boost_min_version}" )
  message ( STATUS "${PROJECT_NAME}: Boost short version is : '${__mygsl_TPDC_Boost_boost_short_version}'" )
  set ( Boost_ADDITIONAL_VERSIONS ${__mygsl_TPDC_Boost_boost_short_version} ${__mygsl_TPDC_Boost_min_version} )
  set ( Boost_NO_SYSTEM_PATHS ON )

  find_package ( Boost ${__mygsl_TPDC_Boost_requested_version} REQUIRED COMPONENTS ${__mygsl_TPDC_Boost_boost_requested_components} )
  if ( Boost_FOUND )

    if ( DEFINED Boost_VERSION )
      set ( __mygsl_TPDC_Boost_check_version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} )
      if ( NOT "x${__mygsl_TPDC_Boost_exact_version}" STREQUAL "x-"
	  AND NOT __mygsl_TPDC_Boost_check_version VERSION_EQUAL __mygsl_TPDC_Boost_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Boost ${__mygsl_TPDC_Boost_check_version} != ${__mygsl_TPDC_Boost_exact_version} !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_Boost_max_version}" STREQUAL "x-"
	  AND Boost_VERSION VERSION_GREATER __mygsl_TPDC_Boost_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Boost ${__mygsl_TPDC_Boost_check_version} > ${__mygsl_TPDC_Boost_max_version} !" )
      endif ()
    else ()
	message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find a valid Boost version number !" )      
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found Boost :")
    message ( STATUS "${PROJECT_NAME}:  o Boost_VERSION          = ${Boost_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o Boost_LIB_VERSION      = ${Boost_LIB_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o Boost_INCLUDE_DIRS     = ${Boost_INCLUDE_DIRS}" )
    message ( STATUS "${PROJECT_NAME}:  o Boost_LIBRARIES        = ${Boost_LIBRARIES}" )
    # Special trick to address the Boost version standardized string : 
    #   Boost_MAJOR_VERSION                 major version number of boost
    #   Boost_MINOR_VERSION                 minor version number of boost
    #   Boost_SUBMINOR_VERSION              subminor version number of boost
    set ( Boost_VERSION_Mms "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" )
    set ( ${infos_} "Boost:${Boost_VERSION_Mms}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find Boost >= ${__mygsl_TPDC_Boost_min_version} libraries !" )
  endif ()

  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_Boost_boost_short_version )
  unset ( __mygsl_TPDC_Boost_boost_requested_components )
  unset ( __mygsl_TPDC_Boost_min_version )
  unset ( __mygsl_TPDC_Boost_max_version )
  unset ( __mygsl_TPDC_Boost_exact_version )
  unset ( __mygsl_TPDC_Boost_requested_version )
  unset ( __mygsl_TPDC_Boost_check_version )
endmacro (mygsl_third_party_dependencies_check_Boost)


# -------------------------------------------------------------------------
# Geant4 - mygsl_third_party_dependencies_check_Geant4 :
#
macro (mygsl_third_party_dependencies_check_Geant4 infos_ min_version_ max_version_ exact_version_ )
  set ( __mygsl_TPDC_Geant4_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_Geant4_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_Geant4_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_Geant4_requested_version ${__mygsl_TPDC_Geant4_min_version} )
  if ( NOT "x${__mygsl_TPDC_Geant4_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_Geant4_requested_version ${__mygsl_TPDC_Geant4_exact_version} )
  endif ()

  if ( NOT DEFINED Geant4_DIR )  
    if ( NOT "$ENV{GEANT4_ROOT_DIR}" STREQUAL "" )  
      message ( STATUS "${PROJECT_NAME}: Environment GEANT4_ROOT_DIR exists : '$ENV{GEANT4_ROOT_DIR}' " )
      set ( Geant4_DIR $ENV{GEANT4_ROOT_DIR} )
    else ()
      message ( STATUS "${PROJECT_NAME}: WARNING: Environment GEANT4_ROOT_DIR is not set !" )
    endif ()

    if ( NOT DEFINED Geant4_DIR )  
      message ( FATAL_ERROR "${PROJECT_NAME}: Geant4_DIR is not set !" )
    endif ()

  endif ()
  
  find_package ( Geant4 ${__mygsl_TPDC_Geant4_requested_version} REQUIRED NO_MODULE
    PATHS ${Geant4_DIR} PATH_SUFFIXES lib64 lib
    )
  if ( Geant4_FOUND )
 
    if ( DEFINED Geant4_VERSION )
      if ( NOT "x${__mygsl_TPDC_Geant4_exact_version}" STREQUAL "x-"
	  AND NOT Geant4_VERSION VERSION_EQUAL __mygsl_TPDC_Geant4_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Geant4 ${Geant4_VERSION} != ${__mygsl_TPDC_Geant4_exact_version} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_Geant4_max_version}" STREQUAL "x-"
	  AND Geant4_VERSION VERSION_GREATER __mygsl_TPDC_Geant4_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching Geant4 ${Geant4_VERSION} > ${__mygsl_TPDC_Geant4_max_version} interpreter !" )
      endif ()
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found Geant4 :" )
    message ( STATUS "${PROJECT_NAME}:  o Geant4_VERSION      : ${Geant4_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o Geant4_INCLUDE_DIRS : ${Geant4_INCLUDE_DIRS}" )
    message ( STATUS "${PROJECT_NAME}:  o Geant4_LIBRARIES    : ${Geant4_LIBRARIES}" )
    message ( STATUS "${PROJECT_NAME}:  o Geant4_USE_FILE     : ${Geant4_USE_FILE}" )
    set ( ${infos_} "Geant4:${Geant4_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find Geant4 >= ${__mygsl_TPDC_Geant4_min_version} library !" )
  endif ()

  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_Geant4_min_version )
  unset ( __mygsl_TPDC_Geant4_max_version )
  unset ( __mygsl_TPDC_Geant4_exact_version )
  unset ( __mygsl_TPDC_Geant4_requested_version )
endmacro (mygsl_third_party_dependencies_check_Geant4)

# -------------------------------------------------------------------------
# SystemC - mygsl_third_party_dependencies_check_SystemC :
#
macro (mygsl_third_party_dependencies_check_SystemC infos_ min_version_ max_version_ exact_version_ )
  set ( __mygsl_TPDC_SystemC_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_SystemC_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_SystemC_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_SystemC_requested_version ${__mygsl_TPDC_SystemC_min_version} )
  if ( NOT "x${__mygsl_TPDC_SystemC_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_SystemC_requested_version ${__mygsl_TPDC_SystemC_exact_version} )
  endif ()
  
  find_package ( SystemC ${__mygsl_TPDC_SystemC_requested_version} REQUIRED )
  if ( SYSTEMC_FOUND )
 
    if ( DEFINED SystemC_VERSION )
      if ( NOT "x${__mygsl_TPDC_SystemC_exact_version}" STREQUAL "x-"
	  AND NOT SystemC_VERSION VERSION_EQUAL __mygsl_TPDC_SystemC_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching SystemC ${SystemC_VERSION} != ${__mygsl_TPDC_SystemC_exact_version} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_SystemC_max_version}" STREQUAL "x-"
	  AND SystemC_VERSION VERSION_GREATER __mygsl_TPDC_SystemC_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching SystemC ${SystemC_VERSION} > ${__mygsl_TPDC_SystemC_max_version} interpreter !" )
      endif ()
    endif ()

    message ( STATUS "${PROJECT_NAME}: Found SystemC :" )
    message ( STATUS "${PROJECT_NAME}:  o SystemC_VERSION      : ${SystemC_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o SystemC_INCLUDE_DIRS : ${SystemC_INCLUDE_DIRS}" )
    message ( STATUS "${PROJECT_NAME}:  o SystemC_LIBRARIES    : ${SystemC_LIBRARIES}" )
    set ( ${infos_} "SystemC:${SystemC_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find SystemC >= ${__mygsl_TPDC_SystemC_min_version} library !" )
  endif ()

  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_SystemC_min_version )
  unset ( __mygsl_TPDC_SystemC_max_version )
  unset ( __mygsl_TPDC_SystemC_exact_version )
  unset ( __mygsl_TPDC_SystemC_requested_version )
endmacro (mygsl_third_party_dependencies_check_SystemC)

# -------------------------------------------------------------------------
# ROOT - mygsl_third_party_dependencies_check_ROOT :
#
macro (mygsl_third_party_dependencies_check_ROOT infos_ min_version_ max_version_ exact_version_ )
  set ( __mygsl_TPDC_ROOT_min_version   ${min_version_}   )
  set ( __mygsl_TPDC_ROOT_max_version   ${max_version_}   )
  set ( __mygsl_TPDC_ROOT_exact_version ${exact_version_} )
  set ( __mygsl_TPDC_ROOT_requested_version ${__mygsl_TPDC_ROOT_min_version} )
  if ( NOT "x${__mygsl_TPDC_ROOT_exact_version}" STREQUAL "x-" )
    set ( __mygsl_TPDC_ROOT_requested_version ${__mygsl_TPDC_ROOT_exact_version} )
  endif ()

  if ( NOT DEFINED ROOT_DIR )  
    if ( NOT "$ENV{ROOT_ROOT_DIR}" STREQUAL "")  
      message ( STATUS "${PROJECT_NAME}: Environment ROOT_ROOT_DIR exists : '$ENV{ROOT_ROOT_DIR}' ")
      set ( ROOT_DIR $ENV{ROOT_ROOT_DIR} )
    else ()
      message ( STATUS  "${PROJECT_NAME}: WARNING: Environment ROOT_ROOT_DIR is not set !" )
      if ( NOT "$ENV{ROOTSYS}" STREQUAL "")  
	message ( STATUS "${PROJECT_NAME}: Environment ROOTSYS exists : '$ENV{ROOTSYS}' ")
	set ( ROOT_DIR $ENV{ROOTSYS} )
      else ()
	message ( STATUS  "${PROJECT_NAME}: WARNING: Environment ROOTSYS is not set !" )
      endif ()
    endif ()
  endif ()
  ###message ( STATUS "DEVEL: Checking ROOT === '${__mygsl_TPDC_ROOT_requested_version}'" )
  
  find_package ( ROOT ${__mygsl_TPDC_ROOT_requested_version} REQUIRED )
  if ( ROOT_FOUND )

    string (REPLACE "/" "." ROOT_VERSION ${ROOTVERSION} )
    string (REPLACE ".00" ".0" ROOT_VERSION ${ROOT_VERSION} )
    string (REGEX REPLACE "[a-z]" "" ROOT_VERSION ${ROOT_VERSION} )
 
    if ( DEFINED ROOT_VERSION )
      if ( NOT "x${__mygsl_TPDC_ROOT_exact_version}" STREQUAL "x-"
	  AND NOT ROOT_VERSION VERSION_EQUAL __mygsl_TPDC_ROOT_exact_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching ROOT ${ROOT_VERSION} != ${__mygsl_TPDC_ROOT_exact_version} interpreter !" )
      endif ()
      if ( NOT "x${__mygsl_TPDC_ROOT_max_version}" STREQUAL "x-"
	  AND ROOT_VERSION VERSION_GREATER __mygsl_TPDC_ROOT_max_version 
	  )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found unmatching ROOT ${ROOT_VERSION} > ${__mygsl_TPDC_ROOT_max_version} interpreter !" )
      endif ()
    endif ()
    
    message ( STATUS "${PROJECT_NAME}: Found ROOT :")
    message ( STATUS "${PROJECT_NAME}:  o ROOTVERSION            = ${ROOTVERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o ROOT_VERSION           = ${ROOT_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o ROOT_CONFIG_EXECUTABLE = ${ROOT_CONFIG_EXECUTABLE}" )
    message ( STATUS "${PROJECT_NAME}:  o ROOT_CINT_EXECUTABLE   = ${ROOT_CINT_EXECUTABLE}" )
    message ( STATUS "${PROJECT_NAME}:  o ROOT_INCLUDE_DIR       = ${ROOT_INCLUDE_DIR}" )
    message ( STATUS "${PROJECT_NAME}:  o ROOT_LIBRARIES         = ${ROOT_LIBRARIES}" )
    set ( ${infos_} "ROOT:${ROOT_VERSION}" )
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find ROOT >= ${__mygsl_TPDC_ROOT_requested_version} library !" )
  endif ()
  
  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_ROOT_min_version )
  unset ( __mygsl_TPDC_ROOT_max_version )
  unset ( __mygsl_TPDC_ROOT_exact_version )
  unset ( __mygsl_TPDC_ROOT_requested_version )
endmacro (mygsl_third_party_dependencies_check_ROOT)

# -------------------------------------------------------------------------
# - mygsl_third_party_dependencies_check :
#
macro (mygsl_third_party_dependencies_check 
    sw_name_ 
    sw_infos_
    sw_min_version_ 
    sw_max_version_
    sw_exact_version_ )
  set ( __mygsl_TPDC_sw_name ${sw_name_} )
  set ( __mygsl_TPDC_sw_min_version ${sw_min_version_} )
  set ( __mygsl_TPDC_sw_max_version ${sw_max_version_} )
  set ( __mygsl_TPDC_sw_exact_version ${sw_exact_version_} )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_third_party_dependencies_check for ${__mygsl_TPDC_sw_name} -- start      " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * Package min version   : ${__mygsl_TPDC_sw_min_version}   " )
  message ( STATUS "${PROJECT_NAME}: * Package max version   : ${__mygsl_TPDC_sw_max_version}   " )
  message ( STATUS "${PROJECT_NAME}: * Package exact version : ${__mygsl_TPDC_sw_exact_version} " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )

  string (TOLOWER "${__mygsl_TPDC_sw_name}" __mygsl_TPDC_sw_name_lower )
  list (FIND mygsl_THIRD_PARTY_KNOWN_SOFTWARE ${__mygsl_TPDC_sw_name} __mygsl_TPDC_find_index)
  # Default value is '-' (invalid):
  set ( ${sw_infos_} "-" )
  if ( __mygsl_TPDC_find_index EQUAL -1 )
    message ( STATUS "${PROJECT_NAME}: WARNING: No support for checking third party dependency '${__mygsl_TPDC_sw_name}' !" )
  else ()
    message ( STATUS "${PROJECT_NAME}: NOTICE: Checking procedure for third party dependency '${__mygsl_TPDC_sw_name}:${__mygsl_TPDC_sw_min_version},${__mygsl_TPDC_sw_max_version},${__mygsl_TPDC_sw_exact_version}'..." )
    if ( __mygsl_TPDC_sw_name STREQUAL "Python" ) 
      mygsl_third_party_dependencies_check_Python ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "GSL" ) 
      mygsl_third_party_dependencies_check_GSL ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "CLHEP" ) 
      mygsl_third_party_dependencies_check_CLHEP ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "Gnuplot" ) 
      mygsl_third_party_dependencies_check_Gnuplot ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "XercesC" ) 
      mygsl_third_party_dependencies_check_XercesC ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "Boost" ) 
      mygsl_third_party_dependencies_check_Boost ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "ROOT" ) 
      mygsl_third_party_dependencies_check_ROOT ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "Geant4" ) 
      mygsl_third_party_dependencies_check_Geant4 ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    elseif ( __mygsl_TPDC_sw_name STREQUAL "SystemC" ) 
      mygsl_third_party_dependencies_check_SystemC ( "${sw_infos_}" 
	${__mygsl_TPDC_sw_min_version} ${__mygsl_TPDC_sw_max_version} ${__mygsl_TPDC_sw_exact_version} )
    else ()
      message ( STATUS "${PROJECT_NAME}: WARNING: No available checking procedure for third party dependency '${__mygsl_TPDC_sw_name}'!" )
    endif ()
  endif ()

  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_third_party_dependencies_check for ${__mygsl_TPDC_sw_name} -- stop" )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )

  # clear pseudo-local variables :
  unset ( __mygsl_TPDC_sw_name_lower )
  unset ( __mygsl_TPDC_sw_name )
  unset ( __mygsl_TPDC_sw_min_version )
  unset ( __mygsl_TPDC_sw_max_version )
  unset ( __mygsl_TPDC_find_index )
endmacro (mygsl_third_party_dependencies_check)

# -------------------------------------------------------------------------
# mygsl_third_party_dependencies_check_list :
#  - input  : list_of_third_party_dependencies_rules_
#  - output : list_of_third_party_dependencies_infos_
macro (mygsl_third_party_dependencies_check_list 
    list_of_third_party_dependencies_rules_
    list_of_third_party_dependencies_infos_
    )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_third_party_dependencies_check_list -- start      " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  foreach ( __tp_dependency_rule ${list_of_third_party_dependencies_rules_} )
    message ( STATUS "${PROJECT_NAME}: NOTICE: Checking third party dependency rule '${__tp_dependency_rule}'" )
    string (REPLACE ":" ";" __mygsl_TPDCL_tp_dependency_tokens "${__tp_dependency_rule}:")
    list (GET __mygsl_TPDCL_tp_dependency_tokens 0 __mygsl_TPDCL_tp_dependency_name )
    list (GET __mygsl_TPDCL_tp_dependency_tokens 1 __mygsl_TPDCL_tp_dependency_version_rules )
    string (REPLACE "," ";" 
      __mygsl_TPDCL_tp_dependency_version_rules_tokens 
      "${__mygsl_TPDCL_tp_dependency_version_rules}")
    list (LENGTH __mygsl_TPDCL_tp_dependency_version_rules_tokens __mygsl_TPDCL_tp_number_of_rules )
    set (__mygsl_TPDCL_tp_dependency_min_version   "-" )
    set (__mygsl_TPDCL_tp_dependency_max_version   "-" )
    set (__mygsl_TPDCL_tp_dependency_exact_version "-" )
    if ( __mygsl_TPDCL_tp_number_of_rules EQUAL 0)
      set (__mygsl_TPDCL_tp_dependency_min_version   "0.0.0" )
      set (__mygsl_TPDCL_tp_dependency_max_version   "-" )
      set (__mygsl_TPDCL_tp_dependency_exact_version "-" )
    else ()
      if (__mygsl_TPDCL_tp_number_of_rules GREATER 0)
	list (GET __mygsl_TPDCL_tp_dependency_version_rules_tokens 0 __mygsl_TPDCL_tp_dependency_first_rule )
	string(LENGTH "${__mygsl_TPDCL_tp_dependency_first_rule}" __mygsl_TPDCL_length_of_tp_dependency_first_rule)
	if (__mygsl_TPDCL_length_of_tp_dependency_first_rule GREATER 0 )
	  string(SUBSTRING ${__mygsl_TPDCL_tp_dependency_first_rule} 0 1 __mygsl_TPDCL_first_char )
	  if ( "x${__mygsl_TPDCL_first_char}" STREQUAL "x=" )
	    string (REGEX REPLACE "^=" "" __mygsl_TPDCL_tp_dependency_exact_version "${__mygsl_TPDCL_tp_dependency_first_rule}")
	    if (__mygsl_TPDCL_tp_number_of_rules GREATER 1)
	      message ( STATUS "${PROJECT_NAME}: WARNING: Ignoring second version rule in '${__mygsl_TPDCL_tp_dependency_version_rules_tokens}" )
	    endif()
	  else ()
	    if (__mygsl_TPDCL_length_of_tp_dependency_first_rule GREATER 1 ) 
	      string(SUBSTRING ${__mygsl_TPDCL_tp_dependency_first_rule} 0 2 __mygsl_TPDCL_first_two_chars )
	      if ( "x${__mygsl_TPDCL_first_two_chars}" STREQUAL "x>=" )
		string (REGEX REPLACE "^>=" "" __mygsl_TPDCL_tp_dependency_min_version "${__mygsl_TPDCL_tp_dependency_first_rule}")
              elseif ( "x${__mygsl_TPDCL_first_two_chars}" STREQUAL "x<=" ) 
		set ( __mygsl_TPDCL_tp_dependency_min_version "0.0.0" )
		string (REGEX REPLACE "^<=" "" __mygsl_TPDCL_tp_dependency_max_version "${__mygsl_TPDCL_tp_dependency_first_rule}")
	      else ()
		set (__mygsl_TPDCL_tp_dependency_min_version "${__mygsl_TPDCL_tp_dependency_first_rule}" )
	      endif ()
	    else ()
	      set (__mygsl_TPDCL_tp_dependency_min_version "${__mygsl_TPDCL_tp_dependency_first_rule}" )
	    endif ()
	  endif ()
	  if (__mygsl_TPDCL_tp_number_of_rules GREATER 1)
	    list (GET __mygsl_TPDCL_tp_dependency_version_rules_tokens 1 __mygsl_TPDCL_tp_dependency_second_rule )
	    string (REGEX REPLACE "^<=" "" __mygsl_TPDCL_tp_dependency_max_version "${__mygsl_TPDCL_tp_dependency_second_rule}")
	  endif ()
	endif ()
      endif ()
    endif ()
    #message ( STATUS "${PROJECT_NAME}: DEVEL: __mygsl_TPDCL_tp_dependency_min_version   = '${__mygsl_TPDCL_tp_dependency_min_version}' ")
    #message ( STATUS "${PROJECT_NAME}: DEVEL: __mygsl_TPDCL_tp_dependency_max_version   = '${__mygsl_TPDCL_tp_dependency_max_version}' ")
    #message ( STATUS "${PROJECT_NAME}: DEVEL: __mygsl_TPDCL_tp_dependency_exact_version = '${__mygsl_TPDCL_tp_dependency_exact_version}' ")

    # 
    set (__mygsl_TPDCL_tp_dependency_infos "__mygsl_TPDCL_${__tp_dependency_name}_infos" )
    mygsl_third_party_dependencies_check ( ${__mygsl_TPDCL_tp_dependency_name} 
      ${__mygsl_TPDCL_tp_dependency_infos}
      ${__mygsl_TPDCL_tp_dependency_min_version} 
      ${__mygsl_TPDCL_tp_dependency_max_version} 
      ${__mygsl_TPDCL_tp_dependency_exact_version} )
    if ( "x${${__mygsl_TPDCL_tp_dependency_infos}}" STREQUAL "x-" )
      message ( STATUS "${PROJECT_NAME}: WARNING: ******************************************************** " )
      message ( STATUS "${PROJECT_NAME}: WARNING: * You should add specific CMake instructions for " )
      message ( STATUS "${PROJECT_NAME}: WARNING: * checking third-party dependency : '${__mygsl_TPDCL_tp_dependency_rule}'" )
      message ( STATUS "${PROJECT_NAME}: WARNING: ******************************************************** " )
    else ()
      list ( APPEND ${list_of_third_party_dependencies_infos_} "${${__mygsl_TPDCL_tp_dependency_infos}}" ) 
    endif ()

    # clear pseudo-local variables :
    unset ( ${__mygsl_TPDCL_tp_dependency_infos} )
    unset ( __mygsl_TPDCL_tp_dependency_infos )
    unset ( __mygsl_TPDCL_tp_dependency_name )
    unset ( __mygsl_TPDCL_tp_dependency_max_version )
    unset ( __mygsl_TPDCL_tp_dependency_min_version )
    unset ( __mygsl_TPDCL_tp_dependency_tokens )
    unset ( __mygsl_TPDCL_tp_infos )
  endforeach ()
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_third_party_dependencies_check_list -- stop      " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
endmacro (mygsl_third_party_dependencies_check_list)

#
#
#
macro (mygsl_third_party_dependencies_populate_build_lists 
    list_of_tp_dependencies_infos_
    list_of_definitions_ 
    list_of_include_dirs_
    list_of_libraries_ 
    )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_third_party_dependencies_populate_build_lists -- start " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )

  foreach ( __mygsl_TPDPBL_tp_dependency_entry ${list_of_tp_dependencies_infos_} )
    string (REPLACE ":" ";" __mygsl_TPDPBL_tp_dependency_tokens "${__mygsl_TPDPBL_tp_dependency_entry}:")
    list (GET __mygsl_TPDPBL_tp_dependency_tokens 0 __mygsl_TPDPBL_tp_dependency_name    )
    list (GET __mygsl_TPDPBL_tp_dependency_tokens 1 __mygsl_TPDPBL_tp_dependency_version )
    message ( STATUS "${PROJECT_NAME}: NOTICE: Attempt to automate the '${__mygsl_TPDPBL_tp_dependency_name} (${__mygsl_TPDPBL_tp_dependency_version})' third-party dependency"  )

    list (FIND mygsl_THIRD_PARTY_KNOWN_SOFTWARE ${__mygsl_TPDPBL_tp_dependency_name} __mygsl_TPDPBL_find_index)
    if ( __mygsl_TPDPBL_find_index EQUAL -1 )
      message ( STATUS "${PROJECT_NAME}: WARNING: Cannot populate build lists for third party dependency '${__mygsl_TPDPBL_sw_name}' !" )
    else ()
      if (DEFINED ${__mygsl_TPDPBL_tp_dependency_name}_DEFINITIONS 
	  AND ${__mygsl_TPDPBL_tp_dependency_name}_DEFINITIONS
	  AND NOT "x${__mygsl_TPDPBL_tp_dependency_name}_DEFINITIONS" STREQUAL "x" )
	message ( STATUS "${PROJECT_NAME}: NOTICE: Adding definitions from the '${__mygsl_TPDPBL_tp_dependency_name}_DEFINITIONS' variable..."  )
	list (APPEND ${list_of_definitions_} ${${__mygsl_TPDPBL_tp_dependency_name}_DEFINITIONS} ) 
      else ()
	message ( STATUS "${PROJECT_NAME}: NOTICE: No available '${__mygsl_TPDPBL_tp_dependency_name}_DEFINITIONS' variable !"  )
      endif ()

      if (DEFINED ${__mygsl_TPDPBL_tp_dependency_name}_INCLUDE_DIRS 
	  AND ${__mygsl_TPDPBL_tp_dependency_name}_INCLUDE_DIRS
	  AND NOT "x${__mygsl_TPDPBL_tp_dependency_name}_INCLUDE_DIRS" STREQUAL "x"  )
	message ( STATUS "${PROJECT_NAME}: NOTICE: Adding include path from the '${__mygsl_TPDPBL_tp_dependency_name}_INCLUDE_DIRS' variable..."  )
	list (APPEND ${list_of_include_dirs_} ${${__mygsl_TPDPBL_tp_dependency_name}_INCLUDE_DIRS} ) 
      else ()
	message ( STATUS "${PROJECT_NAME}: NOTICE: No available '${__mygsl_TPDPBL_tp_dependency_name}_INCLUDE_DIRS' variable !"  )
      endif ()
      
      if (DEFINED ${__mygsl_TPDPBL_tp_dependency_name}_LIBRARIES
	  AND ${__mygsl_TPDPBL_tp_dependency_name}_LIBRARIES
	  AND NOT "x${__mygsl_TPDPBL_tp_dependency_name}_LIBRARIES" STREQUAL "x"   )
	message ( STATUS "${PROJECT_NAME}: NOTICE: Adding libraries from the '${__mygsl_TPDPBL_tp_dependency_name}_LIBRARIES' variable..."  )
	list ( APPEND ${list_of_libraries_} ${${__mygsl_TPDPBL_tp_dependency_name}_LIBRARIES} ) 
      else ()
	message ( STATUS "${PROJECT_NAME}: NOTICE: No available '${__mygsl_TPDPBL_tp_dependency_name}_LIBRARIES' variable !"  )
      endif ()

    endif ()

    # clear pseudo local variables:
    unset ( __mygsl_TPDPBL_find_index )
    unset ( __mygsl_TPDPBL_tp_dependency_name )
    unset ( __mygsl_TPDPBL_tp_dependency_version )
    unset ( __mygsl_TPDPBL_tp_dependency_tokens )

  endforeach ()
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_third_party_dependencies_populate_build_lists -- stop  " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )

endmacro (mygsl_third_party_dependencies_populate_build_lists)

# end of mygsl_ThirdPartyDependencies.cmake