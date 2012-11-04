# -*- mode: cmake; -*-
# mygsl_HomeMadePackageDependencies.cmake 

# Sample code for 'checking' home-made package libraries as defined in Bayeux, Falaise and Channel:
#
# Here we are forced to use macros because we need global scope to retrieve
# xxx_INCLUDE_DIRS and xxx_LIBRARIES values. Unfortunately, we also need many
# intermediate variables within these macros and they all have global scope too 
# so we end up to mess ALL these variables from several macros ! 
# Argh !!! The only hack we can use is to prefix all these variables with some
# long prefix to avoid or limit name collision.

# -----------------------------------------------------------------------
# - This macro checks if some package named 'package_name_'
# with version >= 'min_version_' can be found on the system.
#
macro (mygsl_home_made_package_dependencies_check 
    mygsl_dependency_name_ 
    mygsl_dependency_min_request_ 
    mygsl_dependency_max_request_ 
    mygsl_dependency_exact_request_ )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_home_made_package_dependencies_check for '${mygsl_dependency_name_}' - start  " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * Package min version request   : ${mygsl_dependency_min_request_}   " )
  message ( STATUS "${PROJECT_NAME}: * Package max version request   : ${mygsl_dependency_max_request_}   " )
  message ( STATUS "${PROJECT_NAME}: * Package exact version request : ${mygsl_dependency_exact_request_} " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  # initializing pseudo-local variables :
  set ( __mygsl_HMPDC_dependency_name        ${mygsl_dependency_name_} )
  set ( __mygsl_HMPDC_dependency_min_request ${mygsl_dependency_min_request_} )
  set ( __mygsl_HMPDC_dependency_max_request ${mygsl_dependency_max_request_} )
  set ( __mygsl_HMPDC_dependency_exact_request ${mygsl_dependency_exact_request_} )
  string ( TOUPPER ${__mygsl_HMPDC_dependency_name} __mygsl_HMPDC_dependency_name_upper )
  ###string ( TOLOWER ${__mygsl_HMPDC_dependency_name} __mygsl_HMPDC_dependency_name_lower )

  if ( DEFINED ${__mygsl_HMPDC_dependency_name}_DIR 
      AND NOT "x${${__mygsl_HMPDC_dependency_name}_DIR}" STREQUAL "x" 
      AND NOT "x${${__mygsl_HMPDC_dependency_name}_DIR}" STREQUAL "x${__mygsl_HMPDC_dependency_name}_DIR-NOTFOUND" )
    message ( STATUS "${PROJECT_NAME}: ${__mygsl_HMPDC_dependency_name}_DIR is : '${${__mygsl_HMPDC_dependency_name}_DIR}' " )
  else ()
    if ( NOT "$ENV{${__mygsl_HMPDC_dependency_name_upper}_CMAKE_CONFIG_DIR}" STREQUAL "" )
      message ( STATUS "${PROJECT_NAME}: Environment ${__mygsl_HMPDC_dependency_name_upper}_CMAKE_CONFIG_DIR exists : '$ENV{${__mygsl_HMPDC_dependency_name_upper}_CMAKE_CONFIG_DIR}' " )
      set ( ${__mygsl_HMPDC_dependency_name}_DIR "$ENV{${__mygsl_HMPDC_dependency_name_upper}_CMAKE_CONFIG_DIR}" )
    else ()
      if ( NOT "$ENV{${__mygsl_HMPDC_dependency_name}_DIR}" STREQUAL "" )
        message ( STATUS "${PROJECT_NAME}: Environment ${__mygsl_HMPDC_dependency_name}_DIR exists : '$ENV{${__mygsl_HMPDC_dependency_name}_DIR}' " )
        set ( ${__mygsl_HMPDC_dependency_name}_DIR "$ENV{${__mygsl_HMPDC_dependency_name}_DIR}" )
      endif ()
    endif ()
  endif () 

  set ( __mygsl_HMPDC_dependency_find_exact_option "" )
  if ( NOT "x${__mygsl_HMPDC_dependency_exact_request}" STREQUAL "x-" )
    set ( __mygsl_HMPDC_dependency_version_request "${__mygsl_HMPDC_dependency_exact_request}" )
    set ( __mygsl_HMPDC_dependency_find_exact_option EXACT )
  else ()
    if ( NOT "x${__mygsl_HMPDC_dependency_min_request}" STREQUAL "x-" )
      set ( __mygsl_HMPDC_dependency_version_request "${__mygsl_HMPDC_dependency_min_request}" )
    endif ()
  endif ( )
  if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
    message ( STATUS "${PROJECT_NAME}: DEVEL *********** Attempt to find '${__mygsl_HMPDC_dependency_name}' configuration with version request '${__mygsl_HMPDC_dependency_version_request}' " )
  endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
  find_package ( ${__mygsl_HMPDC_dependency_name} ${__mygsl_HMPDC_dependency_version_request} ${__mygsl_HMPDC_dependency_find_exact_option} REQUIRED NO_MODULE )
  if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
    message ( STATUS "${PROJECT_NAME}: DEVEL *********** After find_package invocation: __mygsl_HMPDC_dependency_name            = '${__mygsl_HMPDC_dependency_name}' " )
    message ( STATUS "${PROJECT_NAME}: DEVEL *********** After find_package invocation: __mygsl_HMPDC_dependency_version_request = '${__mygsl_HMPDC_dependency_version_request}' " )
  endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
  if ( DEFINED ${__mygsl_HMPDC_dependency_name}_CONFIG )
    if ( "x${__mygsl_HMPDC_dependency_find_exact_option}" STREQUAL "xEXACT" )
      if ( NOT ${__mygsl_HMPDC_dependency_name}_VERSION VERSION_EQUAL ${__mygsl_HMPDC_dependency_version_request} )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found version ${${__mygsl_HMPDC_dependency_name}_VERSION} != ${__mygsl_HMPDC_dependency_version_request} !" )
      endif ()
    elseif ( NOT "x${__mygsl_HMPDC_dependency_max_request}" STREQUAL "x-" )
      if ( ${__mygsl_HMPDC_dependency_name}_VERSION VERSION_GREATER __mygsl_HMPDC_dependency_max_request )
	message ( FATAL_ERROR "${PROJECT_NAME}: Found version ${${__mygsl_HMPDC_dependency_name}_VERSION} > ${__mygsl_HMPDC_dependency_max_request} !" )
      endif ()
    endif ( )
    message ( STATUS "${PROJECT_NAME}: Found ${__mygsl_HMPDC_dependency_name} >= ${__mygsl_HMPDC_dependency_version_request}" )
    message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_CONFIG              : ${${__mygsl_HMPDC_dependency_name}_CONFIG}" )
    message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_VERSION             : ${${__mygsl_HMPDC_dependency_name}_VERSION}" )
    message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_LIBRARIES           : ${${__mygsl_HMPDC_dependency_name}_LIBRARIES}" )
    message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_INCLUDE_DIRS        : ${${__mygsl_HMPDC_dependency_name}_INCLUDE_DIRS}" )
    if ( DEFINED ${__mygsl_HMPDC_dependency_name}_WITH_DEBUG )
      message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_WITH_DEBUG          : ${${__mygsl_HMPDC_dependency_name}_WITH_DEBUG}" )
    endif ()
    if ( DEFINED ${__mygsl_HMPDC_dependency_name}_DEFINITIONS )
      message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_DEFINITIONS         : ${${__mygsl_HMPDC_dependency_name}_DEFINITIONS}" )
    endif ()
    if ( DEFINED ${__mygsl_HMPDC_dependency_name}_WITH_BIO )
      message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_WITH_BIO            : ${${__mygsl_HMPDC_dependency_name}_WITH_BIO}" )
    endif ()
    if ( DEFINED ${__mygsl_HMPDC_dependency_name}_WITH_PYTHON_WRAPPER )
      message ( STATUS "${PROJECT_NAME}:  o ${__mygsl_HMPDC_dependency_name}_WITH_PYTHON_WRAPPER : ${${__mygsl_HMPDC_dependency_name}_WITH_PYTHON_WRAPPER}")
    endif ()
  else ()
    message ( FATAL_ERROR "${PROJECT_NAME}: Cannot find '${__mygsl_HMPDC_dependency_name}' with version request '${__mygsl_HMPDC_dependency_version_request}' !" )
  endif ()

  # cleaning:
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_home_made_package_dependencies_check for '${__mygsl_HMPDC_dependency_name}' -- stop  " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )

  unset ( __mygsl_HMPDC_dependency_min_request )
  unset ( __mygsl_HMPDC_dependency_max_request )
  unset ( __mygsl_HMPDC_dependency_exact_request )
  unset ( __mygsl_HMPDC_dependency_name )
  unset ( __mygsl_HMPDC_dependency_name_upper )
  unset ( __mygsl_HMPDC_dependency_version_request )
  unset ( __mygsl_HMPDC_dependency_find_exact_option )
  ###unset ( __mygsl_HMPDC_dependency_name_lower )

endmacro (mygsl_home_made_package_dependencies_check)

# -------------------------------------------------------------------------
# mygsl_home_made_package_dependencies_check_list :
#   input : list_of_dependencies_rules_
#  output : list_of_dependencies_infos_
macro ( mygsl_home_made_package_dependencies_check_list 
    list_of_dependencies_rules_
    list_of_dependencies_infos_
    )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_home_made_package_dependencies_check_list -- start     " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  foreach ( __mygsl_HMPDCL_dependency_rule ${list_of_dependencies_rules_} )
    string (REPLACE ":" ";" __mygsl_HMPDCL_dependency_tokens "${__mygsl_HMPDCL_dependency_rule}:")
    list (GET __mygsl_HMPDCL_dependency_tokens 0 __mygsl_HMPDCL_dependency_name )
    list (GET __mygsl_HMPDCL_dependency_tokens 1 __mygsl_HMPDCL_dependency_version_rules )
    string (REPLACE "," ";" __mygsl_HMPDCL_dependency_version_rules_tokens "${__mygsl_HMPDCL_dependency_version_rules}")
    list (LENGTH __mygsl_HMPDCL_dependency_version_rules_tokens __mygsl_HMPDCL_dependency_number_of_rules )
    set (__mygsl_HMPDCL_dependency_min_version   "-" )
    set (__mygsl_HMPDCL_dependency_max_version   "-" )
    set (__mygsl_HMPDCL_dependency_exact_version "-" )
    if ( __mygsl_HMPDCL_dependency_number_of_rules EQUAL 0)
      set (__mygsl_HMPDCL_dependency_min_version   "0.0.0" )
      set (__mygsl_HMPDCL_dependency_max_version   "-" )
      set (__mygsl_HMPDCL_dependency_exact_version "-" )
    else ()
      if (__mygsl_HMPDCL_dependency_number_of_rules GREATER 0)
	list (GET __mygsl_HMPDCL_dependency_version_rules_tokens 0 __mygsl_HMPDCL_dependency_first_rule )
	string(LENGTH "${__mygsl_HMPDCL_dependency_first_rule}" __length_of_dependency_first_rule)
	if (__length_of_dependency_first_rule GREATER 0 )
	  string(SUBSTRING ${__mygsl_HMPDCL_dependency_first_rule} 0 1 __first_char )
	  if ( "x${__first_char}" STREQUAL "x=" )
	    string (REGEX REPLACE "^=" "" __mygsl_HMPDCL_dependency_exact_version "${__mygsl_HMPDCL_dependency_first_rule}")
	    if (__mygsl_HMPDCL_dependency_number_of_rules GREATER 1)
	      message ( STATUS "WARNING: Ignoring second version rule in '${__mygsl_HMPDCL_dependency_version_rules_tokens}" )
	    endif()
	  else ()
	    if (__length_of_dependency_first_rule GREATER 1 )
	      string(SUBSTRING ${__mygsl_HMPDCL_dependency_first_rule} 0 2 __first_two_chars )
	      if ( "x${__first_two_chars}" STREQUAL "x>=" )
		string (REGEX REPLACE "^>=" "" __mygsl_HMPDCL_dependency_min_version "${__mygsl_HMPDCL_dependency_first_rule}")
              elseif ( "x${__first_two_chars}" STREQUAL "x<=" ) 
		set ( __mygsl_HMPDCL_dependency_min_version "0.0.0" )
		string (REGEX REPLACE "^<=" "" __mygsl_HMPDCL_dependency_max_version "${__mygsl_HMPDCL_dependency_first_rule}")
	      else ()
		set (__mygsl_HMPDCL_dependency_min_version "${__mygsl_HMPDCL_dependency_first_rule}" )
	      endif ()
	    else ()
	      set (__mygsl_HMPDCL_dependency_min_version "${__mygsl_HMPDCL_dependency_first_rule}" )
	    endif ()
	  endif ()
	  if (__mygsl_HMPDCL_dependency_number_of_rules GREATER 1)
	    list (GET __mygsl_HMPDCL_dependency_version_rules_tokens 1 __mygsl_HMPDCL_dependency_second_rule )
	    string (REGEX REPLACE "^<=" "" __mygsl_HMPDCL_dependency_max_version "${__mygsl_HMPDCL_dependency_second_rule}")
	  endif ()
	endif ()
      endif ()
    endif ()
    if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
      message ( STATUS "DEVEL: ********* __mygsl_HMPDCL_dependency_min_version   = '${__mygsl_HMPDCL_dependency_rule}' ")
      message ( STATUS "DEVEL: ********* __mygsl_HMPDCL_dependency_min_version   = '${__mygsl_HMPDCL_dependency_min_version}' ")
      message ( STATUS "DEVEL: ********* __mygsl_HMPDCL_dependency_max_version   = '${__mygsl_HMPDCL_dependency_max_version}' ")
      message ( STATUS "DEVEL: ********* __mygsl_HMPDCL_dependency_exact_version = '${__mygsl_HMPDCL_dependency_exact_version}' ")
    endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
    mygsl_home_made_package_dependencies_check ( ${__mygsl_HMPDCL_dependency_name} 
      ${__mygsl_HMPDCL_dependency_min_version} 
      ${__mygsl_HMPDCL_dependency_max_version} 
      ${__mygsl_HMPDCL_dependency_exact_version} 
      )
    list ( APPEND ${list_of_dependencies_infos_} "${__mygsl_HMPDCL_dependency_name}:${${__mygsl_HMPDCL_dependency_name}_VERSION}" )

    if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
      message ( STATUS "DEVEL: ********* ${list_of_dependencies_infos_}: Appending: '${__mygsl_HMPDCL_dependency_name}:${${__mygsl_HMPDCL_dependency_name}_VERSION}'" )
    endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)

    # cleaning :
    unset ( __mygsl_HMPDCL_dependency_number_of_rules )
    unset ( __mygsl_HMPDCL_dependency_name )
    unset ( __mygsl_HMPDCL_dependency_min_version )
    unset ( __mygsl_HMPDCL_dependency_max_version )
    unset ( __mygsl_HMPDCL_dependency_exact_version )
    unset ( __mygsl_HMPDCL_dependency_tokens )
  endforeach ()
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_home_made_package_dependencies_check_list -- stop      " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
endmacro (mygsl_home_made_package_dependencies_check_list)

# -----------------------------------------------------------------------
# - This macro populates some lists with dependency build directives.
#
#  input : list_of_dependencies_infos_ : a list of info records about checked dependencies
#     a record contains :
#      - the name of the library package
#      -  the actual version 
#     separated by the column character ':' 
#     Example: "foo:1.3.0; bar:6.2" 
#  output : list_of_include_dirs_ :
#    a variable string that addresses the name of a list
#    of include directories to be updated (-I<INCPATH>...)
#  output : list_of_libraries_ : 
#   a variable string that addresses the name of a list
#   of library targets to be updated (-L<LIBPATH> -l<libname>...)
#  output : list_of_definitions_ : 
#   a variable string that addresses the name of a list
#   of compilation definition to be updated (-D<BLAH>... )
#
macro ( mygsl_home_made_package_dependencies_populate_build_lists 
    list_of_dependencies_infos_
    list_of_definitions_ 
    list_of_include_dirs_
    list_of_libraries_ )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_home_made_package_dependencies_populate_build_lists -- start " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  
  foreach ( __mygsl_HMPDPBL_dependency_entry ${list_of_dependencies_infos_} )
    
    string (REPLACE ":" ";" __mygsl_HMPDPBL_dependency_tokens "${__mygsl_HMPDPBL_dependency_entry}:")
    list (GET __mygsl_HMPDPBL_dependency_tokens 0 __mygsl_HMPDPBL_dependency_name )
    list (GET __mygsl_HMPDPBL_dependency_tokens 1 __mygsl_HMPDPBL_dependency_version)

    message ( STATUS "${PROJECT_NAME}: NOTICE: Attempt to automate the '${__mygsl_HMPDPBL_dependency_name} (${__mygsl_HMPDPBL_dependency_version})' dependency" )
    if (DEFINED ${__mygsl_HMPDPBL_dependency_name}_DEFINITIONS 
	AND ${__mygsl_HMPDPBL_dependency_name}_DEFINITIONS
	AND NOT "x${__mygsl_HMPDPBL_dependency_name}_DEFINITIONS" STREQUAL "x" )
      message ( STATUS "${PROJECT_NAME}: NOTICE: Adding definitions from the '${__mygsl_HMPDPBL_dependency_name}_DEFINITIONS' variable..."  )
      list (APPEND ${list_of_definitions_} ${${__mygsl_HMPDPBL_dependency_name}_DEFINITIONS} ) 
    else ()
      ### message ( STATUS "${PROJECT_NAME}: WARNING: Cannot find any '${__mygsl_HMPDPBL_dependency_name}_DEFINITIONS' variable !"  )
    endif ()

    if (DEFINED ${__mygsl_HMPDPBL_dependency_name}_INCLUDE_DIRS 
	AND ${__mygsl_HMPDPBL_dependency_name}_INCLUDE_DIRS
	AND NOT "x${__mygsl_HMPDPBL_dependency_name}_INCLUDE_DIRS" STREQUAL "x"  )
      message ( STATUS "${PROJECT_NAME}: NOTICE: Adding include path from the '${__mygsl_HMPDPBL_dependency_name}_INCLUDE_DIRS' variable..."  )
      list (APPEND ${list_of_include_dirs_} ${${__mygsl_HMPDPBL_dependency_name}_INCLUDE_DIRS} ) 
    else ()
      message ( STATUS "${PROJECT_NAME}: WARNING: Cannot find any '${__mygsl_HMPDPBL_dependency_name}_INCLUDE_DIRS' variable !"  )
    endif ()
    
    if (DEFINED ${__mygsl_HMPDPBL_dependency_name}_LIBRARIES
	AND ${__mygsl_HMPDPBL_dependency_name}_LIBRARIES
	AND NOT "x${__mygsl_HMPDPBL_dependency_name}_LIBRARIES" STREQUAL "x"   )
      message ( STATUS "${PROJECT_NAME}: NOTICE: Adding libraries from the '${__mygsl_HMPDPBL_dependency_name}_LIBRARIES' variable..."  )
      list ( APPEND ${list_of_libraries_} ${${__mygsl_HMPDPBL_dependency_name}_LIBRARIES} ) 
    else ()
      message ( STATUS "${PROJECT_NAME}: WARNING: Cannot find any '${__mygsl_HMPDPBL_dependency_name}_LIBRARIES' variable !"  )
    endif ()
    
    # clear pseudo local variables:
    unset (__mygsl_HMPDPBL_dependency_name)
    unset (__mygsl_HMPDPBL_dependency_version)
    unset (__mygsl_HMPDPBL_dependency_tokens)

  endforeach ()
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  message ( STATUS "${PROJECT_NAME}: * mygsl_home_made_package_dependencies_populate_build_lists -- stop  " )
  message ( STATUS "${PROJECT_NAME}: ******************************************************** " )
  
endmacro (mygsl_home_made_package_dependencies_populate_build_lists )

# end of mygsl_HomeMadePackageDependencies.cmake 

