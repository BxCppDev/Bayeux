# - Module definition for datatools submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name epa)
set(module_root_dir     "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir  "${module_root_dir}/include")
set(module_source_dir   "${module_root_dir}/src")


foreach(dir root_dir include_dir source_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS

  ${module_include_dir}/boost/archive/portable_archive_exception.hpp
  ${module_include_dir}/boost/archive/portable_iarchive.hpp
  ${module_include_dir}/boost/archive/portable_oarchive.hpp
)

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/portable_iarchive.cpp
  ${module_source_dir}/portable_oarchive.cpp
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

