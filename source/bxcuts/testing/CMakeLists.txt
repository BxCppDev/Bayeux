###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the cuts library.
##
## cuts is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## cuts is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with cuts.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set ( cuts_tests_programs_list
      test_cuts.cxx 
      test_accept_cut.cxx 
      test_and_cut.cxx
      test_manager.cxx
    )

# include files search paths
include_directories ( ${PROJECT_BINARY_DIR}/include
                      ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_SOURCE_DIR}/testing
                    )

include_directories ( ${datatools_INCLUDE_DIRS} )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( CUTS_ALL_TESTS "Perform all possible tests" 1 )
message ( STATUS "testing: Performing all available tests : ${CUTS_ALL_TESTS}" )

if ( CUTS_ALL_TESTS )
  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  foreach ( test_program ${cuts_tests_programs_list} )
    unset ( test_libraries )
    get_filename_component ( test_executable ${test_program} NAME_WE )
    message ( STATUS "testing:  Building test target '${test_executable}'" )
    add_executable ( ${test_executable} ${test_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,
    list ( APPEND test_libraries "cuts" )

    list ( APPEND test_libraries "${datatools_LIBRARIES}" )
 	 
    target_link_libraries ( ${test_executable} ${test_libraries} )

    add_test ( NAME ${test_executable}.run
               COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                       --tmp-dir ${_test_tmp_dir}
                       --data-dir ${PROJECT_SOURCE_DIR} 
                       --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
	               run
             )	 	

  endforeach ()
  
  add_custom_target ( clean_test
                      COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                      --prefix ${PROJECT_SOURCE_DIR} 
                      --tmp-dir ${_test_tmp_dir}
                      clean
                    )

endif ()

option ( CUTS_INSTALL_TEST_PROGRAMS "Install test programs" 1 )
message ( STATUS "testing: Performing all available tests : ${CUTS_INSTALL_TEST_PROGRAMS}" )

# - Installation :
if (CUTS_INSTALL_TEST_PROGRAMS) 
  set ( _test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )

  file ( GLOB cuts_tests_config config/test_*.conf )
  install ( FILES ${cuts_tests_config} 
            DESTINATION ${_test_install_dir}/config  
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
          )

  file ( GLOB cuts_tests_config test_*.cxx )
  install ( FILES ${cuts_tests_programs} 
	    cuts_test_color_cut.cc
	    cuts_test_color_cut.h
	    cuts_test_data.h
	    cuts_test_range_cut.cc
	    cuts_test_range_cut.h
	    cuts_test_sphere_cut.cc
	    cuts_test_sphere_cut.h
	    DESTINATION ${_test_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
          )

endif ()

# end of CMakeLists.txt
