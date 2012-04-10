###############################################################################
##
## Copyright (C) 2012 François Mauger 
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the genbb_help library.
##
## genbb_help is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## genbb_help is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with genbb_help.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# set ( CMAKE_Fortran_FLAGS 
#   "${_genbb_help_fortran_definitions} "
#   )

# set ( CMAKE_CXX_FLAGS  
#   "${_genbb_help_fortran_definitions} ${_genbb_help_definitions} "
#   )

set ( genbb_help_tests_programs_list_CXX
      test_genbb_help.cxx 
      test_bb0nu_channel_2.cxx
      test_bb0nu_channel.cxx
      test_bb0nu_cos_theta.cxx
      test_fermi.cxx
      test_genbb.cxx
      test_genbb_help_serialization.cxx
      test_genbb_mgr_2.cxx
      test_genbb_mgr_3.cxx
      test_genbb_mgr_4.cxx
      test_genbb_mgr_5.cxx
      test_genbb_mgr.cxx
      test_genbb_writer.cxx
      test_primary_event.cxx
      test_single_particle_generator_2.cxx
      test_single_particle_generator_3.cxx
      test_single_particle_generator.cxx
      test_wgenbb.cxx
    )

set ( genbb_help_tests_programs_list_Fortran
    test_beta.F
    test_fermi_f.F
    test_rnd2.F
    test_rnd.F
    )

# include files search paths
include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                      ${geomtools_INCLUDE_DIRS}
                      ${PROJECT_SOURCE_DIR}/testing
                    )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( GENBB_HELP_ALL_TESTS "Perform all possible tests" ON )
message ( STATUS "testing: Performing all available tests : ${GENBB_HELP_ALL_TESTS}" )

if ( GENBB_HELP_ALL_TESTS )

  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  execute_process (COMMAND cmake -E remove_directory -f ${_test_tmp_dir} )

  foreach ( test_program ${genbb_help_tests_programs_list_CXX} )
    get_filename_component ( test_executable ${test_program} NAME_WE )
    message ( STATUS "testing: Building C++ test target '${test_executable}'" )
    add_executable ( ${test_executable} ${test_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

 
    if ( GENBB_HELP_WITH_DEBUG )
      add_definitions ( "-g" )
    endif ()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,
    set ( _test_libraries genbb_help genbb_help_f77 )
    list ( APPEND _test_libraries ${geomtools_LIBRARIES} )
    if ( GENBB_HELP_WITH_BIO )
      list ( INSERT _test_libraries 0 genbb_help_bio ) 
    endif ()
	 
    target_link_libraries ( ${test_executable} ${_test_libraries} )

    add_test ( NAME ${test_executable}.run
               COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                       --tmp-dir ${_test_tmp_dir}
                       --source-dir ${PROJECT_SOURCE_DIR} 
                       --binary-dir ${PROJECT_BINARY_DIR} 
                       --data-dir ${PROJECT_SOURCE_DIR} 
                       --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
	               run
             )	 	

  endforeach ()

  foreach ( test_program ${genbb_help_tests_programs_list_Fortran} )
    get_filename_component ( test_executable ${test_program} NAME_WE )
    message ( STATUS "testing: Building Fortran test target '${test_executable}'" )
    add_executable ( ${test_executable} ${test_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,
    target_link_libraries ( ${test_executable} 
      genbb_help_f77
      ${geomtools_LIBRARIES} 
      )

    add_test ( NAME ${test_executable}.run
               COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                       --tmp-dir ${_test_tmp_dir}
                       --source-dir ${PROJECT_SOURCE_DIR} 
                       --binary-dir ${PROJECT_BINARY_DIR} 
                       --data-dir ${PROJECT_SOURCE_DIR} 
                       --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
	               run
             )	 	

  endforeach ()
  
  add_custom_target ( clean_test
                      COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                      --tmp-dir ${_test_tmp_dir}
                      --source-dir ${PROJECT_SOURCE_DIR} 
                      --binary-dir ${PROJECT_BINARY_DIR} 
                      --data-dir ${PROJECT_SOURCE_DIR} 
                      clean
                    )

endif ()

##set_target_properties(foo PROPERTIES LINKER_LANGUAGE CXX)

option ( GENBB_HELP_INSTALL_TEST_PROGRAMS "Install test programs" 1 )
message ( STATUS "testing: Installing all test programs' source files : ${GENBB_HELP_INSTALL_TEST_PROGRAMS}" )

# - Installation :
if (GENBB_HELP_INSTALL_TEST_PROGRAMS) 
  set ( _test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )

  file ( GLOB genbb_help_tests_config config/*.conf )
  install ( FILES ${genbb_help_tests_config} 
     	    DESTINATION ${_test_install_dir}/config  
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
          )

  file ( GLOB genbb_help_tests_data data/*.genbb data/*.txt.gz data/*.data  )
  install ( FILES ${genbb_help_tests_data} 
     	    DESTINATION ${_test_install_dir}/data  
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
          )

  file ( GLOB genbb_help_tests_programs test_*.cxx )
  install ( FILES ${genbb_help_tests_programs} testDriver.bash
	    DESTINATION ${_test_install_dir}
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
          )

endif ()

# end of CMakeLists.txt
