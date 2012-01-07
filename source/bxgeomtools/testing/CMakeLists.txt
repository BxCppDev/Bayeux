###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the geomtools library.
##
## geomtools is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## geomtools is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with geomtools.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################


set ( geomtools_tests_programs_list
test_address_set.cxx
test_any_shape_3d.cxx
test_box.cxx
test_circle.cxx
test_color.cxx
test_cylinder.cxx
test_disk.cxx
test_dummy_geomtools.cxx
test_gdml_writer.cxx
test_geom_id.cxx
test_geomtools.cxx
test_gnuplot_draw.cxx
#test_gnuplot_i.cxx
test_helix.cxx
test_hexagon_box.cxx
test_id_mgr.cxx
test_id_selector.cxx
test_i_model.cxx
test_intersection_3d.cxx
#test_intersection.cxx
test_logical_volume.cxx
test_model_factory.cxx
test_multiple_placement.cxx
test_physical_volume.cxx
test_placement_2.cxx
test_placement_3.cxx
test_placement.cxx
test_polycone_2.cxx
test_polycone.cxx
test_polyhedra.cxx
test_polyline_3d.cxx
test_rectangle.cxx
test_regular_grid_placement.cxx
test_regular_linear_placement.cxx
test_regular_polygon.cxx
test_rotation_3d.cxx
test_s0.cxx
test_serializable_2.cxx
test_serializable.cxx
test_sphere.cxx
test_subtraction_3d.cxx
test_tessellated_solid.cxx
test_tube.cxx
test_union_3d.cxx
test_utils_2.cxx
test_utils.cxx
    )

# include files search paths
include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                      ${DATATOOLS_INCLUDE_DIRS_DEPEND}
                      ${MYGSL_INCLUDE_DIRS_DEPEND}
                      ${PROJECT_SOURCE_DIR}/testing
                 )

# linker search paths
link_directories( ${PROJECT_BINARY_DIR}/src )

option (GEOMTOOLS_ALL_TESTS "Perform all possible tests" ON)
message ( STATUS "Performing all available tests : ${GEOMTOOLS_ALL_TESTS}" )

if (GEOMTOOLS_ALL_TESTS)
set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
#message ( STATUS "_test_tmp_dir : ${_test_tmp_dir}" )
foreach ( test_program ${geomtools_tests_programs_list} )
  unset ( test_libraries )
  get_filename_component ( test_executable ${test_program} NAME_WE )
  message ( STATUS "Building test target '${test_executable}'" )
  add_executable ( ${test_executable} ${test_program} )
  # define d suffix on windows
  if(WIN32)
    set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
  endif()

  #message ( STATUS "DATATOOLS_LIBRARIES_DEPEND='${DATATOOLS_LIBRARIES_DEPEND}'" )
  #message ( STATUS "MYGSL_LIBRARIES_DEPEND='${MYGSL_LIBRARIES_DEPEND}'" )

  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  list (APPEND test_libraries "geomtools" ${DATATOOLS_LIBRARIES_DEPEND} ${MYGSL_LIBRARIES_DEPEND} )
	 
  if ( GEOMTOOLS_WITH_BIO )
    list (INSERT test_libraries 0 "geomtools_bio" )
  endif ()
	 
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
