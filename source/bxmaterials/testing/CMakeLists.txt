###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the materials library.
##
## materials is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## materials is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with materials.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set ( materials_tests_programs_list
      test_materials.cxx
      test_ch_symbol.cxx
      test_element.cxx
      test_factory.cxx
      test_isotope.cxx
      test_manager.cxx
      test_material.cxx
    )

# include files search paths
include_directories ( ${PROJECT_BINARY_DIR}/include
                      ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_SOURCE_DIR}/testing
                    )

include_directories ( ${geomtools_INCLUDE_DIRS} )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( MATERIALS_ALL_TESTS "Perform all possible tests" 1 )
message ( STATUS "testing: Performing all available tests : ${MATERIALS_ALL_TESTS}" )

if ( MATERIALS_ALL_TESTS )
  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  #message ( STATUS "testing:  _test_tmp_dir : ${_test_tmp_dir}" )
  foreach ( test_program ${materials_tests_programs_list} )
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
    list ( APPEND test_libraries "materials" )
    list ( APPEND test_libraries "${geomtools_LIBRARIES}" )
	 
    target_link_libraries ( ${test_executable} ${test_libraries} )

    add_test ( NAME ${test_executable}.run
               COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                       --tmp-dir ${_test_tmp_dir}
                       --data-dir ${PROJECT_SOURCE_DIR}/resources 
                       --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
	               run
             )	 	

  endforeach ()
  
  add_custom_target ( clean_test
                      COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                      --tmp-dir ${_test_tmp_dir}
                      clean
                    )

endif ()

option ( MATERIALS_INSTALL_TEST_PROGRAMS "Install test programs" 1 )
message ( STATUS "testing: Installing all test programs' source files : ${MATERIALS_INSTALL_TEST_PROGRAMS}" )

# - Installation :
if (MATERIALS_INSTALL_TEST_PROGRAMS) 
  set ( _test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )

  file ( GLOB materials_tests_config config/*.def )
  install ( FILES ${materials_tests_config} 
     	    DESTINATION ${_test_install_dir}/config  
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
          )

  file ( GLOB materials_tests_programs test_*.cxx )
  install ( FILES ${materials_tests_programs} testDriver.bash
	    DESTINATION ${_test_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
          )

endif ()

# end of CMakeLists.txt
