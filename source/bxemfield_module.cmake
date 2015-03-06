# - Module definition for emfield submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name emfield)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(emfield_VERSION_MAJOR 1)
set(emfield_VERSION_MINOR 0)
set(emfield_VERSION_PATCH 0)
set(emfield_VERSION "${emfield_VERSION_MAJOR}.${emfield_VERSION_MINOR}.${emfield_VERSION_PATCH}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/base_electromagnetic_field.h
  ${module_include_dir}/${module_name}/electromagnetic_field_macros.h
  ${module_include_dir}/${module_name}/electromagnetic_field_manager.h
  ${module_include_dir}/${module_name}/linear_combination_field.h
  ${module_include_dir}/${module_name}/oscillating_field.h
  ${module_include_dir}/${module_name}/placement_field.h
  ${module_include_dir}/${module_name}/uniform_electric_field.h
  ${module_include_dir}/${module_name}/uniform_magnetic_field.h
  ${module_include_dir}/${module_name}/emfield_geom_plugin.h
  ${module_include_dir}/${module_name}/geom_map.h
  ${module_include_dir}/${module_name}/emfield_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/base_electromagnetic_field.cc
  ${module_source_dir}/electromagnetic_field_manager.cc
  ${module_source_dir}/linear_combination_field.cc
  ${module_source_dir}/oscillating_field.cc
  ${module_source_dir}/placement_field.cc
  ${module_source_dir}/uniform_electric_field.cc
  ${module_source_dir}/uniform_magnetic_field.cc
  ${module_source_dir}/emfield_geom_plugin.cc
  ${module_source_dir}/geom_map.cc
  ${module_source_dir}/version.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "EMFIELD_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_emfield.cxx
  ${module_test_dir}/test_emfield_manager.cxx
  # ${module_test_dir}/test_emfield_geom_plugin.cxx
  ${module_test_dir}/test_oscillating_field.cxx
  )
