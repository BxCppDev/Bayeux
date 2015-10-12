# - Module definition for dpp submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name dpp)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir test_dir app_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(dpp_VERSION_MAJOR 3)
set(dpp_VERSION_MINOR 0)
set(dpp_VERSION_PATCH 0)
set(dpp_VERSION "${dpp_VERSION_MAJOR}.${dpp_VERSION_MINOR}.${dpp_VERSION_PATCH}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  #${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/base_module.h
  ${module_include_dir}/${module_name}/brio_common.h
  ${module_include_dir}/${module_name}/chain_module.h
  ${module_include_dir}/${module_name}/context_service.h
  ${module_include_dir}/${module_name}/dummy_module.h
  ${module_include_dir}/${module_name}/i_data_sink.h
  ${module_include_dir}/${module_name}/i_data_source.h
  ${module_include_dir}/${module_name}/if_module.h
  ${module_include_dir}/${module_name}/io_common.h
  ${module_include_dir}/${module_name}/input_module.h
  ${module_include_dir}/${module_name}/output_module.h
  #${module_include_dir}/${module_name}/module_macros.h
  ${module_include_dir}/${module_name}/module_manager.h
  ${module_include_dir}/${module_name}/module_tools.h
  ${module_include_dir}/${module_name}/simple_brio_data_sink.h
  ${module_include_dir}/${module_name}/simple_brio_data_source.h
  ${module_include_dir}/${module_name}/simple_data_sink.h
  ${module_include_dir}/${module_name}/simple_data_source.h
  ${module_include_dir}/${module_name}/skip_module.h
  ${module_include_dir}/${module_name}/utils_module.h
  ${module_include_dir}/${module_name}/dump_module.h
  ${module_include_dir}/${module_name}/utils_cut.h
  ${module_include_dir}/${module_name}/histogram_service.h
  ${module_include_dir}/${module_name}/ocd_support.h
  ${module_include_dir}/${module_name}/dpp_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/dpp_driver.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/utils_cut.cc
  ${module_source_dir}/base_module.cc
  ${module_source_dir}/brio_common.cc
  ${module_source_dir}/chain_module.cc
  ${module_source_dir}/context_service.cc
  ${module_source_dir}/dummy_module.cc
  ${module_source_dir}/if_module.cc
  ${module_source_dir}/io_common.cc
  ${module_source_dir}/input_module.cc
  ${module_source_dir}/output_module.cc
  ${module_source_dir}/module_manager.cc
  ${module_source_dir}/module_tools.cc
  ${module_source_dir}/i_data_sink.cc
  ${module_source_dir}/i_data_source.cc
  ${module_source_dir}/simple_brio_data_sink.cc
  ${module_source_dir}/simple_brio_data_source.cc
  ${module_source_dir}/simple_data_sink.cc
  ${module_source_dir}/simple_data_source.cc
  ${module_source_dir}/skip_module.cc
  ${module_source_dir}/utils_module.cc
  ${module_source_dir}/dump_module.cc
  ${module_source_dir}/histogram_service.cc
  ${module_source_dir}/ocd_support.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/dpp_driver.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "DPP_TESTING_DIR=${module_test_dir}")
list(APPEND ${module_name}_TEST_ENVIRONMENT "DPP_TMP_TEST_DIR=${CMAKE_BINARY_DIR}/Testing" )

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_module.cxx
  ${module_test_dir}/test_input_output_modules.cxx
  ${module_test_dir}/test_module_chain.cxx #<- Requires program_options
  ${module_test_dir}/test_module_manager.cxx
  ${module_test_dir}/test_histogram_service.cxx
  )

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/dpp_processing.cxx
  )

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )
