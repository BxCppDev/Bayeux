# - Module definition for bayeux submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file
# MODULE_RESOURCE_ROOT must be set before including this file
# MODULE_EXAMPLE_ROOT must be set before including this file

# if(NOT MODULE_HEADER_ROOT)
#   message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
# endif()
foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name bayeux)
set(module_root_dir    "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
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
  ${module_include_dir}/${module_name}/bayeux_init.h
  ${module_include_dir}/${module_name}/bayeux.h
  ${module_include_dir}/${module_name}/reloc.h
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/resource.h
  ${module_include_dir}/${module_name}/detail/bayeux_library.h
  )

# - configure special source files
# configure_file(${module_source_dir}/_bayeux.cc.in
#   bx${module_name}/_bayeux.cc
#   @ONLY
#   )
configure_file(${module_source_dir}/detail/bayeux_library.cc.in
  bx${module_name}/detail/bayeux_library.cc
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
  ${module_source_dir}/resource.cc
  # ${module_source_dir}/detail/bayeux_library.cc
  bx${module_name}/detail/bayeux_library.cc
  bx${module_name}/reloc.cc
  bx${module_name}/${module_name}/BayeuxBinReloc.h
  bx${module_name}/BayeuxBinReloc.c
  )

set(${module_name}_ENDING_MODULE_SOURCES
  ${module_source_dir}/the_introspectable.cc
  )

# - Publish headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/query.cxx
  )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/images/logo/logo_bayeux_240x200_transparent.png
  ${module_resource_dir}/images/logo/logo_bayeux_300x250_transparent.png
  ${module_resource_dir}/images/logo/logo_bayeux_66x55_transparent.png
  ${module_resource_dir}/urn/db/bayeux_logos_infos.def
  ${module_resource_dir}/urn/resolvers/bayeux_logos_urn_map.conf
  )

# - Publish resource files
foreach(_rfin ${${module_name}_MODULE_RESOURCES})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${module_resource_dir}" "${MODULE_RESOURCE_ROOT}" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "BAYEUX_RESOURCE_DIR=${module_resource_dir};BAYEUX_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_bayeux.cxx
)

# Unused 'catch' system:
# set(${module_name}_MODULE_TESTS_CATCH
#   # ${module_test_dir}/test_bayeux_catch.cxx
# )

# end
