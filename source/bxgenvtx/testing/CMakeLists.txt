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
      test_box_vg.cxx
      test_cylinder_vg.cxx
      test_placement_vg.cxx
      test_spot_vertex_generator.cxx
      test_from_file_vg.cxx
      test_factory.cxx
      test_manager.cxx
    )

# include files search paths
include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                      ${GEOMTOOLS_INCLUDE_DIRS_DEPEND}
                      ${PROJECT_SOURCE_DIR}/testing
                    )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( GENVTX_ALL_TESTS "Perform all possible tests" ON )
message ( STATUS "testing: Performing all available tests : ${GENVTX_ALL_TESTS}" )

if ( GENVTX_ALL_TESTS )
  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  #message ( STATUS "testing:  _test_tmp_dir : ${_test_tmp_dir}" )
  foreach ( test_program ${genvtx_tests_programs_list} )
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
    list ( APPEND test_libraries "genvtx" 
           ${GEOMTOOLS_LIBRARIES_DEPEND} 
         )
	 
    target_link_libraries ( ${test_executable} ${test_libraries} )

    add_test ( NAME ${test_executable}.run
               COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                       --tmp-dir ${_test_tmp_dir}
                       --prefix ${PROJECT_SOURCE_DIR} 
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

# end of CMakeLists.txt
