# - Module definition for geomtools submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name geomtools)
set(module_root_dir     "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir  "${module_root_dir}/include")
set(module_source_dir   "${module_root_dir}/src")
set(module_test_dir     "${module_root_dir}/testing")
set(module_app_dir      "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(geomtools_VERSION_MAJOR 4)
set(geomtools_VERSION_MINOR 0)
set(geomtools_VERSION_PATCH 0)
set(geomtools_VERSION "${geomtools_VERSION_MAJOR}.${geomtools_VERSION_MINOR}.${geomtools_VERSION_PATCH}")

# - Boost I/O, CAMP Reflection, Gnuplot pipe interface
set(GEOMTOOLS_WITH_BIO 1)
set(GEOMTOOLS_WITH_REFLECTION 1)
set(GEOMTOOLS_WITH_GNUPLOT_DISPLAY 1)
set(GEOMTOOLS_WITH_ROOT_DISPLAY 1)

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/address_set.h
  ${module_include_dir}/${module_name}/base_hit.h
  ${module_include_dir}/${module_name}/base_hit.ipp
  ${module_include_dir}/${module_name}/bio_guard.h
  ${module_include_dir}/${module_name}/blur_spot.h
  ${module_include_dir}/${module_name}/blur_spot.ipp
  ${module_include_dir}/${module_name}/box.h
  ${module_include_dir}/${module_name}/circle.h
  ${module_include_dir}/${module_name}/clhep.h
  ${module_include_dir}/${module_name}/color.h
  ${module_include_dir}/${module_name}/cone.h
  ${module_include_dir}/${module_name}/cylinder.h
  ${module_include_dir}/${module_name}/cylindric_extrusion_boxed_model.h
  ${module_include_dir}/${module_name}/detail/bio_link_guard.h
  ${module_include_dir}/${module_name}/detail/manager-inl.h
  ${module_include_dir}/${module_name}/detail/model_tools.h
  ${module_include_dir}/${module_name}/disk.h
  ${module_include_dir}/${module_name}/display_data.h
  ${module_include_dir}/${module_name}/display_data.ipp
  ${module_include_dir}/${module_name}/gdml_export.h
  ${module_include_dir}/${module_name}/gdml_writer.h
  ${module_include_dir}/${module_name}/geometry_service.h
  ${module_include_dir}/${module_name}/geom_id.h
  ${module_include_dir}/${module_name}/geom_id.ipp
  ${module_include_dir}/${module_name}/geom_info.h
  ${module_include_dir}/${module_name}/geom_map.h
  ${module_include_dir}/${module_name}/geomtools_config.h.in
  ${module_include_dir}/${module_name}/geomtools.h
  ${module_include_dir}/${module_name}/gnuplot_drawer.h
  ${module_include_dir}/${module_name}/gnuplot_draw.h
  ${module_include_dir}/${module_name}/gnuplot_i.h
  ${module_include_dir}/${module_name}/grid_model.h
  ${module_include_dir}/${module_name}/helix_3d.h
  ${module_include_dir}/${module_name}/helix_3d.ipp
  ${module_include_dir}/${module_name}/hexagon_box.h
  ${module_include_dir}/${module_name}/i_boxed_model.h
  ${module_include_dir}/${module_name}/i_composite_shape_3d.h
  ${module_include_dir}/${module_name}/id_mgr.h
  ${module_include_dir}/${module_name}/id_selector.h
  ${module_include_dir}/${module_name}/i_locator.h
  ${module_include_dir}/${module_name}/i_model.h
  ${module_include_dir}/${module_name}/intersection_3d.h
  ${module_include_dir}/${module_name}/intersection.h
  ${module_include_dir}/${module_name}/i_object_3d.h
  ${module_include_dir}/${module_name}/i_placement.h
  ${module_include_dir}/${module_name}/i_placement.ipp
  ${module_include_dir}/${module_name}/i_shape_1d.h
  ${module_include_dir}/${module_name}/i_shape_2d.h
  ${module_include_dir}/${module_name}/i_shape_3d.h
  ${module_include_dir}/${module_name}/i_stackable.h
  ${module_include_dir}/${module_name}/i_wires_3d_rendering.h
  ${module_include_dir}/${module_name}/line_3d.h
  ${module_include_dir}/${module_name}/line_3d.ipp
  ${module_include_dir}/${module_name}/logical_volume.h
  ${module_include_dir}/${module_name}/manager.h
  ${module_include_dir}/${module_name}/manager_macros.h
  ${module_include_dir}/${module_name}/mapping.h
  ${module_include_dir}/${module_name}/mapping_plugin.h
  ${module_include_dir}/${module_name}/mapping_utils.h
  ${module_include_dir}/${module_name}/material.h
  ${module_include_dir}/${module_name}/materials_plugin.h
  ${module_include_dir}/${module_name}/materials_utils.h
  ${module_include_dir}/${module_name}/model_factory.h
  ${module_include_dir}/${module_name}/model_macros.h
  ${module_include_dir}/${module_name}/model_with_internal_items_tools.h
  ${module_include_dir}/${module_name}/multiple_items_model.h
  ${module_include_dir}/${module_name}/multiple_placement.h
  ${module_include_dir}/${module_name}/ocd_support.h
  ${module_include_dir}/${module_name}/physical_volume.h
  ${module_include_dir}/${module_name}/placement.h
  ${module_include_dir}/${module_name}/placement.ipp
  ${module_include_dir}/${module_name}/plane.h
  ${module_include_dir}/${module_name}/plate_with_hole_model.h
  ${module_include_dir}/${module_name}/polycone.h
  ${module_include_dir}/${module_name}/polyhedra.h
  ${module_include_dir}/${module_name}/polyline_3d.h
  ${module_include_dir}/${module_name}/polyline_3d.ipp
  ${module_include_dir}/${module_name}/rectangle.h
  ${module_include_dir}/${module_name}/regular_grid_placement.h
  ${module_include_dir}/${module_name}/regular_linear_placement.h
  ${module_include_dir}/${module_name}/regular_polygon.h
  ${module_include_dir}/${module_name}/replicated_boxed_model.h
  ${module_include_dir}/${module_name}/replicated_model.h
  ${module_include_dir}/${module_name}/rotated_boxed_model.h
  ${module_include_dir}/${module_name}/sensitive.h
  ${module_include_dir}/${module_name}/simple_boxed_model.h
  ${module_include_dir}/${module_name}/simple_shaped_model.h
  ${module_include_dir}/${module_name}/simple_world_model.h
  ${module_include_dir}/${module_name}/smart_id_locator.h
  ${module_include_dir}/${module_name}/sphere.h
  ${module_include_dir}/${module_name}/spherical_extrusion_box_model.h
  ${module_include_dir}/${module_name}/spherical_extrusion_cylinder_model.h
  ${module_include_dir}/${module_name}/stacked_model.h
  ${module_include_dir}/${module_name}/stl_tools.h
  ${module_include_dir}/${module_name}/subtraction_3d.h
  ${module_include_dir}/${module_name}/surrounded_boxed_model.h
  ${module_include_dir}/${module_name}/tessellation.h
  ${module_include_dir}/${module_name}/the_serializable.h
  ${module_include_dir}/${module_name}/the_serializable.ipp
  ${module_include_dir}/${module_name}/tube.h
  ${module_include_dir}/${module_name}/union_3d.h
  ${module_include_dir}/${module_name}/units.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/utils.ipp
  ${module_include_dir}/${module_name}/visibility.h
  ${module_include_dir}/${module_name}/geomtools_driver.h
  ${module_include_dir}/${module_name}/visibility.h
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/base_hit-reflect.h
  ${module_include_dir}/${module_name}/blur_spot-reflect.h
  ${module_include_dir}/${module_name}/clhep-reflect.h
  ${module_include_dir}/${module_name}/detail/reflection_link_guard.h
  ${module_include_dir}/${module_name}/geom_id-reflect.h
  ${module_include_dir}/${module_name}/i_placement-reflect.h
  ${module_include_dir}/${module_name}/placement-reflect.h
  ${module_include_dir}/${module_name}/reflection_guard.h
  ${module_include_dir}/${module_name}/the_introspectable.h
  ${module_include_dir}/${module_name}/utils-reflect.h
  ${module_include_dir}/${module_name}/resource.h
  )

# - configure resources
configure_file(${module_source_dir}/resource.cc.in
               bx${module_name}/resource.cc)

# - NB Order of sources appears to be important - taken from geomtools
#   listing. Note that the_serializable.cc is added manually - not
#   totally clear this is in the right place...
set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/base_hit.cc
  ${module_source_dir}/blur_spot.cc
  ${module_source_dir}/address_set.cc
  ${module_source_dir}/gdml_export.cc
  ${module_source_dir}/gdml_writer.cc
  ${module_source_dir}/geom_id.cc
  ${module_source_dir}/geom_info.cc
  ${module_source_dir}/geom_map.cc
  ${module_source_dir}/hexagon_box.cc
  ${module_source_dir}/id_mgr.cc
  ${module_source_dir}/id_selector.cc
  ${module_source_dir}/i_locator.cc
  ${module_source_dir}/intersection.cc
  ${module_source_dir}/i_placement.cc
  ${module_source_dir}/i_stackable.cc
  ${module_source_dir}/logical_volume.cc
  ${module_source_dir}/materials_plugin.cc
  ${module_source_dir}/materials_utils.cc
  ${module_source_dir}/mapping.cc
  ${module_source_dir}/mapping_plugin.cc
  ${module_source_dir}/mapping_utils.cc
  ${module_source_dir}/model_factory.cc
  ${module_source_dir}/model_with_internal_items_tools.cc
  ${module_source_dir}/multiple_items_model.cc
  ${module_source_dir}/physical_volume.cc
  ${module_source_dir}/multiple_placement.cc
  ${module_source_dir}/regular_grid_placement.cc
  ${module_source_dir}/regular_linear_placement.cc
  ${module_source_dir}/placement.cc
  ${module_source_dir}/regular_polygon.cc
  ${module_source_dir}/sensitive.cc
  ${module_source_dir}/smart_id_locator.cc
  ${module_source_dir}/helix_3d.cc
  ${module_source_dir}/line_3d.cc
  ${module_source_dir}/polyline_3d.cc
  ${module_source_dir}/circle.cc
  ${module_source_dir}/rectangle.cc
  ${module_source_dir}/disk.cc
  ${module_source_dir}/plane.cc
  ${module_source_dir}/plate_with_hole_model.cc
  ${module_source_dir}/spherical_extrusion_cylinder_model.cc
  ${module_source_dir}/spherical_extrusion_box_model.cc
  ${module_source_dir}/cylindric_extrusion_boxed_model.cc
  ${module_source_dir}/replicated_boxed_model.cc
  ${module_source_dir}/replicated_model.cc
  ${module_source_dir}/grid_model.cc
  ${module_source_dir}/rotated_boxed_model.cc
  ${module_source_dir}/simple_boxed_model.cc
  ${module_source_dir}/simple_shaped_model.cc
  ${module_source_dir}/simple_world_model.cc
  ${module_source_dir}/stacked_model.cc
  ${module_source_dir}/i_boxed_model.cc
  ${module_source_dir}/surrounded_boxed_model.cc
  ${module_source_dir}/cylinder.cc
  ${module_source_dir}/box.cc
  ${module_source_dir}/cone.cc
  ${module_source_dir}/sphere.cc
  ${module_source_dir}/tube.cc
  ${module_source_dir}/tessellation.cc
  ${module_source_dir}/stl_tools.cc
  ${module_source_dir}/polycone.cc
  ${module_source_dir}/polyhedra.cc
  ${module_source_dir}/subtraction_3d.cc
  ${module_source_dir}/union_3d.cc
  ${module_source_dir}/intersection_3d.cc
  ${module_source_dir}/i_composite_shape_3d.cc
  ${module_source_dir}/geomtools_config.cc
  ${module_source_dir}/i_model.cc
  ${module_source_dir}/i_shape_1d.cc
  ${module_source_dir}/i_shape_2d.cc
  ${module_source_dir}/i_shape_3d.cc
  ${module_source_dir}/i_object_3d.cc
  ${module_source_dir}/visibility.cc
  ${module_source_dir}/color.cc
  ${module_source_dir}/material.cc
  ${module_source_dir}/utils.cc
  ${module_source_dir}/gnuplot_draw.cc
  ${module_source_dir}/display_data.cc
  ${module_source_dir}/manager.cc
  ${module_source_dir}/geometry_service.cc
  ${module_source_dir}/ocd_support.cc
  ${module_source_dir}/the_serializable.cc
  ${module_source_dir}/gnuplot_i.cc
  ${module_source_dir}/gnuplot_drawer.cc
  ${module_source_dir}/geomtools_driver.cc
  ${module_source_dir}/version.cc
  bx${module_name}/resource.cc
  #${module_source_dir}/the_introspectable.cc
  )

# - Reflection components
# if(GEOMTOOLS_WITH_REFLECTION)
#   set(geomtools_REFLECTION_HEADERS
#     ${module_include_dir}/${module_name}/base_hit-reflect.h
#     ${module_include_dir}/${module_name}/blur_spot-reflect.h
#     ${module_include_dir}/${module_name}/clhep-reflect.h
#     ${module_include_dir}/${module_name}/detail/reflection_link_guard.h
#     ${module_include_dir}/${module_name}/geom_id-reflect.h
#     ${module_include_dir}/${module_name}/i_placement-reflect.h
#     ${module_include_dir}/${module_name}/placement-reflect.h
#     ${module_include_dir}/${module_name}/reflection_guard.h
#     ${module_include_dir}/${module_name}/the_introspectable.h
#     ${module_include_dir}/${module_name}/utils-reflect.h
#     )
#   set(geomtools_REFLECTION_SOURCES
#     ${module_source_dir}/the_introspectable.cc
#     )
#   set(geomtools_REFLECTION_TESTS
#     ${module_test_dir}/test_reflection_0.cxx
#     )
# endif()

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "GEOMTOOLS_RESOURCE_DIR=${module_resource_dir};GEOMTOOLS_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_address_set.cxx
  ${module_test_dir}/test_base_hit.cxx
  ${module_test_dir}/test_blur_spot.cxx
  ${module_test_dir}/test_box.cxx
  ${module_test_dir}/test_circle.cxx
  ${module_test_dir}/test_color.cxx
  ${module_test_dir}/test_cone.cxx
  ${module_test_dir}/test_cylinder.cxx
  ${module_test_dir}/test_disk.cxx
  ${module_test_dir}/test_display_data.cxx
  ${module_test_dir}/test_gdml_writer.cxx
  ${module_test_dir}/test_geom_id.cxx
  ${module_test_dir}/test_geomtools.cxx
  ${module_test_dir}/test_gnuplot_draw.cxx
  ${module_test_dir}/test_gnuplot_i.cxx
  ${module_test_dir}/test_helix.cxx
  ${module_test_dir}/test_hexagon_box.cxx
  ${module_test_dir}/test_id_mgr.cxx
  ${module_test_dir}/test_id_selector.cxx
  ${module_test_dir}/test_i_model.cxx
  ${module_test_dir}/test_intersection_3d.cxx
  ${module_test_dir}/test_intersection.cxx
  ${module_test_dir}/test_logical_volume.cxx
  ${module_test_dir}/test_manager.cxx
  ${module_test_dir}/test_model_factory.cxx
  ${module_test_dir}/test_multiple_placement.cxx
  ${module_test_dir}/test_physical_volume.cxx
  ${module_test_dir}/test_placement_2.cxx
  ${module_test_dir}/test_placement_3.cxx
  ${module_test_dir}/test_placement.cxx
  ${module_test_dir}/test_plane.cxx # <- Will fail without gnuplot_i
  ${module_test_dir}/test_polycone_2.cxx
  ${module_test_dir}/test_polycone.cxx
  ${module_test_dir}/test_polyhedra.cxx
  ${module_test_dir}/test_polyline_3d.cxx
  ${module_test_dir}/test_rectangle.cxx
  ${module_test_dir}/test_regular_grid_placement.cxx
  ${module_test_dir}/test_regular_linear_placement.cxx
  ${module_test_dir}/test_regular_polygon.cxx
  ${module_test_dir}/test_rotation_3d.cxx
  ${module_test_dir}/test_s0.cxx
  ${module_test_dir}/test_serializable_2.cxx
  ${module_test_dir}/test_serializable_3.cxx
  ${module_test_dir}/test_serializable.cxx
  ${module_test_dir}/test_service_manager.cxx
  ${module_test_dir}/test_sphere.cxx
  ${module_test_dir}/test_stl_tools.cxx
  ${module_test_dir}/test_subtraction_3d.cxx
  ${module_test_dir}/test_tessellated_solid.cxx
  ${module_test_dir}/test_tube.cxx
  ${module_test_dir}/test_union_3d.cxx
  ${module_test_dir}/test_utils_2.cxx
  ${module_test_dir}/test_utils.cxx
  ${module_test_dir}/test_reflection_0.cxx
  )

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/geomtools_inspector.cxx
  )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/gdml_schema/gdml_core.xsd
  ${module_resource_dir}/gdml_schema/gdml_define.xsd
  ${module_resource_dir}/gdml_schema/gdml_extensions.xsd
  ${module_resource_dir}/gdml_schema/gdml_materials.xsd
  ${module_resource_dir}/gdml_schema/gdml_parameterised.xsd
  ${module_resource_dir}/gdml_schema/gdml_replicas.xsd
  ${module_resource_dir}/gdml_schema/gdml_solids.xsd
  ${module_resource_dir}/gdml_schema/gdml.xsd
  ${module_resource_dir}/gdml_schema/XMLSchema-instance
  )

# - Publish resource files
foreach(_rfin ${${module_name}_MODULE_RESOURCES})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${module_resource_dir}" "${MODULE_RESOURCE_ROOT}" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()
