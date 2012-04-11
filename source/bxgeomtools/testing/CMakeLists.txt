###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
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
  test_geomtools.cxx 
  test_blur_spot.cxx 
  test_address_set.cxx
  test_any_shape_3d.cxx
  test_box.cxx
  test_circle.cxx
  test_color.cxx
  test_cylinder.cxx
  test_disk.cxx
  test_gdml_writer.cxx
  test_geom_id.cxx
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
  test_serializable_3.cxx
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

include_directories ( ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include     
  ${mygsl_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( GEOMTOOLS_ALL_TESTS "Perform all possible tests" 1 )
message ( STATUS "testing: Performing all available tests : ${GEOMTOOLS_ALL_TESTS}" )

if ( GEOMTOOLS_ALL_TESTS )
  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  execute_process (COMMAND cmake -E remove_directory ${_test_tmp_dir} )

  foreach ( test_program ${geomtools_tests_programs_list} )
    get_filename_component ( test_executable ${test_program} NAME_WE )
    message ( STATUS "testing:  Building test target '${test_executable}'" )
    add_executable ( ${test_executable} ${test_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,
    set ( _test_libraries geomtools )
    # list ( APPEND _test_libraries ${mygsl_LIBRARIES} )
    # #message ( STATUS "testing:  GEOMTOOLS_WITH_BIO=${GEOMTOOLS_WITH_BIO}" )
    # if ( GEOMTOOLS_WITH_BIO )
    #   #message ( STATUS "testing:  INSERT geomtools_bio" )
    #   list (INSERT _test_libraries 0 geomtools_bio )
    # endif ()
    if ( GEOMTOOLS_WITH_BIO )
      set ( _test_libraries geomtools_bio )
    endif ()
    
    target_link_libraries ( ${test_executable} ${_test_libraries} ${_test_libraries} )

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

option ( GEOMTOOLS_INSTALL_TEST_PROGRAMS "Install test programs" 1 )
message ( STATUS "testing: Performing all available tests : ${GEOMTOOLS_INSTALL_TEST_PROGRAMS}" )

# - Installation :
if (GEOMTOOLS_INSTALL_TEST_PROGRAMS) 
  set ( _test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )

  file ( GLOB geomtools_tests_config config/test_* )
  install ( FILES ${geomtools_tests_config} 
    DESTINATION ${_test_install_dir}/config  
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
    )

  file ( GLOB geomtools_tests_data data/test_*.data )
  install ( FILES ${geomtools_tests_data} 
    DESTINATION ${_test_install_dir}/data  
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
    )

  file ( GLOB geomtools_test_programs test_*.cxx test_*.gpl)
  install ( FILES ${geomtools_test_programs} 
    geomtools_test_model_1.cc
    geomtools_test_model_1.h
    geomtools_test_model_2.cc
    geomtools_test_model_2.h
    geomtools_test_world_model.cc
    geomtools_test_world_model.h
    DESTINATION ${_test_install_dir}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
    )

endif ()

# end of CMakeLists.txt
