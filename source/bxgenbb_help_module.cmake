# - Module definition for genbb_help submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name genbb_help)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir app_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(genbb_help_VERSION_MAJOR 6)
set(genbb_help_VERSION_MINOR 2)
set(genbb_help_VERSION_PATCH 0)
set(genbb_help_VERSION "${genbb_help_VERSION_MAJOR}.${genbb_help_VERSION_MINOR}.${genbb_help_VERSION_PATCH}")

# - Boost I/O, CAMP Reflection, Gnuplot pipe interface
set(GENBB_HELP_WITH_BIO 1)
set(GENBB_HELP_WITH_REFLECTION 1)

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/genbb_macros.h
  ${module_include_dir}/${module_name}/genbb_mgr.h
  ${module_include_dir}/${module_name}/genbb_utils.h
  ${module_include_dir}/${module_name}/genbb_writer.h
  ${module_include_dir}/${module_name}/i_genbb.h
  ${module_include_dir}/${module_name}/kinematics.h
  ${module_include_dir}/${module_name}/two_body_decay.h
  ${module_include_dir}/${module_name}/manager.h
  ${module_include_dir}/${module_name}/primary_event.h
  ${module_include_dir}/${module_name}/primary_particle.h
  ${module_include_dir}/${module_name}/single_particle_generator.h
  ${module_include_dir}/${module_name}/combined_particle_generator.h
  ${module_include_dir}/${module_name}/time_slicer_generator.h
  ${module_include_dir}/${module_name}/save_to_file_wrapper.h
  ${module_include_dir}/${module_name}/lorentz_boost_wrapper.h
  ${module_include_dir}/${module_name}/from_file_generator.h
  ${module_include_dir}/${module_name}/fermi_function.h
  ${module_include_dir}/${module_name}/nuclear_level.h
  ${module_include_dir}/${module_name}/nuclear_decay.h
  ${module_include_dir}/${module_name}/nuclear_decay_manager.h
  ${module_include_dir}/${module_name}/nuclear_decay_generator.h
  ${module_include_dir}/${module_name}/base_decay_driver.h
  ${module_include_dir}/${module_name}/nuclear_transition.h
  ${module_include_dir}/${module_name}/beta_decay.h
  ${module_include_dir}/${module_name}/alpha_decay.h
  ${module_include_dir}/${module_name}/wdecay0.h
  ${module_include_dir}/${module_name}/pdg_particle_tools.h
  ${module_include_dir}/${module_name}/primary_event.ipp
  ${module_include_dir}/${module_name}/primary_particle.ipp
  ${module_include_dir}/${module_name}/genbb_help_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/detail/pg_tools.h
  ${module_include_dir}/${module_name}/primary_particle-reflect.h
  ${module_include_dir}/${module_name}/primary_event-reflect.h
  ${module_include_dir}/${module_name}/resource.h
  )

include_directories(${BxDecay0_INCLUDE_DIRS})

# - NB Order of sources appears to be important - taken from genbb_help
#   listing. Note that the_serializable.cc is added manually - not
#   totally clear this is in the right place...
set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/genbb_utils.cc
  ${module_source_dir}/genbb_writer.cc
  ${module_source_dir}/kinematics.cc
  ${module_source_dir}/two_body_decay.cc
  ${module_source_dir}/pg_tools.cc
  ${module_source_dir}/i_genbb.cc
  ${module_source_dir}/pdg_particle_tools.cc
  ${module_source_dir}/primary_event.cc
  ${module_source_dir}/primary_particle.cc
  ${module_source_dir}/wdecay0.cc
  ${module_source_dir}/genbb_mgr.cc
  ${module_source_dir}/single_particle_generator.cc
  ${module_source_dir}/combined_particle_generator.cc
  ${module_source_dir}/time_slicer_generator.cc
  ${module_source_dir}/save_to_file_wrapper.cc
  ${module_source_dir}/lorentz_boost_wrapper.cc
  ${module_source_dir}/from_file_generator.cc
  ${module_source_dir}/fermi_function.cc
  ${module_source_dir}/nuclear_level.cc
  ${module_source_dir}/nuclear_decay.cc
  ${module_source_dir}/nuclear_decay_manager.cc
  ${module_source_dir}/nuclear_decay_generator.cc
  ${module_source_dir}/base_decay_driver.cc
  ${module_source_dir}/beta_decay.cc
  ${module_source_dir}/alpha_decay.cc
  ${module_source_dir}/nuclear_transition.cc
  ${module_source_dir}/manager.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/the_serializable.cc
  #${module_source_dir}/the_introspectable.cc
  ${module_source_dir}/resource.cc
  )

# - Publish public headers only
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - add private headers
# list(APPEND ${module_name}_MODULE_HEADERS ${${module_name}_PRIVATE_MODULE_HEADERS})

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "GENBB_HELP_RESOURCE_DIR=${module_resource_dir};GENBB_HELP_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_genbb_help.cxx
  ${module_test_dir}/test_bb0nu_channel_2.cxx
  ${module_test_dir}/test_bb0nu_channel.cxx
  ${module_test_dir}/test_bb0nu_cos_theta.cxx
  ${module_test_dir}/test_genbb_help_serialization.cxx
  ${module_test_dir}/test_genbb_mgr_2.cxx
  ${module_test_dir}/test_genbb_mgr_3.cxx
  ${module_test_dir}/test_genbb_mgr_4.cxx
  ${module_test_dir}/test_genbb_mgr_5.cxx
  ${module_test_dir}/test_genbb_mgr.cxx
  ${module_test_dir}/test_genbb_writer.cxx
  ${module_test_dir}/test_pdg_particle_tools.cxx
  ${module_test_dir}/test_primary_event.cxx
  ${module_test_dir}/test_single_particle_generator_2.cxx
  ${module_test_dir}/test_single_particle_generator_3.cxx
  ${module_test_dir}/test_single_particle_generator_4.cxx
  ${module_test_dir}/test_single_particle_generator_5.cxx
  ${module_test_dir}/test_single_particle_generator.cxx
  ${module_test_dir}/test_lorentz_boost_wrapper.cxx
  ${module_test_dir}/test_from_to_file_generator.cxx
  ${module_test_dir}/test_fermi_function_2.cxx
  ${module_test_dir}/test_nuclear_level.cxx
  ${module_test_dir}/test_nuclear_decay.cxx
  ${module_test_dir}/test_nuclear_transition.cxx
  ${module_test_dir}/test_beta_decay.cxx
  ${module_test_dir}/test_alpha_decay.cxx
  ${module_test_dir}/test_nuclear_decay_manager.cxx
  ${module_test_dir}/test_nuclear_decay_generator.cxx
  ${module_test_dir}/test_wdecay0.cxx
  ${module_test_dir}/test_manager.cxx
  ${module_test_dir}/test_reflection_0.cxx
  )

list(APPEND ${module_name}_MODULE_TESTS ${module_test_dir}/test_wdecay0_2.cxx)

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/genbb_inspector.cxx
  )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/manager/config/pro-1.0/README.rst
  ${module_resource_dir}/manager/config/pro-1.0/misc.conf
  ${module_resource_dir}/manager/config/pro-1.0/calibrations.conf
  ${module_resource_dir}/manager/config/pro-1.0/manager.conf
  ${module_resource_dir}/manager/config/pro-1.0/backgrounds.conf
  ${module_resource_dir}/manager/config/pro-1.0/dbd.conf
  ${module_resource_dir}/inspector/config/le_nuphy-1.0/README.rst
  ${module_resource_dir}/inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf
  ${module_resource_dir}/inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf
  ${module_resource_dir}/generators/Co60/README.rst
  ${module_resource_dir}/generators/Co60/manager.conf
  ${module_resource_dir}/generators/Co60/generators.def
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_levels.def
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_decays.def
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_level_scheme.jpeg
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_decay_scheme.jpeg
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_levels.jpeg
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_levels.def
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_decays.def
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_level_scheme.jpeg
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_levels.jpeg
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

# - Utility script:
if(BAYEUX_WITH_DEVELOPER_TOOLS)
  configure_file(${module_app_dir}/genbb_mkskelcfg.in
    ${BAYEUX_BUILD_BINDIR}/bxgenbb_mkskelcfg @ONLY)

  install(FILES
    ${BAYEUX_BUILD_BINDIR}/bxgenbb_mkskelcfg
    DESTINATION
    ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS
    OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )
endif()
