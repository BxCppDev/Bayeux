# - Module definition for cuts submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name cuts)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(cuts_VERSION_MAJOR 4)
set(cuts_VERSION_MINOR 0)
set(cuts_VERSION_PATCH 0)
set(cuts_VERSION "${cuts_VERSION_MAJOR}.${cuts_VERSION_MINOR}.${cuts_VERSION_PATCHD}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/accept_cut.h
  ${module_include_dir}/${module_name}/and_cut.h
  ${module_include_dir}/${module_name}/cut_macros.h
  ${module_include_dir}/${module_name}/cut_manager.h
  ${module_include_dir}/${module_name}/cuts_config.h.in
  ${module_include_dir}/${module_name}/cut_service.h
  ${module_include_dir}/${module_name}/cut_tools.h
  ${module_include_dir}/${module_name}/i_binary_cut.h
  ${module_include_dir}/${module_name}/i_cut.h
  ${module_include_dir}/${module_name}/i_multi_cut.h
  ${module_include_dir}/${module_name}/multi_and_cut.h
  ${module_include_dir}/${module_name}/multi_or_cut.h
  ${module_include_dir}/${module_name}/multi_xor_cut.h
  ${module_include_dir}/${module_name}/nand_cut.h
  ${module_include_dir}/${module_name}/nor_cut.h
  ${module_include_dir}/${module_name}/not_cut.h
  ${module_include_dir}/${module_name}/ocd_support.h
  ${module_include_dir}/${module_name}/or_cut.h
  ${module_include_dir}/${module_name}/random_cut.h
  ${module_include_dir}/${module_name}/reject_cut.h
  ${module_include_dir}/${module_name}/xnor_cut.h
  ${module_include_dir}/${module_name}/xor_cut.h
  ${module_include_dir}/${module_name}/version.h.in
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/accept_cut.cc
  ${module_source_dir}/and_cut.cc
  ${module_source_dir}/cut_manager.cc
  ${module_source_dir}/cuts_config.cc
  ${module_source_dir}/cut_service.cc
  ${module_source_dir}/cut_tools.cc
  ${module_source_dir}/i_binary_cut.cc
  ${module_source_dir}/i_cut.cc
  ${module_source_dir}/i_multi_cut.cc
  ${module_source_dir}/multi_and_cut.cc
  ${module_source_dir}/multi_or_cut.cc
  ${module_source_dir}/multi_xor_cut.cc
  ${module_source_dir}/nand_cut.cc
  ${module_source_dir}/nor_cut.cc
  ${module_source_dir}/not_cut.cc
  ${module_source_dir}/ocd_support.cc
  ${module_source_dir}/or_cut.cc
  ${module_source_dir}/random_cut.cc
  ${module_source_dir}/reject_cut.cc
  ${module_source_dir}/xnor_cut.cc
  ${module_source_dir}/xor_cut.cc
  ${module_source_dir}/version.cc
    )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "CUTS_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_accept_cut.cxx
  ${module_test_dir}/test_and_cut.cxx
  ${module_test_dir}/test_cuts.cxx
  ${module_test_dir}/test_manager.cxx
  ${module_test_dir}/test_random_cut.cxx
  ${module_test_dir}/test_version.cxx
  )

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )
