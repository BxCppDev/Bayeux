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
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(brio_VERSION_MAJOR 6)
set(brio_VERSION_MINOR 2)
set(brio_VERSION_PATCH 0)
set(brio_VERSION "${brio_VERSION_MAJOR}.${brio_VERSION_MINOR}.${brio_VERSION_PATCH}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/brio_config.h.in
  ${module_include_dir}/${module_name}/brio.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/reader.h
  ${module_include_dir}/${module_name}/reader-inl.h
  ${module_include_dir}/${module_name}/writer.h
  ${module_include_dir}/${module_name}/writer-inl.h
  ${module_include_dir}/${module_name}/version.h.in
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
  ${module_source_dir}/version.cc
  ${module_source_dir}/TArrayCMod.cc
  )

# - Generate ROOT headers
if(ROOT_VERSION VERSION_GREATER 5.99)
  # ROOT6 dictionary generation requires a MODULE arg
  set(__BRIO_MODULE_ARG MODULE Bayeux)
  set(__BRIO_ROOT_GEN_DICT_OPTS "-noIncludePaths")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/bxbrio/include)
root_generate_dictionary(brio_dict
  ${__BRIO_MODULE_ARG}
  LINKDEF "${PROJECT_SOURCE_DIR}/source/brio_linkdef.h"
  OPTIONS ${__BRIO_ROOT_GEN_DICT_OPTS}
  )


list(APPEND ${module_name}_MODULE_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/brio_dict.cxx)
if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
  set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/brio_dict.cxx
    PROPERTY COMPILE_FLAGS "-Wno-keyword-macro -Wno-c++11-long-long"
    )
endif()

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "BRIO_TESTING_DIR=${module_test_dir}")

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

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )
