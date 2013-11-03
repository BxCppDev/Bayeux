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
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(mctools_VERSION_MAJOR 1)
set(mctools_VERSION_MINOR 0)
set(mctools_VERSION_PATCH 0)
set(mctools_VERSION "${mctools_VERSION_MAJOR}.${mctools_VERSION_MINOR}.${mctools_VERSION_PATCH}")

# - Boost I/O, CAMP Reflection
set(MCTOOLS_WITH_BIO 1)
set(MCTOOLS_WITH_REFLECTION 1)

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/base_step_hit.h
  ${module_include_dir}/${module_name}/simulated_data.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/base_step_hit_processor.h
  ${module_include_dir}/${module_name}/step_hit_processor_factory.h
  ${module_include_dir}/${module_name}/calorimeter_step_hit_processor.h
  ${module_include_dir}/${module_name}/simulated_data_reader.h
  ${module_include_dir}/${module_name}/simulated_data_input_module.h
  ${module_include_dir}/${module_name}/base_step_hit.ipp
  ${module_include_dir}/${module_name}/simulated_data.ipp
  ${module_include_dir}/${module_name}/the_serializable.ipp
  ${module_include_dir}/${module_name}/the_serializable.h
  ${module_include_dir}/${module_name}/bio_guard.h
  ${module_include_dir}/${module_name}/base_step_hit-reflect.h
  ${module_include_dir}/${module_name}/simulated_data-reflect.h
  ${module_include_dir}/${module_name}/the_introspectable.h
  ${module_include_dir}/${module_name}/reflection_guard.h
  ${module_include_dir}/${module_name}/mctools_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/detail/bio_link_guard.h
  ${module_include_dir}/${module_name}/detail/reflection_link_guard.h
  ${module_include_dir}/${module_name}/detail/step_hit_processor_macros.h
  ${module_include_dir}/${module_name}/mctools.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/base_step_hit.cc
  ${module_source_dir}/base_step_hit_processor.cc
  ${module_source_dir}/step_hit_processor_factory.cc
  ${module_source_dir}/calorimeter_step_hit_processor.cc
  ${module_source_dir}/utils.cc
  ${module_source_dir}/simulated_data.cc
  ${module_source_dir}/simulated_data_reader.cc
  ${module_source_dir}/simulated_data_input_module.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/the_serializable.cc
  ${module_source_dir}/mctools.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout
"${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()


# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "MCTOOLS_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_base_step_hit.cxx
  ${module_test_dir}/test_simulated_data_1.cxx
  ${module_test_dir}/test_simulated_data_reader_1.cxx
  ${module_test_dir}/test_step_hit_processor_factory.cxx
  ${module_test_dir}/test_simulated_data_input_module_1.cxx
  ${module_test_dir}/test_simulated_data_input_module_2.cxx
  )

#-----------------------------------------------------------------------
# The Geant4 Extension Headers and Sources
# - Slight hack, only list the manager and manager_parameters as
# headers, put the rest in SOURCES. Idea is to only publish public
# headers... Though these two headers are the only real public interface,
# they #include other headers, so these also need to be present...
#
if(Bayeux_WITH_GEANT4)
  set(${module_name}_GEANT4_HEADERS
    ${module_include_dir}/${module_name}/g4/manager_parameters.h
    ${module_include_dir}/${module_name}/g4/manager.h
    ${module_include_dir}/${module_name}/g4/loggable_support.h
    ${module_include_dir}/${module_name}/g4/g4_prng.h
    ${module_include_dir}/${module_name}/g4/track_history.h
    )
  # - Published headers
  foreach(_hdrin ${${module_name}_GEANT4_HEADERS})
    string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
    string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
    configure_file(${_hdrin} ${_hdrout} @ONLY)
  endforeach()

  # - Configurable private headers
  configure_file(${module_include_dir}/${module_name}/g4/data_libraries.h.in
    mctools/g4/data_libraries.h
    @ONLY
    )

  set(${module_name}_GEANT4_SOURCES
    mctools/g4/data_libraries.h
    ${module_include_dir}/${module_name}/g4/simulation_module.h
    ${module_include_dir}/${module_name}/g4/primary_generator.h
    ${module_include_dir}/${module_name}/g4/magnetic_field.h
    ${module_include_dir}/${module_name}/g4/particles_physics_constructor.h
    ${module_include_dir}/${module_name}/g4/physics_list_utils.h
    ${module_include_dir}/${module_name}/g4/tracking_action.h
    ${module_include_dir}/${module_name}/g4/stacking_action.h
    ${module_include_dir}/${module_name}/g4/run_action.h
    ${module_include_dir}/${module_name}/g4/sensitive_detector.h
    ${module_include_dir}/${module_name}/g4/simulation_ctrl.h
    ${module_include_dir}/${module_name}/g4/em_physics_constructor.h
    ${module_include_dir}/${module_name}/g4/sensitive_hit.h
    ${module_include_dir}/${module_name}/g4/physics_list.h
    ${module_include_dir}/${module_name}/g4/event_action.h
    ${module_include_dir}/${module_name}/g4/stepping_verbose.h
    ${module_include_dir}/${module_name}/g4/base_physics_constructor.h
    ${module_include_dir}/${module_name}/g4/detector_construction.h
    ${module_include_dir}/${module_name}/g4/stepping_action.h
    ${module_include_dir}/${module_name}/g4/sensitive_hit_collection.h

    ${module_source_dir}/g4/manager_parameters.cc
    ${module_source_dir}/g4/particles_physics_constructor.cc
    ${module_source_dir}/g4/physics_list_utils.cc
    ${module_source_dir}/g4/tracking_action.cc
    ${module_source_dir}/g4/stacking_action.cc
    ${module_source_dir}/g4/g4_prng.cc
    ${module_source_dir}/g4/run_action.cc
    ${module_source_dir}/g4/sensitive_detector.cc
    ${module_source_dir}/g4/simulation_ctrl.cc
    ${module_source_dir}/g4/em_physics_constructor.cc
    ${module_source_dir}/g4/loggable_support.cc
    ${module_source_dir}/g4/manager.cc
    ${module_source_dir}/g4/physics_list.cc
    ${module_source_dir}/g4/sensitive_hit.cc
    ${module_source_dir}/g4/stepping_verbose.cc
    ${module_source_dir}/g4/event_action.cc
    ${module_source_dir}/g4/base_physics_constructor.cc
    ${module_source_dir}/g4/detector_construction.cc
    ${module_source_dir}/g4/track_history.cc
    ${module_source_dir}/g4/stepping_action.cc
    ${module_source_dir}/g4/sensitive_hit_collection.cc
    ${module_source_dir}/g4/primary_generator.cc
    ${module_source_dir}/g4/simulation_module.cc
    ${module_source_dir}/g4/magnetic_field.cc
    )

    # - Applications
    set(${module_name}_GEANT4_MODULE_APPS
      ${module_app_dir}/g4/g4_production.cxx
      )

endif()

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )