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
set(mctools_VERSION_MAJOR 2)
set(mctools_VERSION_MINOR 2)
set(mctools_VERSION_PATCH 0)
set(mctools_VERSION "${mctools_VERSION_MAJOR}.${mctools_VERSION_MINOR}.${mctools_VERSION_PATCH}")

# - Boost I/O, CAMP Reflection
set(MCTOOLS_WITH_BIO 1)
set(MCTOOLS_WITH_REFLECTION 1)
set(MCTOOLS_WITH_G4 0)
if(Bayeux_WITH_GEANT4_MODULE)
  set(MCTOOLS_WITH_G4 1)
  option(MCTOOLS_WITH_G4_G4VIS_USE "Build the Geant4 visualization interface" ON)
  option(MCTOOLS_WITH_G4_MT "Build the Geant4 MT support" ON)
endif()

if(MCTOOLS_WITH_G4)
  set(G4VIS_NONE)
  if(MCTOOLS_WITH_G4_G4VIS_USE)
    unset(G4VIS_NONE)
    # set(G4VIS_USE 1)
    # if (Geant4_qt_FOUND)
    #   set(G4VIS_USE_OPENGLQT 1)
    # endif()
    # if (Geant4_vis_opengl_x11_FOUND)
    #   set(G4VIS_USE_OPENGLX 1)
    #   set(G4VIS_USE_OPENGLXM 1)
    # endif()
  endif()
endif()

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/base_step_hit.h
  ${module_include_dir}/${module_name}/simulated_data.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/base_step_hit_processor.h
  ${module_include_dir}/${module_name}/step_hit_processor_factory.h
  ${module_include_dir}/${module_name}/calorimeter_step_hit_processor.h
  ${module_include_dir}/${module_name}/fluence_step_hit_processor.h
  ${module_include_dir}/${module_name}/simulated_data_reader.h
  ${module_include_dir}/${module_name}/simulated_data_input_module.h
  ${module_include_dir}/${module_name}/base_step_hit.ipp
  ${module_include_dir}/${module_name}/simulated_data.ipp
  ${module_include_dir}/${module_name}/base_step_hit-reflect.h
  ${module_include_dir}/${module_name}/simulated_data-reflect.h
  ${module_include_dir}/${module_name}/mctools_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/detail/step_hit_processor_macros.h
  ${module_include_dir}/${module_name}/biasing/point_of_interest.h
  ${module_include_dir}/${module_name}/biasing/primary_event_bias.h
  ${module_include_dir}/${module_name}/signal/utils.h
  ${module_include_dir}/${module_name}/signal/base_signal.h
  ${module_include_dir}/${module_name}/signal/base_signal.ipp
  ${module_include_dir}/${module_name}/signal/signal_shape_builder.h
  ${module_include_dir}/${module_name}/signal/multi_signal_shape.h
  ${module_include_dir}/${module_name}/signal/triangle_signal_shape.h
  ${module_include_dir}/${module_name}/signal/triangle_gate_signal_shape.h
  ${module_include_dir}/${module_name}/signal/signal_data.h
  ${module_include_dir}/${module_name}/signal/signal_data.ipp
  ${module_include_dir}/${module_name}/digitization/i_adc.h
  ${module_include_dir}/${module_name}/digitization/simple_linear_adc.h
  ${module_include_dir}/${module_name}/digitization/sampled_signal.h
  ${module_include_dir}/${module_name}/digitization/sampled_signal.ipp
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/base_step_hit.cc
  ${module_source_dir}/base_step_hit_processor.cc
  ${module_source_dir}/step_hit_processor_factory.cc
  ${module_source_dir}/calorimeter_step_hit_processor.cc
  ${module_source_dir}/fluence_step_hit_processor.cc
  ${module_source_dir}/utils.cc
  ${module_source_dir}/simulated_data.cc
  ${module_source_dir}/simulated_data_reader.cc
  ${module_source_dir}/simulated_data_input_module.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/the_serializable.cc
  ${module_source_dir}/biasing/point_of_interest.cc
  ${module_source_dir}/biasing/primary_event_bias.cc
  ${module_source_dir}/signal/utils.cc
  ${module_source_dir}/signal/base_signal.cc
  ${module_source_dir}/signal/signal_shape_builder.cc
  ${module_source_dir}/signal/multi_signal_shape.cc
  ${module_source_dir}/signal/triangle_signal_shape.cc
  ${module_source_dir}/signal/triangle_gate_signal_shape.cc
  ${module_source_dir}/signal/signal_data.cc
  ${module_source_dir}/digitization/simple_linear_adc.cc
  ${module_source_dir}/digitization/sampled_signal.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout
    "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()


# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "MATERIALS_RESOURCE_DIR=${bxmaterials_resource_dir};MCTOOLS_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_base_step_hit.cxx
  ${module_test_dir}/test_simulated_data_1.cxx
  ${module_test_dir}/test_simulated_data_reader_1.cxx
  ${module_test_dir}/test_step_hit_processor_factory.cxx
  ${module_test_dir}/test_simulated_data_input_module_1.cxx
  ${module_test_dir}/test_simulated_data_input_module_2.cxx
  ${module_test_dir}/test_biasing_primary_event_bias.cxx
  ${module_test_dir}/test_signal_triangle_signal_shape.cxx
  ${module_test_dir}/test_signal_triangle_gate_signal_shape.cxx
  ${module_test_dir}/test_signal_base_signal.cxx
  ${module_test_dir}/test_signal_shape_builder.cxx
  ${module_test_dir}/test_signal_signal_data.cxx
  ${module_test_dir}/test_digitization_simple_linear_adc.cxx
  ${module_test_dir}/test_digitization_sampled_signal.cxx
  )

#-----------------------------------------------------------------------
# - Build/Link the geant4 extension module, if required
#
include(${CMAKE_CURRENT_SOURCE_DIR}/bxmctools_geant4_module.cmake)

#-----------------------------------------------------------------------
# - Build/Link the MCNP extension module, if required
#
#include(${CMAKE_CURRENT_SOURCE_DIR}/bxmctools_mcnp_module.cmake)

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )

if(BAYEUX_WITH_DEVELOPER_TOOLS AND Bayeux_WITH_GEANT4_MODULE)
  # - Utility script:
  configure_file(${module_app_dir}/g4/mctools_g4_mkskelcfg.in
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bxmctools_g4_mkskelcfg @ONLY)

  install(FILES
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bxmctools_g4_mkskelcfg
    DESTINATION
    ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS
    OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

endif()
