# - Module definition for events submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name events)
set(module_root_dir     "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir  "${module_root_dir}/include")
set(module_source_dir   "${module_root_dir}/src")
set(module_test_dir     "${module_root_dir}/testing")
set(module_app_dir      "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(events_VERSION_MAJOR 1)
set(events_VERSION_MINOR 0)
set(events_VERSION_PATCH 0)
set(events_VERSION "${events_VERSION_MAJOR}.${events_VERSION_MINOR}.${events_VERSION_PATCH}")

# - Boost I/O, CAMP Reflection
set(EVENTS_WITH_BIO 1)
set(EVENTS_WITH_REFLECTION 1)

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/base_event.h
  ${module_include_dir}/${module_name}/base_event.ipp
  ${module_include_dir}/${module_name}/timestamp.h
  ${module_include_dir}/${module_name}/timestamp.ipp
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/base_event.cc
  ${module_source_dir}/timestamp.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout
    "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()


# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "EVENTS_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_timestamp.cxx
  ${module_test_dir}/test_base_event.cxx
  )

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )

# end
