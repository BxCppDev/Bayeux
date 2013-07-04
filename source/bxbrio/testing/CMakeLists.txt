###############################################################################
##
## Copyright (C) 2011-2013 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the brio library.
##
## brio is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## brio is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with brio.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

# Explicitely in this order because some 'reader' test programs use the output
# files produced by some 'writer' test programs :
set ( brio_tests_programs_list
      test_brio.cxx
      test_data_io.cxx
      test_brio_writer_1.cxx
      test_brio_reader_1.cxx
      test_brio_writer_2.cxx
      test_brio_reader_2.cxx
      test_writer.cxx
      test_reader.cxx
      test_writer_2.cxx
      test_reader_2.cxx
      test_reader_3.cxx
    )

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/testing
  ${datatools_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIRS}
  )

set(_test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/testing_output")

execute_process(COMMAND cmake -E remove_directory ${_test_tmp_dir} )

foreach(test_program ${brio_tests_programs_list})
  get_filename_component(test_executable ${test_program} NAME_WE)
  add_executable(${test_executable} ${test_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${test_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_test_libraries brio)
  #message (STATUS "testing: datatools_WITH_BIO=${datatools_WITH_BIO}")
  #message (STATUS "testing: datatools_BIO_LIBRARY=${datatools_BIO_LIBRARY}")

  target_link_libraries(${test_executable} ${_test_libraries})

  add_test(NAME ${test_executable}.run
    COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash
    --tmp-dir ${_test_tmp_dir}
    --prefix ${PROJECT_SOURCE_DIR}
    --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
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
