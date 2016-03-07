
#-----------------------------------------------------------------------
# The MCNP Extension Headers and Sources
# - Slight hack, only list the manager and manager_parameters as
# headers, put the rest in SOURCES. Idea is to only publish public
# headers... Though these two headers are the only real public interface,
# they #include other headers, so these also need to be present...
#
if(BAYEUX_WITH_MCNP_MODULE)

  set(${module_name}_MCNP
    ${module_include_dir}/${module_name}/mcnp/utils.h
    ${module_include_dir}/${module_name}/mcnp/units.h
    ${module_include_dir}/${module_name}/mcnp/format.h
    ${module_include_dir}/${module_name}/mcnp/mcsetup.h
    ${module_include_dir}/${module_name}/mcnp/material/material.h
    ${module_include_dir}/${module_name}/mcnp/material/material_store.h
    ${module_include_dir}/${module_name}/mcnp/geometry/surface_type.h
    ${module_include_dir}/${module_name}/mcnp/geometry/surface.h
    ${module_include_dir}/${module_name}/mcnp/geometry/surface_store.h
    # ${module_include_dir}/${module_name}/mcnp/geometry/universe.h
    # ${module_include_dir}/${module_name}/mcnp/geometry/universe_store.h
    )
  # - Published headers
  foreach(_hdrin ${${module_name}_MCNP_HEADERS})
    string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
    string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
    configure_file(${_hdrin} ${_hdrout} @ONLY)
  endforeach()

  # # - Configurable private headers
  # configure_file(${module_include_dir}/${module_name}/mcnp/xxx.h.in
  #   mctools/mcnp/xxx.h
  #   @ONLY
  #   )

  set(${module_name}_MCNP_SOURCES
    # mctools/g4/xxx.h
    ${module_source_dir}/mcnp/units.cc
    ${module_source_dir}/mcnp/format.cc
    ${module_source_dir}/mcnp/mcsetup.cc
    ${module_source_dir}/mcnp/material/material.cc
    ${module_source_dir}/mcnp/material/material_store.cc
    ${module_source_dir}/mcnp/geometry/surface_type.cc
    ${module_source_dir}/mcnp/geometry/surface.cc
    ${module_source_dir}/mcnp/geometry/surface_store.cc
    # ${module_source_dir}/mcnp/geometry/universe.cc
    # ${module_source_dir}/mcnp/geometry/universe_store.cc
    )

  list(APPEND ${module_name}_MODULE_TESTS
    ${module_test_dir}/test_mcnp_format.cxx
    ${module_test_dir}/test_mcnp_surface.cxx
    ${module_test_dir}/test_mcnp_material.cxx
    ${module_test_dir}/test_mcnp_mcsetup.cxx
    )

    # # - Applications
    # set(${module_name}_MCNP_MODULE_APPS
    #   ${module_app_dir}/mcnp/mcnp_production.cxx
    #   )

  include_directories(${MCNP_INCLUDE_DIRS})
  add_library(Bayeux_mctools_mcnp SHARED ${mctools_MCNP_SOURCES} ${mctool_MCNP_HEADERS})
  target_compile_features(Bayeux_mctools_mcnp PUBLIC ${BAYEUX_CXX_COMPILE_FEATURES})
  # Hack - strip "-D" flag as we should only supply the def names
  set(Bayeux_MCNP_DEFINITIONS)
  foreach(_def ${MCNP_DEFINITIONS})
    string(REGEX REPLACE "^-D" "" _bxdef ${_def})
    list(APPEND Bayeux_MCNP_DEFINITIONS ${_bxdef})
  endforeach()

  set_target_properties(Bayeux_mctools_mcnp
    PROPERTIES COMPILE_DEFINITIONS "${Bayeux_MCNP_DEFINITIONS}"
    )
  target_link_libraries(Bayeux_mctools_mcnp)

  # - Set RPATH as needed
  set_target_properties(Bayeux_mctools_mcnp PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

  if(UNIX AND NOT APPLE)
    set_target_properties(Bayeux_mctools_mcnp
      PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
      )
  elseif(APPLE)
    # Temporary setting - needs testing
    set_target_properties(Bayeux_mctools_mcnp
      PROPERTIES
        INSTALL_NAME_DIR "@rpath"
        LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  # - Create the public link interface
  set_target_properties(Bayeux_mctools_mcnp PROPERTIES LINK_INTERFACE_LIBRARIES "")
  # or shall we use this one ?
  # set_target_properties(Bayeux_mctools_mcnp PROPERTIES INTERFACE_LINK_LIBRARIES "")

  list(APPEND Bayeux_ADDON_TARGETS Bayeux_mctools_mcnp)
endif()
