# - Module definition for datatools submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

#-----------------------------------------------------------------------
# Special settings
set(DATATOOLS_WITH_UNICODE_SYMBOLS 0)
if (Bayeux_WITH_UNICODE_SYMBOLS)
  set(DATATOOLS_WITH_UNICODE_SYMBOLS 1)
endif()
set(DATATOOLS_WITH_ANSI_COLORS 0)
if (Bayeux_WITH_ANSI_COLORS)
  set(DATATOOLS_WITH_ANSI_COLORS 1)
endif()

# - Module
set(module_name datatools)
set(module_root_dir     "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir  "${module_root_dir}/include")
set(module_source_dir   "${module_root_dir}/src")
set(module_test_dir     "${module_root_dir}/testing")
set(module_app_dir      "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir test_dir app_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(datatools_VERSION_MAJOR 7)
set(datatools_VERSION_MINOR 2)
set(datatools_VERSION_PATCH 0)
set(datatools_VERSION "${datatools_VERSION_MAJOR}.${datatools_VERSION_MINOR}.${datatools_VERSION_PATCH}")

# - Readline (for the datatools::ui::basic_shell class)
set(DATATOOLS_WITH_READLINE 0)
find_package(Readline QUIET)
if (Readline_FOUND)
  # message (STATUS "bxdatatools: Found readline library...")
  # message (STATUS "bxdatatools: Readline_INCLUDE_DIR = '${Readline_INCLUDE_DIR}' ")
  # message (STATUS "bxdatatools: Readline_LIBRARIES   = '${Readline_LIBRARIES}' ")
  set(DATATOOLS_WITH_READLINE 1)
else()
  message (STATUS "bxdatatools: Readline library not found!")
endif()

# - CAMP Reflection
set(DATATOOLS_WITH_REFLECTION 1)

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/archives_instantiation.h
  ${module_include_dir}/${module_name}/archives_list.h
  ${module_include_dir}/${module_name}/base_service.h
  ${module_include_dir}/${module_name}/bit_mask.h
  ${module_include_dir}/${module_name}/caster_utils.h
  ${module_include_dir}/${module_name}/clhep_units.h
  ${module_include_dir}/${module_name}/command_utils.h
  ${module_include_dir}/${module_name}/compiler_macros.h
  ${module_include_dir}/${module_name}/datatools_init.h
  ${module_include_dir}/${module_name}/datatools.h
  ${module_include_dir}/${module_name}/datatools_config.h.in
  ${module_include_dir}/${module_name}/detail/api.h
  ${module_include_dir}/${module_name}/detail/Configure.h
  ${module_include_dir}/${module_name}/detail/DynamicLoader.h
  ${module_include_dir}/${module_name}/detail/ocd_utils.h
  ${module_include_dir}/${module_name}/enriched_base.h
  ${module_include_dir}/${module_name}/enriched_base.ipp
  ${module_include_dir}/${module_name}/eos/polymorphic_portable_archive.hpp
  ${module_include_dir}/${module_name}/eos/portable_archive_exception.hpp
  ${module_include_dir}/${module_name}/eos/portable_archive.hpp
  ${module_include_dir}/${module_name}/eos/portable_iarchive.hpp
  ${module_include_dir}/${module_name}/eos/portable_oarchive.hpp
  ${module_include_dir}/${module_name}/event_id.h
  ${module_include_dir}/${module_name}/event_id.ipp
  ${module_include_dir}/${module_name}/exception.h
  ${module_include_dir}/${module_name}/factory.h
  ${module_include_dir}/${module_name}/factory-inl.h
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
 #${module_include_dir}/${module_name}/iocolor.h
  ${module_include_dir}/${module_name}/i_predicate.h
  ${module_include_dir}/${module_name}/i_serializable.h
  ${module_include_dir}/${module_name}/i_serializable.ipp
  ${module_include_dir}/${module_name}/i_tree_dump.h
  ${module_include_dir}/${module_name}/kernel.h
  ${module_include_dir}/${module_name}/library_info.h
  ${module_include_dir}/${module_name}/library_query_service.h
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
  ${module_include_dir}/${module_name}/reflection_interface.h
  ${module_include_dir}/${module_name}/resource.h
  ${module_include_dir}/${module_name}/safe_serial.h
  ${module_include_dir}/${module_name}/serialization_macros.h
  ${module_include_dir}/${module_name}/service_manager.h
  ${module_include_dir}/${module_name}/service_manager-inl.h
  ${module_include_dir}/${module_name}/service_tools.h
  ${module_include_dir}/${module_name}/service_tools-inl.h
  ${module_include_dir}/${module_name}/smart_filename.h
  ${module_include_dir}/${module_name}/smart_ref.h
  ${module_include_dir}/${module_name}/temporary_files.h
  ${module_include_dir}/${module_name}/things.h
  ${module_include_dir}/${module_name}/things.ipp
  ${module_include_dir}/${module_name}/things-inl.h
  ${module_include_dir}/${module_name}/things_macros.h
  ${module_include_dir}/${module_name}/time_tools.h
  ${module_include_dir}/${module_name}/tracer.h
  ${module_include_dir}/${module_name}/types.h
  ${module_include_dir}/${module_name}/units.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/version_check.h
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/version_id.h
  ${module_include_dir}/${module_name}/version_id.ipp
  ${module_include_dir}/${module_name}/urn.h
  ${module_include_dir}/${module_name}/urn.ipp
  ${module_include_dir}/${module_name}/urn_info.h
  ${module_include_dir}/${module_name}/urn_info.ipp
  ${module_include_dir}/${module_name}/urn_db_service.h
  ${module_include_dir}/${module_name}/urn_to_path.h
  ${module_include_dir}/${module_name}/urn_to_path.ipp
  ${module_include_dir}/${module_name}/urn_to_path_resolver_service.h
  ${module_include_dir}/${module_name}/urn_query_service.h
  # ${module_include_dir}/${module_name}/url.h
  # ${module_include_dir}/${module_name}/url.ipp
  # ${module_include_dir}/${module_name}/url_parsers-inl.h
  # ${module_include_dir}/${module_name}/log_record.h
  # ${module_include_dir}/${module_name}/log_record.ipp
  # ${module_include_dir}/${module_name}/log_format.h
  # ${module_include_dir}/${module_name}/rst_tools.h

  ${module_include_dir}/${module_name}/configuration/i_occurrence.h
  ${module_include_dir}/${module_name}/configuration/single_occurrence.h
  ${module_include_dir}/${module_name}/configuration/array_occurrence.h
  ${module_include_dir}/${module_name}/configuration/occurrence_factory.h
  ${module_include_dir}/${module_name}/configuration/io.h
  ${module_include_dir}/${module_name}/configuration/parameter_model.h
  ${module_include_dir}/${module_name}/configuration/parameter_physical.h
  ${module_include_dir}/${module_name}/configuration/utils.h
  ${module_include_dir}/${module_name}/configuration/variant_registry_manager.h
  ${module_include_dir}/${module_name}/configuration/variant_model.h
  ${module_include_dir}/${module_name}/configuration/variant_physical.h
  ${module_include_dir}/${module_name}/configuration/variant_record.h
  ${module_include_dir}/${module_name}/configuration/variant_registry.h
  ${module_include_dir}/${module_name}/configuration/variant_repository.h
  ${module_include_dir}/${module_name}/configuration/variant_service.h
  ${module_include_dir}/${module_name}/configuration/variant_dependency_utils.h
  ${module_include_dir}/${module_name}/configuration/variant_dependency_logic_parsing.h
  ${module_include_dir}/${module_name}/configuration/variant_dependency_logic_builder.h
  ${module_include_dir}/${module_name}/configuration/variant_dependency_logic.h
  ${module_include_dir}/${module_name}/configuration/variant_dependency.h
  ${module_include_dir}/${module_name}/configuration/variant_dependency_model.h
  ${module_include_dir}/${module_name}/configuration/variant_object_info.h
  ${module_include_dir}/${module_name}/configuration/ui/variant_registry_cli.h
  ${module_include_dir}/${module_name}/configuration/ui/variant_repository_cli.h
  ${module_include_dir}/${module_name}/configuration/parsers.h

  ${module_include_dir}/${module_name}/introspection/data_type.h
  ${module_include_dir}/${module_name}/introspection/unit_support.h
  ${module_include_dir}/${module_name}/introspection/data_layout.h
  ${module_include_dir}/${module_name}/introspection/access_type.h
  ${module_include_dir}/${module_name}/introspection/data_description.h
  ${module_include_dir}/${module_name}/introspection/argument.h
  ${module_include_dir}/${module_name}/introspection/method.h
  ${module_include_dir}/${module_name}/introspection/builder.h

  ${module_include_dir}/${module_name}/ui/ansi_colors.h
  ${module_include_dir}/${module_name}/ui/special_symbols.h
  ${module_include_dir}/${module_name}/ui/traits.h
  ${module_include_dir}/${module_name}/ui/utils.h
  ${module_include_dir}/${module_name}/ui/ihs.h
  ${module_include_dir}/${module_name}/ui/base_command.h
  ${module_include_dir}/${module_name}/ui/target_command.h
  ${module_include_dir}/${module_name}/ui/reflective_command.h
  ${module_include_dir}/${module_name}/ui/reflective_command-inl.h
  ${module_include_dir}/${module_name}/ui/base_command_interface.h
  ${module_include_dir}/${module_name}/ui/target_command_interface.h
  ${module_include_dir}/${module_name}/ui/reflective_command_interface.h
  ${module_include_dir}/${module_name}/ui/reflective_command_interface-inl.h
  ${module_include_dir}/${module_name}/ui/basic_shell.h
  ${module_include_dir}/${module_name}/ui/shell_commands.h
  ${module_include_dir}/${module_name}/ui/shell_command_interface.h
  ${module_include_dir}/${module_name}/ui/shell_exit_command.h
  ${module_include_dir}/${module_name}/ui/shell_pwd_command.h
  ${module_include_dir}/${module_name}/ui/shell_ls_command.h
  ${module_include_dir}/${module_name}/ui/shell_cd_command.h
  ${module_include_dir}/${module_name}/ui/shell_man_command.h
  ${module_include_dir}/${module_name}/ui/shell_tree_command.h
  ${module_include_dir}/${module_name}/ui/shell_load_command.h
  ${module_include_dir}/${module_name}/ui/shell_help_command.h
  ${module_include_dir}/${module_name}/ui/wrappers.h
  ${module_include_dir}/${module_name}/ui/validators.h

  ${module_include_dir}/${module_name}/i_serializable-reflect.h
  ${module_include_dir}/${module_name}/i_tree_dump-reflect.h
  ${module_include_dir}/${module_name}/logger-reflect.h
  ${module_include_dir}/${module_name}/command_utils-reflect.h
  ${module_include_dir}/${module_name}/enriched_base-reflect.h
  ${module_include_dir}/${module_name}/base_service-reflect.h
  ${module_include_dir}/${module_name}/event_id-reflect.h
  ${module_include_dir}/${module_name}/multi_properties-reflect.h
  ${module_include_dir}/${module_name}/properties-reflect.h
  ${module_include_dir}/${module_name}/things-reflect.h
  ${module_include_dir}/${module_name}/units-reflect.h

  ## ${module_include_dir}/${module_name}/observer_design_pattern.h
  ${module_include_dir}/${module_name}/dependency_graph.h

  ${module_include_dir}/${module_name}/introspection/data_type-reflect.h
  ${module_include_dir}/${module_name}/introspection/unit_support-reflect.h
  ${module_include_dir}/${module_name}/introspection/access_type-reflect.h
  ${module_include_dir}/${module_name}/introspection/data_layout-reflect.h
  ${module_include_dir}/${module_name}/introspection/argument.h
  ${module_include_dir}/${module_name}/introspection/method.h

  ${module_include_dir}/${module_name}/detail/reflection_export.h
  ${module_include_dir}/${module_name}/detail/reflection_utils.h
  ${module_include_dir}/${module_name}/detail/reflection_version.h
  ${module_include_dir}/${module_name}/detail/reflection_macros.h
  ${module_include_dir}/${module_name}/detail/command_macros.h
  ${module_include_dir}/${module_name}/detail/logger_macros.h
)

# # - configure special source file
# configure_file(${module_source_dir}/_datatools.cc.in bx${module_name}/_datatools.cc)

set(${module_name}_MODULE_SOURCES
${module_source_dir}/base_service.cc
${module_source_dir}/bit_mask.cc
${module_source_dir}/command_utils.cc
${module_source_dir}/datatools.cc
${module_source_dir}/DynamicLoader.cc
${module_source_dir}/enriched_base.cc
${module_source_dir}/event_id.cc
${module_source_dir}/factory.cc
${module_source_dir}/i_clear.cc
${module_source_dir}/i_named.cc
${module_source_dir}/integer_range.cc
${module_source_dir}/io_factory.cc
${module_source_dir}/ioutils.cc
#${module_source_dir}/iocolor.cc
${module_source_dir}/i_serializable.cc
${module_source_dir}/i_tree_dump.cc
${module_source_dir}/kernel.cc
${module_source_dir}/library_info.cc
${module_source_dir}/library_query_service.cc
${module_source_dir}/library_loader.cc
${module_source_dir}/logger.cc
${module_source_dir}/memory_streambuf.cc
${module_source_dir}/multi_properties.cc
${module_source_dir}/object_configuration_description.cc
${module_source_dir}/ocd_driver.cc
${module_source_dir}/ocd_utils.cc
${module_source_dir}/properties.cc
${module_source_dir}/range_tools.cc
${module_source_dir}/real_range.cc
${module_source_dir}/resource.cc
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
${module_source_dir}/urn.cc
${module_source_dir}/urn_info.cc
${module_source_dir}/urn_db_service.cc
${module_source_dir}/urn_to_path.cc
${module_source_dir}/urn_to_path_resolver_service.cc
${module_source_dir}/urn_query_service.cc
${module_source_dir}/dependency_graph.cc
# ${module_source_dir}/url.cc
# ${module_source_dir}/log_record.cc
# ${module_source_dir}/log_format.cc
# ${module_source_dir}/rst_tools.cc

${module_source_dir}/configuration/i_occurrence.cc
${module_source_dir}/configuration/single_occurrence.cc
${module_source_dir}/configuration/array_occurrence.cc
${module_source_dir}/configuration/occurrence_factory.cc
${module_source_dir}/configuration/io.cc
${module_source_dir}/configuration/parameter_model.cc
${module_source_dir}/configuration/parameter_physical.cc
${module_source_dir}/configuration/utils.cc
${module_source_dir}/configuration/variant_registry_manager.cc
${module_source_dir}/configuration/variant_model.cc
${module_source_dir}/configuration/variant_physical.cc
${module_source_dir}/configuration/variant_record.cc
${module_source_dir}/configuration/variant_registry.cc
${module_source_dir}/configuration/variant_repository.cc
${module_source_dir}/configuration/variant_service.cc
${module_source_dir}/configuration/variant_object_info.cc
${module_source_dir}/configuration/variant_dependency_logic.cc
${module_source_dir}/configuration/variant_dependency_logic_parsing.cc
${module_source_dir}/configuration/variant_dependency_logic_builder.cc
${module_source_dir}/configuration/variant_dependency.cc
${module_source_dir}/configuration/variant_dependency_model.cc
${module_source_dir}/configuration/ui/variant_registry_cli.cc
${module_source_dir}/configuration/ui/variant_repository_cli.cc

${module_source_dir}/introspection/data_type.cc
${module_source_dir}/introspection/unit_support.cc
${module_source_dir}/introspection/access_type.cc
${module_source_dir}/introspection/data_layout.cc
${module_source_dir}/introspection/data_description.cc
${module_source_dir}/introspection/argument.cc
${module_source_dir}/introspection/method.cc
${module_source_dir}/introspection/builder.cc

${module_source_dir}/ui/basic_shell.cc
${module_source_dir}/ui/shell_command_interface.cc
${module_source_dir}/ui/shell_exit_command.cc
${module_source_dir}/ui/shell_pwd_command.cc
${module_source_dir}/ui/shell_ls_command.cc
${module_source_dir}/ui/shell_cd_command.cc
${module_source_dir}/ui/shell_man_command.cc
${module_source_dir}/ui/shell_tree_command.cc
${module_source_dir}/ui/shell_load_command.cc
${module_source_dir}/ui/shell_help_command.cc
${module_source_dir}/ui/ihs.cc
${module_source_dir}/ui/base_command_interface.cc
${module_source_dir}/ui/base_command.cc
${module_source_dir}/ui/traits.cc
${module_source_dir}/ui/utils.cc
${module_source_dir}/ui/validators.cc
${module_source_dir}/ui/wrappers.cc
${module_source_dir}/ui/ansi_colors.cc
${module_source_dir}/ui/special_symbols.cc
)

if(DATATOOLS_WITH_UNICODE_SYMBOLS)
  list(APPEND
    ${module_name}_MODULE_HEADERS
    ${module_include_dir}/${module_name}/ui/unicode_symbols.h
    )
  list(APPEND
    ${module_name}_MODULE_SOURCES
    ${module_source_dir}/ui/unicode_symbols.cc
    )
endif()

set(DATATOOLS_WITH_QT_GUI 0)
if (BAYEUX_WITH_QT_GUI)
  set(DATATOOLS_WITH_QT_GUI 1)
  # - QT moc headers
  set(${module_name}_MODULE_HEADERS_QT_TO_BE_MOCCED
    ${module_include_dir}/${module_name}/qt/led.h
    ${module_include_dir}/${module_name}/configuration/ui/parameter_item_delegate.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_registry_dialog.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_registry_tree_model.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_registry_viewer.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_repository_dialog.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_repository_viewer.h
    )
  # message(STATUS "To be mocced = '${${module_name}_MODULE_HEADERS_QT_TO_BE_MOCCED}'")

  # Qt MOC bug on SL 5.3
  # Some specific code from Boost is broken while compiled by Qt MOC. It seems possible
  # to use some guards to workaround this issue:
  # See https://bugreports.qt-project.org/browse/QTBUG-22829
  # #ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
  # #include <boost/lexical_cast.hpp>
  # #endif

  QT5_WRAP_CPP(${module_name}_MODULE_HEADERS_QT_MOC
    ${${module_name}_MODULE_HEADERS_QT_TO_BE_MOCCED}
    # OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED ### This option does not work
    )
  # message(STATUS "Mocced = '${${module_name}_MODULE_HEADERS_QT_MOC}'")
  list(APPEND ${module_name}_MODULE_SOURCES
    ${module_source_dir}/qt/led.cc
    ${module_source_dir}/qt/interface.cc
    ${module_source_dir}/configuration/ui/parameter_item_delegate.cc
    ${module_source_dir}/configuration/ui/variant_registry_dialog.cc
    ${module_source_dir}/configuration/ui/variant_registry_tree_model.cc
    ${module_source_dir}/configuration/ui/variant_registry_viewer.cc
    ${module_source_dir}/configuration/ui/variant_repository_dialog.cc
    ${module_source_dir}/configuration/ui/variant_repository_viewer.cc
    ${${module_name}_MODULE_HEADERS_QT_MOC}
    )
  list(APPEND ${module_name}_MODULE_HEADERS
    ${module_include_dir}/${module_name}/qt/led.h
    ${module_include_dir}/${module_name}/qt/interface.h
    ${module_include_dir}/${module_name}/configuration/ui/parameter_item_delegate.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_registry_dialog.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_registry_tree_model.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_registry_viewer.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_repository_dialog.h
    ${module_include_dir}/${module_name}/configuration/ui/variant_repository_viewer.h
    )
endif()

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "DATATOOLS_RESOURCE_DIR=${module_resource_dir};DATATOOLS_TESTING_DIR=${module_test_dir}")

# ls -1 bxdatatools/testing/*.cxx | sed -e 's@bxdatatools/testing@${module_test_dir}@g'

set(${module_name}_MODULE_TESTS
${module_test_dir}/test_reflection_0.cxx
${module_test_dir}/test_enriched_base.cxx
${module_test_dir}/test_binary_serialization.cxx
${module_test_dir}/test_cloneable_2.cxx
${module_test_dir}/test_cloneable.cxx
${module_test_dir}/test_data_serialization.cxx
${module_test_dir}/test_dummy_service.cxx
${module_test_dir}/test_exception.cxx
${module_test_dir}/test_factory.cxx
${module_test_dir}/test_handle_1.cxx
${module_test_dir}/test_handle_2.cxx
${module_test_dir}/test_handle_3.cxx
${module_test_dir}/test_handle_macros.cxx
${module_test_dir}/test_handle_operators.cxx
${module_test_dir}/test_integer_range.cxx
${module_test_dir}/test_ioutils.cxx
${module_test_dir}/test_kernel.cxx
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
${module_test_dir}/test_urn.cxx
${module_test_dir}/test_urn_info.cxx
${module_test_dir}/test_urn_db_service.cxx
${module_test_dir}/test_urn_db_service_2.cxx
${module_test_dir}/test_urn_to_path.cxx
${module_test_dir}/test_urn_to_path_resolver_service.cxx
${module_test_dir}/test_urn_query_service.cxx
${module_test_dir}/test_dependency_graph.cxx
# ${module_test_dir}/test_url.cxx
# ${module_test_dir}/test_log_record.cxx
# ${module_test_dir}/test_rst_tools.cxx
${module_test_dir}/test_configuration_parameter_model.cxx
${module_test_dir}/test_configuration_variant_model.cxx
${module_test_dir}/test_configuration_variant_api_0.cxx
${module_test_dir}/test_configuration_variant_service.cxx
${module_test_dir}/test_configuration_variant_service_2.cxx
${module_test_dir}/test_configuration_variant_dependency.cxx
${module_test_dir}/test_configuration_parsers.cxx
${module_test_dir}/test_i_tree_dump.cxx
)

# Catastrophically broken on Mac
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  list(APPEND ${module_name}_MODULE_TESTS
    ${module_test_dir}/test_ui_ansi_colors.cxx
    ${module_test_dir}/test_ui_utils.cxx
    ${module_test_dir}/test_ui_ihs.cxx
    ${module_test_dir}/test_ui_base_command.cxx
    ${module_test_dir}/test_ui_reflective_command.cxx
    ${module_test_dir}/test_ui_base_command_interface.cxx
    ${module_test_dir}/test_ui_shell_command_interface.cxx
    ${module_test_dir}/test_ui_basic_shell.cxx
    ${module_test_dir}/test_ui_regex.cxx
    ${module_test_dir}/test_ui_validators.cxx
    ${module_test_dir}/test_backward_things.cxx
    ${module_test_dir}/test_introspection_data_description.cxx
    ${module_test_dir}/test_introspection_argument.cxx
    ${module_test_dir}/test_introspection_method.cxx
    ${module_test_dir}/test_event_id.cxx
    )
endif()

# List new Catch based tests in this one
set(${module_name}_MODULE_TESTS_CATCH
  ${module_test_dir}/test_datatools.cxx
  )

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/ocd_manual.cxx
  )

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/OCD/pandoc/templates/OCD2DoxygenTemplate.html
  ${module_resource_dir}/variants/models/base_variants.def
  ${module_resource_dir}/variants/models/basic/1.0/utils.def
  )

list(APPEND ${module_name}_MODULE_APPS
  ${module_app_dir}/variant_inspector.cxx
  )

if (BAYEUX_WITH_QT_GUI)

  # - Special test program(s)
  list(APPEND ${module_name}_MODULE_TESTS
   ${module_test_dir}/test_qt_led.cxx
  )

  # - Special resource files
  list(APPEND ${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/qt/led/circle_black.svg
  ${module_resource_dir}/qt/led/circle_blue.svg
  ${module_resource_dir}/qt/led/circle_green.svg
  ${module_resource_dir}/qt/led/circle_grey.svg
  ${module_resource_dir}/qt/led/circle_orange.svg
  ${module_resource_dir}/qt/led/circle_purple.svg
  ${module_resource_dir}/qt/led/circle_red.svg
  ${module_resource_dir}/qt/led/circle_yellow.svg
  ${module_resource_dir}/qt/led/led_triangular_1_green.svg
  ${module_resource_dir}/qt/led/led_triangular_1_grey.svg
  ${module_resource_dir}/qt/led/led_triangular_1_red.svg
  ${module_resource_dir}/qt/led/rect_green.svg
  ${module_resource_dir}/qt/led/rect_grey.svg
  ${module_resource_dir}/qt/led/round_blue.svg
  ${module_resource_dir}/qt/led/round_green.svg
  ${module_resource_dir}/qt/led/round_grey.svg
  ${module_resource_dir}/qt/led/round_orange.svg
  ${module_resource_dir}/qt/led/round_purple.svg
  ${module_resource_dir}/qt/led/round_red.svg
  ${module_resource_dir}/qt/led/round_yellow.svg
  ${module_resource_dir}/qt/led/square_blue.svg
  ${module_resource_dir}/qt/led/square_green.svg
  ${module_resource_dir}/qt/led/square_grey.svg
  ${module_resource_dir}/qt/led/square_orange.svg
  ${module_resource_dir}/qt/led/square_purple.svg
  ${module_resource_dir}/qt/led/square_red.svg
  ${module_resource_dir}/qt/led/square_yellow.svg
  ${module_resource_dir}/qt/led/triang_blue.svg
  ${module_resource_dir}/qt/led/triang_green.svg
  ${module_resource_dir}/qt/led/triang_grey.svg
  ${module_resource_dir}/qt/led/triang_orange.svg
  ${module_resource_dir}/qt/led/triang_purple.svg
  ${module_resource_dir}/qt/led/triang_red.svg
  ${module_resource_dir}/qt/led/triang_yellow.svg
  )
endif()

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

# - Utility script:
if(BAYEUX_WITH_DEVELOPER_TOOLS)
  configure_file(${module_app_dir}/ocd_make_doc
    ${BAYEUX_BUILD_BINDIR}/bxocd_make_doc @ONLY)
  configure_file(${module_app_dir}/ocd_sort_classnames.py
    ${BAYEUX_BUILD_BINDIR}/bxocd_sort_classnames.py @ONLY)
  configure_file(${module_app_dir}/extract_table_of_objects
    ${BAYEUX_BUILD_BINDIR}/bxextract_table_of_objects @ONLY)

  install(FILES
    ${BAYEUX_BUILD_BINDIR}/bxocd_make_doc
    ${BAYEUX_BUILD_BINDIR}/bxocd_sort_classnames.py
    ${BAYEUX_BUILD_BINDIR}/bxextract_table_of_objects
    DESTINATION
    ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS
    OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT  Development
    )
endif()
