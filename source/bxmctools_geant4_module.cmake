
#-----------------------------------------------------------------------
# The Geant4 Extension Headers and Sources
# - Slight hack, only list the manager and manager_parameters as
# headers, put the rest in SOURCES. Idea is to only publish public
# headers... Though these two headers are the only real public interface,
# they #include other headers, so these also need to be present...
#
if(Bayeux_WITH_GEANT4_MODULE)
   include(${Geant4_USE_FILE})
  
  set(${module_name}_GEANT4_HEADERS
    ${module_include_dir}/${module_name}/g4/manager_parameters.h
    ${module_include_dir}/${module_name}/g4/manager.h
    ${module_include_dir}/${module_name}/g4/loggable_support.h
    ${module_include_dir}/${module_name}/g4/g4_prng.h
    ${module_include_dir}/${module_name}/g4/track_history.h
    ${module_include_dir}/${module_name}/g4/simulation_module.h
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

  # Disable specific warnings because CLHEP source code makes use
  # of deprecated std::auto_ptr:
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-variable -Wno-deprecated-declarations -Wno-error=deprecated-declarations")

  set(${module_name}_GEANT4_SOURCES
    mctools/g4/data_libraries.h
    ${module_include_dir}/${module_name}/g4/processes/utils.h
    ${module_include_dir}/${module_name}/g4/processes/em_extra_models.h
    ${module_include_dir}/${module_name}/g4/processes/em_model_factory.h
    ${module_include_dir}/${module_name}/g4/simulation_module.h
    ${module_include_dir}/${module_name}/g4/primary_generator.h
    ${module_include_dir}/${module_name}/g4/magnetic_field.h
    ${module_include_dir}/${module_name}/g4/electromagnetic_field.h
    ${module_include_dir}/${module_name}/g4/em_field_equation_of_motion.h
    ${module_include_dir}/${module_name}/g4/em_field_g4_utils.h
    ${module_include_dir}/${module_name}/g4/em_field_g4_stuff.h
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
    ${module_include_dir}/${module_name}/g4/region_tools.h
    ${module_include_dir}/${module_name}/g4/detector_construction.h
    ${module_include_dir}/${module_name}/g4/stepping_action.h
    ${module_include_dir}/${module_name}/g4/sensitive_hit_collection.h
    ${module_include_dir}/${module_name}/g4/neutrons_physics_constructor.h
    ${module_include_dir}/${module_name}/g4/biasing_manager.h

    ${module_source_dir}/g4/processes/utils.cc
    ${module_source_dir}/g4/processes/em_extra_models.cc
    ${module_source_dir}/g4/processes/em_model_factory.cc
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
    ${module_source_dir}/g4/physics_list.cc
    ${module_source_dir}/g4/sensitive_hit.cc
    ${module_source_dir}/g4/stepping_verbose.cc
    ${module_source_dir}/g4/event_action.cc
    ${module_source_dir}/g4/base_physics_constructor.cc
    ${module_source_dir}/g4/region_tools.cc
    ${module_source_dir}/g4/detector_construction.cc
    ${module_source_dir}/g4/track_history.cc
    ${module_source_dir}/g4/stepping_action.cc
    ${module_source_dir}/g4/sensitive_hit_collection.cc
    ${module_source_dir}/g4/primary_generator.cc
    ${module_source_dir}/g4/magnetic_field.cc
    ${module_source_dir}/g4/electromagnetic_field.cc
    ${module_source_dir}/g4/em_field_equation_of_motion.cc
    ${module_source_dir}/g4/em_field_g4_stuff.cc
    ${module_source_dir}/g4/neutrons_physics_constructor.cc
    ${module_source_dir}/g4/biasing_manager.cc
    ${module_source_dir}/g4/manager.cc
    ${module_source_dir}/g4/simulation_module.cc
    )

  list(APPEND ${module_name}_MODULE_TESTS
    ${module_test_dir}/test_g4_prng.cxx
    ${module_test_dir}/test_g4_processes_em_model_factory.cxx
    ${module_test_dir}/test_g4_detector_construction.cxx
    ${module_test_dir}/test_g4_manager.cxx
    )

  # - Applications
  set(${module_name}_GEANT4_MODULE_APPS
    ${module_app_dir}/g4/g4_production.cxx
    ${module_app_dir}/g4/g4_seeds.cxx
    )

  add_library(Bayeux_mctools_geant4 SHARED ${mctools_GEANT4_SOURCES} ${mctool_GEANT4_HEADERS})
  target_compile_features(Bayeux_mctools_geant4 PUBLIC ${BAYEUX_CXX_COMPILE_FEATURES})
  target_include_directories(Bayeux_mctools_geant4
    PUBLIC
    $<BUILD_INTERFACE:${BAYEUX_BUILD_INCLUDEDIR}>
    $<BUILD_INTERFACE:${BAYEUX_BUILD_INCLUDEDIR}/bayeux>
     $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
     $<BUILD_INTERFACE:${module_include_dir}>
     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bayeux>
     ${Geant4_INCLUDE_DIRS}
     $<TARGET_PROPERTY:Bayeux,INTERFACE_INCLUDE_DIRECTORIES>
     )
  # Hack - strip "-D" flag as we should only supply the def names
  set(Bayeux_Geant4_DEFINITIONS)
  foreach(_def ${Geant4_DEFINITIONS})
    string(REGEX REPLACE "^-D" "" _bxdef ${_def})
    list(APPEND Bayeux_Geant4_DEFINITIONS ${_bxdef})
  endforeach()
  # message (STATUS  "Bayeux_Geant4_DEFINITIONS='${Bayeux_Geant4_DEFINITIONS}'")

  set_target_properties(Bayeux_mctools_geant4
    PROPERTIES COMPILE_DEFINITIONS "${Bayeux_Geant4_DEFINITIONS}"
    )
  message(STATUS "Geant4_LIBRARIES='${Geant4_LIBRARIES}'")
  message(STATUS "Geant4_LIBRARY_DIR='${Geant4_LIBRARY_DIR}'")
  target_link_libraries(Bayeux_mctools_geant4
    PUBLIC
    # -L${Geant4_LIBRARY_DIR}
    ${Geant4_LIBRARIES}
    Boost::thread
    )

  # - Set RPATH as needed
  set_target_properties(Bayeux_mctools_geant4 PROPERTIES SKIP_BUILD_RPATH FALSE)
  set_target_properties(Bayeux_mctools_geant4 PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
  set_target_properties(Bayeux_mctools_geant4 PROPERTIES BUILD_WITH_INSTALL_RPATH 0)
  if(UNIX AND NOT APPLE)
    set_target_properties(Bayeux_mctools_geant4
      PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
      )
  endif()

  # - Create the public link interface
  set_target_properties(Bayeux_mctools_geant4 PROPERTIES INTERFACE_LINK_LIBRARIES "")

  list(APPEND Bayeux_ADDON_TARGETS Bayeux_mctools_geant4)

endif()
