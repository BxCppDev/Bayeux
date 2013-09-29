###############################################################################
##
## Copyright (C) 2011-2013 François Mauger, Université de Caen Basse-Normandie
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

set ( geomtools_tests_programs
  test_geomtools.cxx
  test_base_hit.cxx
  test_blur_spot.cxx
  test_address_set.cxx
  test_box.cxx
  test_circle.cxx
  test_color.cxx
  test_cylinder.cxx
  test_disk.cxx
  test_gdml_writer.cxx
  test_geom_id.cxx
  test_gnuplot_i.cxx
  test_helix.cxx
  test_hexagon_box.cxx
  test_id_mgr.cxx
  test_id_selector.cxx
  test_i_model.cxx
  test_intersection_3d.cxx
  test_intersection.cxx
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
  test_plane.cxx
  test_regular_grid_placement.cxx
  test_regular_linear_placement.cxx
  test_regular_polygon.cxx
  test_rotation_3d.cxx
  test_s0.cxx
  test_serializable_3.cxx
  test_serializable_2.cxx
  test_serializable.cxx
  test_sphere.cxx
  test_subtraction_3d.cxx
  test_tessellated_solid.cxx
  test_stl_tools.cxx
  test_tube.cxx
  test_union_3d.cxx
  test_utils_2.cxx
  test_utils.cxx
  test_gnuplot_draw.cxx
  test_display_data.cxx
  test_manager.cxx
  test_service_manager.cxx
  test_reflection_0.cxx
)
#set ( geomtools_tests_programs)
# if(GEOMTOOLS_WITH_REFLECTION)
#   list(APPEND geomtools_tests_programs ${CMAKE_CURRENT_SOURCE_DIR}/test_reflection_0.cxx)
# endif()

set(geomtools_tests_programs_list "${geomtools_tests_programs}")
list(SORT geomtools_tests_programs_list)

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${mygsl_INCLUDE_DIRS}
  ${materials_INCLUDE_DIRS}
  ${CLHEP_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/testing
  )

set(_test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/testing_output")

set(_geomtools_test_additional_options )

# depends on materials data dir:
if (DEFINED materials_DATA_DIR
    AND NOT "x${materials_DATA_DIR}" STREQUAL "x" )
  list (APPEND _geomtools_test_additional_options --materials-data-dir ${materials_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_geomtools_test_additional_options}'" )
endif ()

execute_process(COMMAND cmake -E remove_directory ${_test_tmp_dir} )

foreach(test_program ${geomtools_tests_programs_list})
  get_filename_component(test_executable ${test_program} NAME_WE)
  add_executable(${test_executable} ${test_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${test_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_test_libraries geomtools)

  # Special bio DLL plugins :
  if(mygsl_WITH_BIO)
    list(INSERT _test_libraries 0 mygsl_bio )
  endif()
  if(GEOMTOOLS_WITH_BIO)
    list(INSERT _test_libraries 0 geomtools_bio )
  endif()

  # Special reflection DLL plugins :
  if(datatools_WITH_REFLECTION)
    list(INSERT _test_libraries 0 datatools_reflection)
  endif()
  if(GEOMTOOLS_WITH_REFLECTION)
    list(INSERT _test_libraries 0 geomtools_reflection )
  endif()

  target_link_libraries(${test_executable} ${_test_libraries})

  add_test(NAME ${test_executable}.run
    COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash
    --tmp-dir ${_test_tmp_dir}
    --data-dir ${PROJECT_SOURCE_DIR}
    --prefix ${PROJECT_SOURCE_DIR}
    --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
     ${_geomtools_test_additional_options}
    run
    )

  unset(_test_libraries)
endforeach ()

add_custom_target(clean_test
  COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash
  --prefix ${PROJECT_SOURCE_DIR}
  --tmp-dir ${_test_tmp_dir}
  clean
  )

# end of CMakeLists.txt
