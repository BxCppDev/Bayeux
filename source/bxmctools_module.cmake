# - Module definition for mctools submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name mctools)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(mctools_VERSION_MAJOR 1)
set(mctools_VERSION_MINOR 0)
set(mctools_VERSION_PATCH 0)
set(mctools_VERSION "${mctools_VERSION_MAJOR}.${mctools_VERSION_MINOR}.${mctools_VERSION_PATCH}")


