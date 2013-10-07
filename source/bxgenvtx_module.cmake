# - Module definition for genvtx submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name genvtx)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")

foreach(dir root_dir include_dir source_dir app_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(genvtx_VERSION_MAJOR 4)
set(genvtx_VERSION_MINOR 0)
set(genvtx_VERSION_PATCH 0)
set(genvtx_VERSION "${genvtx_VERSION_MAJOR}.${genvtx_VERSION_MINOR}.${genvtx_VERSION_PATCH}")
set(GENVTX_WITH_BIO 1)

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/box_vg.h
  ${module_include_dir}/${module_name}/cylinder_vg.h
  ${module_include_dir}/${module_name}/from_file_vg.h
  ${module_include_dir}/${module_name}/i_vertex_generator.h
  ${module_include_dir}/${module_name}/placement_vg.h
  ${module_include_dir}/${module_name}/quadrangle_random_tools.h
  ${module_include_dir}/${module_name}/spot_vertex_generator.h
  ${module_include_dir}/${module_name}/triangle_random_tools.h
  ${module_include_dir}/${module_name}/tube_vg.h
  ${module_include_dir}/${module_name}/vg_macros.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/manager.h
  ${module_include_dir}/${module_name}/combined_vg.h
  ${module_include_dir}/${module_name}/box_model_vg.h
  ${module_include_dir}/${module_name}/cylinder_model_vg.h
  ${module_include_dir}/${module_name}/tube_model_vg.h
  ${module_include_dir}/${module_name}/genvtx_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/genvtx_driver.h
  ${module_include_dir}/${module_name}/detail/vg_tools.h
  ${module_include_dir}/${module_name}/detail/geom_manager_utils.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/i_vertex_generator.cc
  ${module_source_dir}/spot_vertex_generator.cc
  ${module_source_dir}/from_file_vg.cc
  ${module_source_dir}/placement_vg.cc
  ${module_source_dir}/box_vg.cc
  ${module_source_dir}/cylinder_vg.cc
  ${module_source_dir}/tube_vg.cc
  ${module_source_dir}/vg_tools.cc
  ${module_source_dir}/triangle_random_tools.cc
  ${module_source_dir}/quadrangle_random_tools.cc
  ${module_source_dir}/utils.cc
  ${module_source_dir}/manager.cc
  ${module_source_dir}/combined_vg.cc
  ${module_source_dir}/box_model_vg.cc
  ${module_source_dir}/cylinder_model_vg.cc
  ${module_source_dir}/tube_model_vg.cc
  ${module_source_dir}/geom_manager_utils.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/genvtx_driver.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "GENVTX_DATA_DIR=${module_root_dir};MATERIALS_DATA_DIR=${module_root_dir}/../bxmaterials")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_genvtx.cxx
  ${module_test_dir}/test_spot_vertex_generator.cxx
  ${module_test_dir}/test_box_vg.cxx
  ${module_test_dir}/test_cylinder_vg.cxx
  ${module_test_dir}/test_tube_vg.cxx
  ${module_test_dir}/test_placement_vg.cxx
  ${module_test_dir}/test_from_file_vg.cxx
  ${module_test_dir}/test_triangle_random_tools.cxx
  ${module_test_dir}/test_quadrangle_random_tools.cxx
  ${module_test_dir}/test_manager.cxx
  )

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/genvtx_production.cxx
  )
