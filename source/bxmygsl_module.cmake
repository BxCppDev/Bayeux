# - Module definition for mygsl submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name mygsl)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(mygsl_VERSION_MAJOR 4)
set(mygsl_VERSION_MINOR 0)
set(mygsl_PATCH_VERSION 0)
set(mygsl_VERSION "${mygsl_VERSION_MAJOR}.${mygsl_VERSION_MINOR}.${mygsl_PATCH_VERSION}")
set(MYGSL_WITH_BIO ON)


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/best_value.h
  ${module_include_dir}/${module_name}/best_value.ipp
  ${module_include_dir}/${module_name}/bio_guard.h
  ${module_include_dir}/${module_name}/constants.h
  ${module_include_dir}/${module_name}/datapoint.h
  ${module_include_dir}/${module_name}/datapoint.ipp
  ${module_include_dir}/${module_name}/detail/bio_link_guard.h
  ${module_include_dir}/${module_name}/error.h
  ${module_include_dir}/${module_name}/fft_real.h
  ${module_include_dir}/${module_name}/histogram_1d.h
  ${module_include_dir}/${module_name}/histogram_2d.h
  ${module_include_dir}/${module_name}/histogram_2d.ipp
  ${module_include_dir}/${module_name}/histogram.h
  ${module_include_dir}/${module_name}/histogram.ipp
  ${module_include_dir}/${module_name}/histogram_pool.h
  ${module_include_dir}/${module_name}/histogram_pool.ipp
  ${module_include_dir}/${module_name}/histogram_utils.h
  ${module_include_dir}/${module_name}/interval.h
  ${module_include_dir}/${module_name}/interval.ipp
  ${module_include_dir}/${module_name}/ioutils.h
  ${module_include_dir}/${module_name}/i_unary_function.h
  ${module_include_dir}/${module_name}/i_unary_function_with_derivative.h
  ${module_include_dir}/${module_name}/linear_regression.h
  ${module_include_dir}/${module_name}/linear_system_solver.h
  ${module_include_dir}/${module_name}/mean.h
  ${module_include_dir}/${module_name}/min_max.h
  ${module_include_dir}/${module_name}/multidimensional_minimization.h
  ${module_include_dir}/${module_name}/multi_eval.h
  ${module_include_dir}/${module_name}/multimin.h
  ${module_include_dir}/${module_name}/multiparameter_system.h
  ${module_include_dir}/${module_name}/mygsl_config.h.in
  ${module_include_dir}/${module_name}/numerical_differentiation.h
  ${module_include_dir}/${module_name}/ode.h
  ${module_include_dir}/${module_name}/one_dimensional_minimization.h
  ${module_include_dir}/${module_name}/one_dimensional_root_finding.h
  ${module_include_dir}/${module_name}/param_entry.h
  ${module_include_dir}/${module_name}/permutation.h
  ${module_include_dir}/${module_name}/polynomial.h
  ${module_include_dir}/${module_name}/polynomial.ipp
  ${module_include_dir}/${module_name}/prng_state_manager.h
  ${module_include_dir}/${module_name}/random_utils.h
  ${module_include_dir}/${module_name}/rng.h
  ${module_include_dir}/${module_name}/seed_manager.h
  ${module_include_dir}/${module_name}/tabulated_function.h
  ${module_include_dir}/${module_name}/the_serializable.h
  ${module_include_dir}/${module_name}/the_serializable.ipp
  ${module_include_dir}/${module_name}/von_neumann_method.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/best_value.cc
  ${module_source_dir}/constants.cc
  ${module_source_dir}/datapoint.cc
  ${module_source_dir}/error.cc
  ${module_source_dir}/fft_real.cc
  ${module_source_dir}/histogram_2d.cc
  ${module_source_dir}/histogram.cc
  ${module_source_dir}/histogram_pool.cc
  ${module_source_dir}/interval.cc
  ${module_source_dir}/ioutils.cc
  ${module_source_dir}/i_unary_function.cc
  ${module_source_dir}/i_unary_function_with_derivative.cc
  ${module_source_dir}/linear_regression.cc
  ${module_source_dir}/linear_system_solver.cc
  ${module_source_dir}/mean.cc
  ${module_source_dir}/min_max.cc
  ${module_source_dir}/multidimensional_minimization.cc
  ${module_source_dir}/multi_eval.cc
  ${module_source_dir}/multimin.cc
  ${module_source_dir}/multiparameter_system.cc
  ${module_source_dir}/mygsl_config.cc
  ${module_source_dir}/numerical_differentiation.cc
  ${module_source_dir}/ode.cc
  ${module_source_dir}/one_dimensional_minimization.cc
  ${module_source_dir}/one_dimensional_root_finding.cc
  ${module_source_dir}/param_entry.cc
  ${module_source_dir}/permutation.cc
  ${module_source_dir}/polynomial.cc
  ${module_source_dir}/prng_state_manager.cc
  ${module_source_dir}/random_utils.cc
  ${module_source_dir}/rng.cc
  ${module_source_dir}/seed_manager.cc
  ${module_source_dir}/tabulated_function.cc
  ${module_source_dir}/the_serializable.cc
  ${module_source_dir}/von_neumann_method.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "MYGSL_DATA_DIR=${module_root_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_best_value.cxx
  ${module_test_dir}/test_constants.cxx
  ${module_test_dir}/test_datapoint.cxx
  ${module_test_dir}/test_error.cxx
  ${module_test_dir}/test_fft_real2.cxx
  ${module_test_dir}/test_fft_real.cxx
  ${module_test_dir}/test_histogram_2d.cxx
  ${module_test_dir}/test_histogram.cxx
  ${module_test_dir}/test_histogram_pool.cxx
  ${module_test_dir}/test_interval.cxx
  ${module_test_dir}/test_ioutils.cxx
  ${module_test_dir}/test_i_unary_function.cxx
  ${module_test_dir}/test_linear_regression.cxx
  ${module_test_dir}/test_linear_system_solver.cxx
  ${module_test_dir}/test_mean.cxx
  ${module_test_dir}/test_min_max.cxx
  ${module_test_dir}/test_multidimensional_minimization.cxx
  ${module_test_dir}/test_multi_eval.cxx
  ${module_test_dir}/test_multimin.cxx
  ${module_test_dir}/test_multiparameter_system.cxx
  ${module_test_dir}/test_mygsl.cxx
  ${module_test_dir}/test_numerical_differentiation.cxx
  ${module_test_dir}/test_ode.cxx
  ${module_test_dir}/test_one_dimensional_minimization.cxx
  ${module_test_dir}/test_one_dimensional_root_finding.cxx
  ${module_test_dir}/test_permutation.cxx
  ${module_test_dir}/test_polynomial.cxx
  ${module_test_dir}/test_prng_state_manager.cxx
  ${module_test_dir}/test_rng_2.cxx
  ${module_test_dir}/test_rng.cxx
  ${module_test_dir}/test_seed_manager.cxx
  ${module_test_dir}/test_tabulated_function_2.cxx
  ${module_test_dir}/test_tabulated_function_3.cxx
  ${module_test_dir}/test_tabulated_function.cxx
  ${module_test_dir}/test_von_neumann.cxx
  )
