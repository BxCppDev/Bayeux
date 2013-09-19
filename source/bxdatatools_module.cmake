# - Module definition for datatools submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name datatools)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")

foreach(dir root_dir include_dir source_dir test_dir app_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(datatools_VERSION_MAJOR 5)
set(datatools_VERSION_MINOR 0)
set(datatools_VERSION_PATCH 0)
set(datatools_VERSION "${datatools_VERSION_MAJOR}.${datatools_VERSION_MINOR}.${datatools_VERSION_PATCH}")

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/advanced_object.h
  ${module_include_dir}/${module_name}/advanced_object.ipp
  ${module_include_dir}/${module_name}/archives_instantiation.h
  ${module_include_dir}/${module_name}/archives_list.h
  ${module_include_dir}/${module_name}/base_service.h
  ${module_include_dir}/${module_name}/bio_guard.h
  ${module_include_dir}/${module_name}/bit_mask.h
  ${module_include_dir}/${module_name}/caster_utils.h
  ${module_include_dir}/${module_name}/clhep_units.h
  ${module_include_dir}/${module_name}/datatools_config.h.in
  ${module_include_dir}/${module_name}/detail/api.h
  ${module_include_dir}/${module_name}/detail/bio_link_guard.h
  ${module_include_dir}/${module_name}/detail/Configure.h
  ${module_include_dir}/${module_name}/detail/DynamicLoader.h
  ${module_include_dir}/${module_name}/detail/ocd_utils.h
  ${module_include_dir}/${module_name}/eos/polymorphic_portable_archive.hpp
  ${module_include_dir}/${module_name}/eos/portable_archive_exception.hpp
  ${module_include_dir}/${module_name}/eos/portable_archive.hpp
  ${module_include_dir}/${module_name}/eos/portable_iarchive.hpp
  ${module_include_dir}/${module_name}/eos/portable_oarchive.hpp
  ${module_include_dir}/${module_name}/event_id.h
  ${module_include_dir}/${module_name}/event_id.ipp
  ${module_include_dir}/${module_name}/exception.h
  ${module_include_dir}/${module_name}/factory.h
  ${module_include_dir}/${module_name}/factory_macros.h
  ${module_include_dir}/${module_name}/handle.h
  ${module_include_dir}/${module_name}/handle_macros.h
  ${module_include_dir}/${module_name}/handle_pool.h
  ${module_include_dir}/${module_name}/i_clear.h
  ${module_include_dir}/${module_name}/i_cloneable.h
  ${module_include_dir}/${module_name}/i_named.h
  ${module_include_dir}/${module_name}/integer_range.h
  ${module_include_dir}/${module_name}/io_factory.h
  ${module_include_dir}/${module_name}/ioutils.h
  ${module_include_dir}/${module_name}/i_predicate.h
  ${module_include_dir}/${module_name}/i_serializable.h
  ${module_include_dir}/${module_name}/i_serializable.ipp
  ${module_include_dir}/${module_name}/i_tree_dump.h
  ${module_include_dir}/${module_name}/library_info.h
  ${module_include_dir}/${module_name}/library_loader.h
  ${module_include_dir}/${module_name}/logger.h
  ${module_include_dir}/${module_name}/memory_streambuf.h
  ${module_include_dir}/${module_name}/multi_properties.h
  ${module_include_dir}/${module_name}/multi_properties.ipp
  ${module_include_dir}/${module_name}/object_configuration_description.h
  ${module_include_dir}/${module_name}/ocd_macros.h
  ${module_include_dir}/${module_name}/ocd_driver.h
  ${module_include_dir}/${module_name}/properties.h
  ${module_include_dir}/${module_name}/properties.ipp
  ${module_include_dir}/${module_name}/range_tools.h
  ${module_include_dir}/${module_name}/real_range.h
  ${module_include_dir}/${module_name}/safe_serial.h
  ${module_include_dir}/${module_name}/serialization_macros.h
  ${module_include_dir}/${module_name}/service_macros.h
  ${module_include_dir}/${module_name}/service_manager.h
  ${module_include_dir}/${module_name}/service_manager-inl.h
  ${module_include_dir}/${module_name}/service_tools.h
  ${module_include_dir}/${module_name}/smart_filename.h
  ${module_include_dir}/${module_name}/smart_ref.h
  ${module_include_dir}/${module_name}/temporary_files.h
  ${module_include_dir}/${module_name}/the_serializable.h
  ${module_include_dir}/${module_name}/the_serializable.ipp
  ${module_include_dir}/${module_name}/things.h
  ${module_include_dir}/${module_name}/things-inl.h
  ${module_include_dir}/${module_name}/things.ipp
  ${module_include_dir}/${module_name}/things_macros.h
  ${module_include_dir}/${module_name}/time_tools.h
  ${module_include_dir}/${module_name}/tracer.h
  ${module_include_dir}/${module_name}/types.h
  ${module_include_dir}/${module_name}/units.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/version_check.h
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/version_id.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/advanced_object.cc
  ${module_source_dir}/DynamicLoader.cc
  ${module_source_dir}/event_id.cc
  ${module_source_dir}/factory.cc
  ${module_source_dir}/i_named.cc
  ${module_source_dir}/integer_range.cc
  ${module_source_dir}/io_factory.cc
  ${module_source_dir}/ioutils.cc
  ${module_source_dir}/i_serializable.cc
  ${module_source_dir}/i_tree_dump.cc
  ${module_source_dir}/library_info.cc
  ${module_source_dir}/library_loader.cc
  ${module_source_dir}/logger.cc
  ${module_source_dir}/multi_properties.cc
  ${module_source_dir}/object_configuration_description.cc
  ${module_source_dir}/ocd_utils.cc
  ${module_source_dir}/ocd_driver.cc
  ${module_source_dir}/properties.cc
  ${module_source_dir}/real_range.cc
  ${module_source_dir}/service_manager.cc
  ${module_source_dir}/service_tools.cc
  ${module_source_dir}/smart_filename.cc
  ${module_source_dir}/temporary_files.cc
  ${module_source_dir}/the_serializable.cc
  ${module_source_dir}/things.cc
  ${module_source_dir}/time_tools.cc
  ${module_source_dir}/tracer.cc
  ${module_source_dir}/types.cc
  ${module_source_dir}/units.cc
  ${module_source_dir}/utils.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/version_check.cc
  ${module_source_dir}/version_id.cc
  ${module_source_dir}/base_service.cc # <- Must go at end
  )

# - Reflection component - still optional, so factor out and allow for
#   inclusion later

if(DATATOOLS_WITH_REFLECTION)
  set(datatools_REFLECTION_HEADERS
    ${module_include_dir}/${module_name}/reflection_guard.h
    ${module_include_dir}/${module_name}/reflection_macros.h
    ${module_include_dir}/${module_name}/i_serializable-reflect.h
    ${module_include_dir}/${module_name}/i_tree_dump-reflect.h
    ${module_include_dir}/${module_name}/logger-reflect.h
    ${module_include_dir}/${module_name}/advanced_object-reflect.h
    ${module_include_dir}/${module_name}/event_id-reflect.h
    ${module_include_dir}/${module_name}/multi_properties-reflect.h
    ${module_include_dir}/${module_name}/properties-reflect.h
    ${module_include_dir}/${module_name}/things-reflect.h
    ${module_include_dir}/${module_name}/the_introspectable.h
    ${module_include_dir}/${module_name}/detail/reflection_link_guard.h
    ${module_include_dir}/${module_name}/detail/reflection_export.h
    ${module_include_dir}/${module_name}/detail/reflection_utils.h
    ${module_include_dir}/${module_name}/detail/reflection_version.h
    )
  set(datatools_REFLECTION_SOURCES
    ${module_source_dir}/the_introspectable.cc
    )
  set(datatools_REFLECTION_TESTS
    ${module_test_dir}/test_reflection_0.cxx
    )
endif()

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "DATATOOLS_DATA_DIR=${module_root_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_advanced_object.cxx
  ${module_test_dir}/test_binary_serialization.cxx
  ${module_test_dir}/test_cloneable_2.cxx
  ${module_test_dir}/test_cloneable.cxx
  ${module_test_dir}/test_data_serialization.cxx
  ${module_test_dir}/test_datatools.cxx
  ${module_test_dir}/test_dummy_service.cxx
  ${module_test_dir}/test_event_id.cxx
  ${module_test_dir}/test_exception.cxx
  ${module_test_dir}/test_factory.cxx
  ${module_test_dir}/test_handle_1.cxx
  ${module_test_dir}/test_handle_2.cxx
  ${module_test_dir}/test_handle_3.cxx
  ${module_test_dir}/test_handle_macros.cxx
  ${module_test_dir}/test_integer_range.cxx
  ${module_test_dir}/test_ioutils.cxx
  ${module_test_dir}/test_library_info.cxx
  ${module_test_dir}/test_library_loader_0.cxx
  ${module_test_dir}/test_library_loader_1.cxx
  ${module_test_dir}/test_logger.cxx
  ${module_test_dir}/test_multi_properties_0.cxx
  ${module_test_dir}/test_multi_properties.cxx
  ${module_test_dir}/test_named.cxx
  ${module_test_dir}/test_nans_ar.cxx
  ${module_test_dir}/test_OCD.cxx
  ${module_test_dir}/test_predicate_1.cxx
  ${module_test_dir}/test_properties_0.cxx
  ${module_test_dir}/test_properties_2b.cxx
  ${module_test_dir}/test_properties_2.cxx
  ${module_test_dir}/test_properties_3.cxx
  ${module_test_dir}/test_properties_4.cxx
  ${module_test_dir}/test_properties.cxx
  ${module_test_dir}/test_real_range.cxx
  ${module_test_dir}/test_ser_bitset.cxx
  ${module_test_dir}/test_serializable_1.cxx
  ${module_test_dir}/test_serializable_2.cxx
  ${module_test_dir}/test_serialization_2.cxx
  ${module_test_dir}/test_serialization_3.cxx
  ${module_test_dir}/test_serialization_4.cxx
  ${module_test_dir}/test_serialization.cxx
  ${module_test_dir}/test_service_manager.cxx
  ${module_test_dir}/test_shared_ptr_0.cxx
  ${module_test_dir}/test_shared_ptr_1.cxx
  ${module_test_dir}/test_smart_filename.cxx
  ${module_test_dir}/test_smart_ref.cxx
  ${module_test_dir}/test_temp_file.cxx
  ${module_test_dir}/test_things_1.cxx
  ${module_test_dir}/test_things_2.cxx
  ${module_test_dir}/test_things_3.cxx
  ${module_test_dir}/test_things.cxx
  ${module_test_dir}/test_things_macros.cxx
  ${module_test_dir}/test_time_tools.cxx
  ${module_test_dir}/test_tmp.cxx
  ${module_test_dir}/test_tracer.cxx
  ${module_test_dir}/test_tree_dump.cxx
  ${module_test_dir}/test_types.cxx
  ${module_test_dir}/test_units.cxx
  ${module_test_dir}/test_utils.cxx
  ${module_test_dir}/test_version_check.cxx
  ${module_test_dir}/test_version.cxx
  ${module_test_dir}/test_version_id.cxx
  )

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/ocd_manual.cxx
  )

