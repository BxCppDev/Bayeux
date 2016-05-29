# - Module definition for LaHague submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name lahague)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/source")
set(module_source_dir  "${module_root_dir}/source/lahague")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")
set(module_examples_dir "${module_root_dir}/examples")

# Store explicitely this resource dir for further usage during other modules' tests
set(bxlahague_resource_dir ${module_resource_dir})

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(lahague_VERSION_MAJOR 0)
set(lahague_VERSION_MINOR 1)
set(lahague_VERSION_PATCH 0)
set(lahague_VERSION "${lahague_VERSION_MAJOR}.${lahague_VERSION_MINOR}.${lahague_VERSION_PATCH}")

# - configure special source file
configure_file(${module_source_dir}/_lahague.cc.in
               bx${module_name}/_lahague.cc
              )

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/lahague.h
  ${module_include_dir}/${module_name}/lahague_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/resource.h
  ${module_include_dir}/${module_name}/dosimetry/fluence_hit.h
  ${module_include_dir}/${module_name}/dosimetry/fluence_hit-serial.h
  ${module_include_dir}/${module_name}/dosimetry/fluence_to_h10.h
  ${module_include_dir}/${module_name}/dosimetry/radiation_weighting_factor.h
  ${module_include_dir}/${module_name}/the_serializable.h
  )

# - configure resources
configure_file(${module_source_dir}/resource.cc.in
               bx${module_name}/resource.cc)

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/version.cc
  ${module_source_dir}/lahague.cc
  ${module_source_dir}/dosimetry/fluence_hit.cc
  ${module_source_dir}/dosimetry/fluence_to_h10.cc
  ${module_source_dir}/dosimetry/radiation_weighting_factor.cc
  ${module_source_dir}/the_serializable.cc
  bx${module_name}/resource.cc
  bx${module_name}/_lahague.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "LAHAGUE_RESOURCE_DIR=${module_resource_dir};LAHAGUE_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_lahague.cxx
  ${module_test_dir}/test_resource.cxx
  ${module_test_dir}/test_version.cxx
  ${module_test_dir}/test-dosimetry-fluence_hit
  )

# - Applications
# set(${module_name}_MODULE_APPS
#   ${module_app_dir}/lahague_inspector.cxx
#   )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/data/README.rst
  ${module_resource_dir}/data/dosimetry/README.rst
  ${module_resource_dir}/data/dosimetry/gamma_fluence_to_h10.table
  ${module_resource_dir}/data/dosimetry/neutron_fluence_to_h10.table
  )

# - Publish resource files
foreach(_rfin ${${module_name}_MODULE_RESOURCES})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${module_resource_dir}" "${MODULE_RESOURCE_ROOT}" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )
