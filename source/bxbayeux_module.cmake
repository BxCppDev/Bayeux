# - Module definition for bayeux submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name bayeux)
set(module_root_dir    "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

set(MANGLE_BINRELOC "libBayeux")

# - Unpublished header
configure_file(${module_include_dir}/${module_name}/BayeuxBinReloc.h.in
  bx${module_name}/${module_name}/BayeuxBinReloc.h
  @ONLY
  )

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/bayeux_config.h.in
  ${module_include_dir}/${module_name}/bayeux.h
  ${module_include_dir}/${module_name}/reloc.h
  ${module_include_dir}/${module_name}/version.h.in
  )

# - configure special source files
configure_file(${module_source_dir}/_bayeux.cc.in
  bx${module_name}/_bayeux.cc
  @ONLY
  )
configure_file(${module_source_dir}/reloc.cc.in
  bx${module_name}/reloc.cc
  @ONLY
  )
configure_file(${module_source_dir}/BayeuxBinReloc.c.in
  bx${module_name}/BayeuxBinReloc.c
  @ONLY
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/version.cc
  ${module_source_dir}/bayeux.cc
  ${module_source_dir}/the_introspectable.cc
  bx${module_name}/_bayeux.cc
  bx${module_name}/reloc.cc
  bx${module_name}/${module_name}/BayeuxBinReloc.h
  bx${module_name}/BayeuxBinReloc.c
  )

set(${module_name}_ENDING_MODULE_SOURCES )

# - Publish headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..
  ${CMAKE_CURRENT_BINARY_DIR}/bx${module_name}
  )

if (BAYEUX_WITH_IMPLICIT_INIT_FINI)
  list(APPEND ${module_name}_ENDING_MODULE_SOURCES
    bx${module_name}/_init_fini.cc
    )
endif()

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/query.cxx
  )


set(${module_name}_MODULE_TESTS
${module_test_dir}/test_bayeux.cxx
)
