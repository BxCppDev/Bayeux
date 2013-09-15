# - Module definition for trackfit submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name trackfit)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(trackfit_VERSION_MAJOR 3)
set(trackfit_VERSION_MINOR 0)
set(trackfit_VERSION_PATCH 0)
set(trackfit_VERSION "${trackfit_VERSION_MAJOR}.${trackfit_VERSION_MINOR}.${trackfit_VERSION_PATCH}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/drawing.h
  ${module_include_dir}/${module_name}/fit_utils.h
  ${module_include_dir}/${module_name}/gg_hit.h
  ${module_include_dir}/${module_name}/helix_fit_mgr.h
  ${module_include_dir}/${module_name}/i_drift_time_calibration.h
  ${module_include_dir}/${module_name}/line_fit_mgr.h
  ${module_include_dir}/${module_name}/trackfit_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/drawing.cc
  ${module_source_dir}/fit_utils.cc
  ${module_source_dir}/gg_hit.cc
  ${module_source_dir}/helix_fit_mgr.cc
  ${module_source_dir}/i_drift_time_calibration.cc
  ${module_source_dir}/line_fit_mgr.cc
  ${module_source_dir}/version.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "TRACKFIT_DATA_DIR=${module_root_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_drawing.cxx
  ${module_test_dir}/test_drift_time_calibration.cxx
  ${module_test_dir}/test_gg_hit.cxx
  ${module_test_dir}/test_helix_fit_mgr.cxx
  ${module_test_dir}/test_line_fit_mgr.cxx
  ${module_test_dir}/test_trackfit.cxx
  )
