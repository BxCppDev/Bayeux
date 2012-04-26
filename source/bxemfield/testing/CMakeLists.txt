# -*- mode: cmake; -*-
###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the emfield library.
##
## emfield is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## emfield is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with emfield.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################


file ( GLOB _emfield_testing_active_programs 
  test_*.cxx )

set ( emfield_tests_programs_list
  ###test_emfield.cxx
  ${_emfield_testing_active_programs}
  )
unset ( _emfield_testing_active_programs )

# include files search paths
include_directories ( ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  ${PROJECT_SOURCE_DIR}/testing
  )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( EMFIELD_ALL_TESTS "Perform all possible tests" 1 )
message ( STATUS "emfield: testing: Performing all available tests : ${EMFIELD_ALL_TESTS}" )

if (AUTOMATIC_CMAKE_DEPENDENCIES)

  # List of definitions flags to be passed to the compiler :
  set (_emfield_list_of_dependencies_definitions  )
  # List of include directories to be used at build time :
  set (_emfield_list_of_dependencies_include_dirs )
  # List of library targets to be linked against :
  set (_emfield_list_of_dependencies_libraries    )

  # Compute all dependencies directives in one shot :
  if ( _emfield_number_of_dependencies GREATER 0 )   
    emfield_home_made_package_dependencies_populate_build_lists (
      "${_emfield_list_of_dependencies_infos}"
      _emfield_list_of_dependencies_definitions
      _emfield_list_of_dependencies_include_dirs
      _emfield_list_of_dependencies_libraries
      )
  endif ()

  if ( _emfield_number_of_third_party_dependencies GREATER 0 )   
    emfield_third_party_dependencies_populate_build_lists (
      "${_emfield_list_of_third_party_dependencies_infos}"
      _emfield_list_of_dependencies_definitions
      _emfield_list_of_dependencies_include_dirs
      _emfield_list_of_dependencies_libraries
      )
  endif ()

  # Apply automatic dependencies build directives to the compiler :
  add_definitions ( ${_emfield_list_of_dependencies_definitions} )
  include_directories ( ${_emfield_list_of_dependencies_include_dirs} )

endif (AUTOMATIC_CMAKE_DEPENDENCIES)

if ( EMFIELD_ALL_TESTS )
  set ( _emfield_test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  execute_process (COMMAND cmake -E remove_directory ${_emfield_test_tmp_dir} )

  foreach ( _emfield_test_program ${emfield_tests_programs_list} )
    get_filename_component ( _emfield_test_executable ${_emfield_test_program} NAME_WE )
    message ( STATUS "emfield: testing: Building test target '${_emfield_test_executable}'" )
    add_executable ( ${_emfield_test_executable} ${_emfield_test_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${_emfield_test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,
    set ( _emfield_test_libraries emfield )
    if ( EMFIELD_WITH_BIO_SUPPORT AND EMFIELD_WITH_BIO )
      set ( _emfield_test_libraries emfield_bio )
    endif ()

    # Not needed :
    # if (AUTOMATIC_CMAKE_DEPENDENCIES)
    #   # Apply automatic dependencies build directives to the compiler :
    #   list ( APPEND _emfield_test_libraries ${_emfield_list_of_dependencies_libraries} )
    # endif (AUTOMATIC_CMAKE_DEPENDENCIES)

    target_link_libraries ( ${_emfield_test_executable} ${_emfield_test_libraries} )

    set ( _emfield_test_additional_options )

    ### This is a code snipet that allows to add a special switch to the testDriver.bash script
    ### in order to specify the path to data directory (<Yyy-install-path>/share/Yyy) of another Yyy package :
    # if (DEFINED EMFIELD_TEST_YYY_DATA_DIR 
    # 	AND NOT "x${EMFIELD_TEST_YYY_DATA_DIR}" STREQUAL "x" )
    #   list (APPEND _emfield_test_additional_options --yyy-data-dir ${EMFIELD_TEST_YYY_DATA_DIR} )
    #   message ( STATUS "${PROJECT_NAME}: testing: Using option '${_emfield_test_additional_options}'" )
    # endif ()

    ### This is a code snipet that allows to add a special switch to the testDriver.bash script
    ### in order to specify the path to data directory (<Yyy-install-path>/bin) of another Yyy package :
    # if (DEFINED EMFIELD_TEST_YYY_BIN_DIR 
    # 	AND NOT "x${EMFIELD_TEST_YYY_BIN_DIR}" STREQUAL "x" )
    #   list (APPEND _emfield_test_additional_options --yyy-bin-dir ${EMFIELD_TEST_YYY_BIN_DIR} )
    #   message ( STATUS "${PROJECT_NAME}: testing: Using option '${_emfield_test_additional_options}'" )
    # endif ()

    add_test ( NAME ${_emfield_test_executable}.run
      COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
      --tmp-dir ${_emfield_test_tmp_dir}
      --data-dir ${PROJECT_SOURCE_DIR} 
      --prefix ${PROJECT_SOURCE_DIR} 
      --exe $<TARGET_FILE_DIR:${_emfield_test_executable}>/$<TARGET_FILE_NAME:${_emfield_test_executable}>
      ${_emfield_test_additional_options}
      run
      )	 
    
    unset ( _emfield_test_additional_options )
    unset ( _emfield_test_libraries )

  endforeach ()
  
  add_custom_target ( clean_test
    COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
    --prefix ${PROJECT_SOURCE_DIR} 
    --data-dir ${PROJECT_SOURCE_DIR} 
    --tmp-dir ${_emfield_test_tmp_dir}
    clean
    )

endif ()

option ( EMFIELD_INSTALL_TEST_PROGRAMS "Install test programs" 1 )
message ( STATUS "emfield: testing: Installing all available test programs : ${EMFIELD_INSTALL_TEST_PROGRAMS}" )

# - Installation :
set ( _emfield_test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )
if (EMFIELD_INSTALL_TEST_PROGRAMS) 

  if (EXISTS ${PROJECT_SOURCE_DIR}/testing/config )
    file ( GLOB _emfield_testing_config config/*.conf )
    install ( FILES ${_emfield_testing_config} 
      DESTINATION ${_emfield_test_install_dir}/config  
      PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
     )
    unset ( _emfield_testing_config )
  endif ()

  if (EXISTS ${PROJECT_SOURCE_DIR}/testing/data )
    file ( GLOB _emfield_testing_data data/*.data )
    install ( FILES ${_emfield_testing_data} 
      DESTINATION ${_emfield_test_install_dir}/data  
      PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
      )
    unset ( _emfield_testing_data )
  endif ()

  file ( GLOB _emfield_testing_programs test_*.cxx )
  install ( FILES ${_emfield_testing_programs} testDriver.bash
    DESTINATION ${_emfield_test_install_dir}
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
    )
  unset ( emfield_testing_programs )

  if (EXISTS ${PROJECT_SOURCE_DIR}/testing/post_install )
    
    install ( DIRECTORY  ${PROJECT_SOURCE_DIR}/testing/post_install
      DESTINATION ${_emfield_test_install_dir}
      DIRECTORY_PERMISSIONS 
      OWNER_READ OWNER_WRITE OWNER_EXECUTE 
      GROUP_READ GROUP_WRITE GROUP_EXECUTE  
      WORLD_READ WORLD_WRITE WORLD_EXECUTE 
      PATTERN "*.svn" EXCLUDE 
      PATTERN "__build" EXCLUDE 
      PATTERN "__install" EXCLUDE 
      )
  endif ()

endif ()

# end of CMakeLists.txt
