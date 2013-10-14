###############################################################################
##
## Copyright (C) 2011-2013 François Mauger, Université de Caen Basse-Normandie
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


###file(GLOB genvtx_tests_programs test_*.cxx)
set ( genvtx_tests_programs
      test_genvtx.cxx
      test_spot_vertex_generator.cxx
      test_box_vg.cxx
      test_cylinder_vg.cxx
      test_tube_vg.cxx
      test_sphere_vg.cxx
      test_placement_vg.cxx
      test_from_file_vg.cxx
      test_polycone_vg.cxx
      test_polycone_vg_2.cxx
      test_triangle_random_tools.cxx
      test_quadrangle_random_tools.cxx
      test_ring_random_tools.cxx
      test_conical_frustrum_random_tools.cxx
      test_manager.cxx
)

###message (STATUS "genvtx_tests_programs=${genvtx_tests_programs}" )
list(REMOVE_ITEM genvtx_tests_programs ${CMAKE_CURRENT_SOURCE_DIR}/test_reflection_0.cxx)
if(GENVTX_WITH_REFLECTION)
  list(APPEND genvtx_tests_programs ${CMAKE_CURRENT_SOURCE_DIR}/test_reflection_0.cxx)
endif()

set(genvtx_tests_programs_list "${genvtx_tests_programs}")
list(SORT genvtx_tests_programs_list)

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${geomtools_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

set(_test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/testing_output")

set(_genvtx_test_additional_options )

# depends on materials data dir:
if (DEFINED materials_DATA_DIR
    AND NOT "x${materials_DATA_DIR}" STREQUAL "x" )
  list (APPEND _genvtx_test_additional_options --materials-data-dir ${materials_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_genvtx_test_additional_options}'" )
endif ()

# depends on geomtools data dir:
if (DEFINED geomtools_DATA_DIR
    AND NOT "x${geomtools_DATA_DIR}" STREQUAL "x" )
  list (APPEND _genvtx_test_additional_options --geomtools-data-dir ${geomtools_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_genvtx_test_additional_options}'" )
endif ()

execute_process(COMMAND cmake -E remove_directory ${_test_tmp_dir} )

foreach(test_program ${genvtx_tests_programs_list})
  get_filename_component(test_executable ${test_program} NAME_WE)
  add_executable(${test_executable} ${test_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${test_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_test_libraries genvtx)

  # # Special bio DLL plugins :
  # if(datatools_WITH_BIO)
  #   list(INSERT _test_libraries 0 datatools_bio)
  # endif()
  # if(mygsl_WITH_BIO)
  #   list(INSERT _test_libraries 0 mygsl_bio )
  # endif()
  # if(GENVTX_WITH_BIO)
  #   list(INSERT _test_libraries 0 genvtx_bio )
  # endif()

  # # Special reflection DLL plugins :
  # if(datatools_WITH_REFLECTION)
  #   list(INSERT _test_libraries 0 datatools_reflection)
  # endif()
  # if(GENVTX_WITH_REFLECTION)
  #   list(INSERT _test_libraries 0 genvtx_reflection )
  # endif()

  target_link_libraries(${test_executable} ${_test_libraries})

  add_test(NAME ${test_executable}.run
    COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash
    --tmp-dir ${_test_tmp_dir}
    --data-dir ${PROJECT_SOURCE_DIR}
    --prefix ${PROJECT_SOURCE_DIR}
    --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
    ${_genvtx_test_additional_options}
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
