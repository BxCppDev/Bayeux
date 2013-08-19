# - Module definition for brio submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

#-----------------------------------------------------------------------
# - Module
set(module_name brio)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(brio_VERSION_MAJOR 1)
set(brio_VERSION_MINOR 0)
set(brio_PATCH_VERSION 0)
set(brio_VERSION "${brio_VERSION_MAJOR}.${brio_VERSION_MINOR}.${brio_PATCH_VERSION}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/brio_config.h.in
  ${module_include_dir}/${module_name}/brio.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/reader.h
  ${module_include_dir}/${module_name}/reader-inl.h
  ${module_include_dir}/${module_name}/writer.h
  ${module_include_dir}/${module_name}/writer-inl.h
  ${module_include_dir}/${module_name}/detail/base_io.h
  ${module_include_dir}/${module_name}/detail/brio_record.h
  ${module_include_dir}/${module_name}/detail/TArrayCMod.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/base_io.cc
  ${module_source_dir}/brio_record.cc
  ${module_source_dir}/utils.cc
  ${module_source_dir}/reader.cc
  ${module_source_dir}/writer.cc
  ${module_source_dir}/TArrayCMod.cc
  )

# - Generate ROOT headers
root_generate_dictionary("${module_include_dir}/${module_name}/detail/brio_record.h;${module_include_dir}/${module_name}/detail/TArrayCMod.h" 
  "${PROJECT_SOURCE_DIR}/source/brio_linkdef.h" 
  "${PROJECT_BINARY_DIR}/brio_dict.cc"
  "${module_include_dir}"
  )
list(APPEND ${module_name}_MODULE_SOURCES ${PROJECT_BINARY_DIR}/brio_dict.cc)

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "BRIO_DATA_DIR=${module_root_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_brio.cxx
  ${module_test_dir}/test_data_io.cxx
  ${module_test_dir}/test_brio_writer_1.cxx
  ${module_test_dir}/test_brio_reader_1.cxx
  ${module_test_dir}/test_brio_writer_2.cxx
  ${module_test_dir}/test_brio_reader_2.cxx
  ${module_test_dir}/test_writer.cxx
  ${module_test_dir}/test_reader.cxx
  ${module_test_dir}/test_writer_2.cxx
  ${module_test_dir}/test_reader_2.cxx
  ${module_test_dir}/test_reader_3.cxx
  )
