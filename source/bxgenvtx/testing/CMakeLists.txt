###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the genvtx library.
##
## genvtx is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## genvtx is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with genvtx.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set ( genvtx_tests_programs_list
      test_genvtx.cxx 
      test_spot_vertex_generator.cxx
      test_box_vg.cxx
      test_cylinder_vg.cxx
      test_tube_vg.cxx
      test_placement_vg.cxx
      test_from_file_vg.cxx
      test_triangle_random_tools.cxx
      test_quadrangle_random_tools.cxx
    )

# include files search paths
include_directories ( ${PROJECT_BINARY_DIR}/include
                      ${PROJECT_SOURCE_DIR}/include	
                      ${geomtools_INCLUDE_DIRS}
                      ${PROJECT_SOURCE_DIR}/testing
                    )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( GENVTX_ALL_TESTS "Perform all possible tests" 1 )
message ( STATUS "testing: Performing all available tests : ${GENVTX_ALL_TESTS}" )

if ( GENVTX_ALL_TESTS )
  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  execute_process (COMMAND cmake -E remove_directory ${_test_tmp_dir} )
  foreach ( test_program ${genvtx_tests_programs_list} )
    get_filename_component ( test_executable ${test_program} NAME_WE )
    message ( STATUS "testing:  Building test target '${test_executable}'" )
    add_executable ( ${test_executable} ${test_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,
    set ( _test_libraries genvtx )
	 
    target_link_libraries ( ${test_executable} ${_test_libraries} )

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
                      --data-dir ${PROJECT_SOURCE_DIR} 
                      --tmp-dir ${_test_tmp_dir}
                      clean
                    )

endif ()

option ( GENVTX_INSTALL_TEST_PROGRAMS "Install test programs" 1 )
message ( STATUS "testing: Performing all available tests : ${GENVTX_INSTALL_TEST_PROGRAMS}" )

# - Installation :
if (GENVTX_INSTALL_TEST_PROGRAMS) 
  set ( _test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )

  file ( GLOB genvtx_tests_config config/test_*.data )
  install ( FILES ${genvtx_tests_config} 
     	      DESTINATION ${_test_install_dir}/config  
             PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
           )

  file ( GLOB genvtx_tests_programs test_*.cxx test_*.gp testDriver.bash)
  install ( FILES ${genvtx_tests_programs} 
	    DESTINATION ${_test_install_dir}
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
          )

endif ()

# end of CMakeLists.txt
