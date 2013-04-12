###############################################################################
##
## Copyright (C) 2011-2013 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the mygsl library.
##
## mygsl is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## mygsl is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with mygsl.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################


###file(GLOB mygsl_tests_programs test_*.cxx)
set ( mygsl_tests_programs
  test_mygsl.cxx 
  test_best_value.cxx
  test_constants.cxx
  test_datapoint.cxx
  #test_error.cxx
  #test_fft_real2.cxx
  #test_fft_real.cxx
  test_histogram.cxx
  test_histogram_2d.cxx
  test_histogram_pool.cxx
  test_histogram_service.cxx
  test_interval.cxx
  test_ioutils.cxx
  test_linear_regression.cxx
  test_linear_system_solver.cxx
  test_mean.cxx
  test_min_max.cxx
  #test_multidimensional_minimization.cxx
  test_multi_eval.cxx
  test_multimin.cxx
  test_multiparameter_system.cxx
  test_numerical_differentiation.cxx
  test_ode.cxx
  test_one_dimensional_minimization.cxx
  test_one_dimensional_root_finding.cxx
  test_permutation.cxx
  test_polynomial.cxx
  test_rng_2.cxx
  test_rng.cxx
  test_tabulated_function_2.cxx
  test_tabulated_function_3.cxx
  test_tabulated_function.cxx
  test_von_neumann.cxx
  test_prng_state_manager.cxx
  test_seed_manager.cxx
  test_i_unary_function.cxx
)

###message (STATUS "mygsl_tests_programs=${mygsl_tests_programs}" )
list(REMOVE_ITEM mygsl_tests_programs ${CMAKE_CURRENT_SOURCE_DIR}/test_reflection_0.cxx)
if(MYGSL_WITH_REFLECTION)
  list(APPEND mygsl_tests_programs ${CMAKE_CURRENT_SOURCE_DIR}/test_reflection_0.cxx)
endif() 

set(mygsl_tests_programs_list "${mygsl_tests_programs}") 
list(SORT mygsl_tests_programs_list)

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${GSL_INCLUDE_DIRS}
  ${datatools_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

set(_test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/testing_output")

execute_process(COMMAND cmake -E remove_directory ${_test_tmp_dir} )

foreach(test_program ${mygsl_tests_programs_list})
  get_filename_component(test_executable ${test_program} NAME_WE)
  add_executable(${test_executable} ${test_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${test_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_test_libraries mygsl) 
  if(MYGSL_WITH_BIO)
    list(INSERT _test_libraries 0 mygsl_bio datatools_bio)
  endif() 
  if(MYGSL_WITH_REFLECTION)
    list(INSERT _test_libraries 0 mygsl_reflection datatools_reflection)
  endif() 

  target_link_libraries(${test_executable} ${_test_libraries})

  add_test(NAME ${test_executable}.run
    COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
    --tmp-dir ${_test_tmp_dir}
    --data-dir ${PROJECT_SOURCE_DIR} 
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
