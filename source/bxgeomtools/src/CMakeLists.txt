# - CMakeLists.txt for geomtools/src subdir

#-----------------------------------------------------------------------
# function list_prepend(<list> <value>)
#          Prepend value to each element in list, in place
function(list_prepend _list _value)
  set(_tmp_list)
  foreach(_elem ${${_list}})
    list(APPEND _tmp_list "${_value}${_elem}")
  endforeach()
  set(${_list} ${_tmp_list} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources,
# public headers, private headers and resources.
#-----------------------------------------------------------------------
# PUBLIC HEADERS
# Because we're in the awkward position of separate headers and sources,
# use a simple listing and then prepend the actual source dir root

# file ( GLOB geomtools_PUBLIC_HEADERS
#   ${PROJECT_SOURCE_DIR}/include/geomtools/*.h
#   ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
#   )

set(geomtools_PUBLIC_HEADERS
  include/geomtools/address_set.h
  include/geomtools/base_hit.h
  include/geomtools/bio_guard.h
  include/geomtools/blur_spot.h
  include/geomtools/box.h
  include/geomtools/circle.h
  include/geomtools/clhep.h
  include/geomtools/color.h
  include/geomtools/cylinder.h
  include/geomtools/cylindric_extrusion_boxed_model.h
  include/geomtools/disk.h
  include/geomtools/display_data.h
  include/geomtools/gdml_export.h
  include/geomtools/gdml_writer.h
  include/geomtools/geom_id.h
  include/geomtools/geom_info.h
  include/geomtools/geom_map.h
  include/geomtools/geomtools.h
  include/geomtools/gnuplot_drawer.h
  include/geomtools/gnuplot_draw.h
  include/geomtools/gnuplot_i.h
  include/geomtools/helix_3d.h
  include/geomtools/hexagon_box.h
  include/geomtools/i_boxed_model.h
  include/geomtools/i_composite_shape_3d.h
  include/geomtools/id_mgr.h
  include/geomtools/id_selector.h
  include/geomtools/i_locator.h
  include/geomtools/i_model.h
  include/geomtools/intersection_3d.h
  include/geomtools/intersection.h
  include/geomtools/i_object_3d.h
  include/geomtools/i_placement.h
  include/geomtools/i_shape_1d.h
  include/geomtools/i_shape_2d.h
  include/geomtools/i_shape_3d.h
  include/geomtools/i_stackable.h
  include/geomtools/i_wires_3d_rendering.h
  include/geomtools/line_3d.h
  include/geomtools/logical_volume.h
  include/geomtools/mapping.h
  include/geomtools/mapping_plugin.h
  include/geomtools/mapping_utils.h
  include/geomtools/material.h
  include/geomtools/model_factory.h
  include/geomtools/model_macros.h
  include/geomtools/model_with_internal_items_tools.h
  include/geomtools/multiple_items_model.h
  include/geomtools/multiple_placement.h
  include/geomtools/physical_volume.h
  include/geomtools/placement.h
  include/geomtools/plane.h
  include/geomtools/plate_with_hole_model.h
  include/geomtools/polycone.h
  include/geomtools/polyhedra.h
  include/geomtools/polyline_3d.h
  include/geomtools/rectangle.h
  include/geomtools/regular_grid_placement.h
  include/geomtools/regular_linear_placement.h
  include/geomtools/regular_polygon.h
  include/geomtools/replicated_boxed_model.h
  include/geomtools/replicated_model.h
  include/geomtools/grid_model.h
  include/geomtools/rotated_boxed_model.h
  include/geomtools/sensitive.h
  include/geomtools/simple_boxed_model.h
  include/geomtools/simple_shaped_model.h
  include/geomtools/simple_world_model.h
  include/geomtools/smart_id_locator.h
  include/geomtools/sphere.h
  include/geomtools/spherical_extrusion_box_model.h
  include/geomtools/spherical_extrusion_cylinder_model.h
  include/geomtools/stacked_model.h
  include/geomtools/stl_tools.h
  include/geomtools/subtraction_3d.h
  include/geomtools/surrounded_boxed_model.h
  include/geomtools/tessellation.h
  include/geomtools/tube.h
  include/geomtools/union_3d.h
  include/geomtools/units.h
  include/geomtools/utils.h
  include/geomtools/visibility.h
  include/geomtools/manager.h
  include/geomtools/manager_macros.h
  include/geomtools/geometry_service.h
  include/geomtools/materials_plugin.h
  include/geomtools/materials_utils.h

  include/geomtools/ocd_support.h

  include/geomtools/base_hit.ipp
  include/geomtools/blur_spot.ipp
  include/geomtools/display_data.ipp
  include/geomtools/geom_id.ipp
  include/geomtools/helix_3d.ipp
  include/geomtools/i_placement.ipp
  include/geomtools/line_3d.ipp
  include/geomtools/placement.ipp
  include/geomtools/polyline_3d.ipp
  include/geomtools/utils.ipp
  include/geomtools/the_serializable.ipp
  include/geomtools/the_serializable.h

  include/geomtools/clhep-reflect.h
  include/geomtools/utils-reflect.h
  include/geomtools/geom_id-reflect.h
  include/geomtools/i_placement-reflect.h
  include/geomtools/placement-reflect.h
  include/geomtools/base_hit-reflect.h
  include/geomtools/blur_spot-reflect.h
  include/geomtools/the_introspectable.h
  include/geomtools/reflection_guard.h

  )
list_prepend(geomtools_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/geomtools/geomtools_config.h.in
  ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
  )
list(APPEND geomtools_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(geomtools_detail_HEADERS
  include/geomtools/detail/bio_link_guard.h
  include/geomtools/detail/model_tools.h
  include/geomtools/detail/manager-inl.h
  include/geomtools/detail/reflection_link_guard.h
  )
list_prepend(geomtools_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${geomtools_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if geomtools
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail)
    install(FILES ${geomtools_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(geomtools_SOURCES
  base_hit.cc
  blur_spot.cc
  address_set.cc
  gdml_export.cc
  gdml_writer.cc
  geom_id.cc
  geom_info.cc
  geom_map.cc
  hexagon_box.cc
  id_mgr.cc
  id_selector.cc
  i_locator.cc
  intersection.cc
  i_placement.cc
  i_stackable.cc
  logical_volume.cc
  materials_plugin.cc
  materials_utils.cc
  mapping.cc
  mapping_plugin.cc
  mapping_utils.cc
  model_factory.cc
  model_with_internal_items_tools.cc
  multiple_items_model.cc
  physical_volume.cc
  multiple_placement.cc
  regular_grid_placement.cc
  regular_linear_placement.cc
  placement.cc
  regular_polygon.cc
  sensitive.cc
  smart_id_locator.cc
  helix_3d.cc
  line_3d.cc
  polyline_3d.cc
  circle.cc
  rectangle.cc
  disk.cc
  plane.cc
  plate_with_hole_model.cc
  spherical_extrusion_cylinder_model.cc
  spherical_extrusion_box_model.cc
  cylindric_extrusion_boxed_model.cc
  replicated_boxed_model.cc
  replicated_model.cc
  grid_model.cc
  rotated_boxed_model.cc
  simple_boxed_model.cc
  simple_shaped_model.cc
  simple_world_model.cc
  stacked_model.cc
  i_boxed_model.cc
  surrounded_boxed_model.cc
  cylinder.cc
  box.cc
  sphere.cc
  tube.cc
  tessellation.cc
  stl_tools.cc
  polycone.cc
  polyhedra.cc
  subtraction_3d.cc
  union_3d.cc
  intersection_3d.cc
  i_composite_shape_3d.cc
  geomtools_config.cc
  i_model.cc
  i_shape_1d.cc
  i_shape_2d.cc
  i_shape_3d.cc
  i_object_3d.cc
  visibility.cc
  color.cc
  material.cc
  utils.cc
  gnuplot_draw.cc
  display_data.cc
  manager.cc
  geometry_service.cc
  ocd_support.cc
  )

#message(STATUS "********** GEOMTOOLS_WITH_GNUPLOT_DISPLAY=${GEOMTOOLS_WITH_GNUPLOT_DISPLAY}")
if (GEOMTOOLS_WITH_GNUPLOT_DISPLAY)
  list (APPEND geomtools_SOURCES
    ${PROJECT_SOURCE_DIR}/src/gnuplot_i.cc
    ${PROJECT_SOURCE_DIR}/src/gnuplot_drawer.cc
    )
endif ()
#message(STATUS "********** geomtools_SOURCES=${geomtools_SOURCES}")

#-----------------------------------------------------------------------
# CMAKE RESOURCE FILES
# We do this here because it allows us to to add the config files as
# resources on Mac. This allows them to be easily imported into the
# Framework with minimal platform specific commands by adding them
# as sources of the framework/library.
# We have to deal with the library depends part of this later because
# their generation require the targets to be present...
#
if(APPLE)
  set(TO_GEOMTOOLS_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_GEOMTOOLS_INCLUDEDIR
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/geomtools-${geomtools_VERSION}"
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(geomtools_RESOURCES
  cmake/geomtools-config.cmake
  cmake/geomtools-config-version.cmake
  )

foreach(_dtresource ${geomtools_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND geomtools_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND geomtools_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${geomtools_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${geomtools_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/geomtools-${geomtools_VERSION}"
    COMPONENT Development
    )
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/geomtools-config.in
    ${PROJECT_BINARY_DIR}/cmake/geomtools-config
    @ONLY
    )

install(FILES ${PROJECT_BINARY_DIR}/cmake/geomtools-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    		GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

if ( GEOMTOOLS_WITH_BASH_SETUP )
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/geomtools_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/geomtools_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/geomtools_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
  )
endif ()

#-----------------------------------------------------------------------
# BUILD THE GEOMTOOLS LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${mygsl_INCLUDE_DIRS}
  ${materials_INCLUDE_DIRS}
  ${CLHEP_INCLUDE_DIR}
  )

add_library(geomtools SHARED
  ${geomtools_PUBLIC_HEADERS}
  ${geomtools_detail_HEADERS}
  ${geomtools_SOURCES}
  ${geomtools_RESOURCES_OUTPUTS}
  )

set_target_properties(geomtools
  PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.geomtools
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${geomtools_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${geomtools_VERSION}
    VERSION                               ${geomtools_VERSION}
    SOVERSION                             ${geomtools_VERSION_MAJOR}
    PUBLIC_HEADER                         "${geomtools_PUBLIC_HEADERS}"
  )


target_link_libraries(geomtools
     ${mygsl_LIBRARIES}
     ${materials_LIBRARIES}
     ${CLHEP_LIBRARIES}
     ${CMAKE_DL_LIBS}
  )

set(GEOMTOOLS_TARGETS geomtools)

#-----------------------------------------------------------------------
# BUILD THE GEOMTOOLS BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(GEOMTOOLS_WITH_BIO)
  set(geomtools_bio_SOURCES
    include/geomtools/the_serializable.h
    include/geomtools/the_serializable.ipp
    src/the_serializable.cc
    )
  list_prepend(geomtools_bio_SOURCES "${PROJECT_SOURCE_DIR}/")

  add_library(geomtools_bio SHARED ${geomtools_bio_SOURCES})

  target_link_libraries(geomtools_bio geomtools)

  if(APPLE)
    set_property(TARGET geomtools_bio APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND GEOMTOOLS_TARGETS geomtools_bio)
endif()

#-----------------------------------------------------------------------
# BUILD THE GEOMTOOLS REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(GEOMTOOLS_WITH_REFLECTION)
  set(geomtools_reflection_SOURCES
    include/geomtools/clhep-reflect.h
    include/geomtools/utils-reflect.h
    include/geomtools/geom_id-reflect.h
    include/geomtools/base_hit-reflect.h
    include/geomtools/i_placement-reflect.h
    include/geomtools/placement-reflect.h
    include/geomtools/blur_spot-reflect.h
    include/geomtools/the_introspectable.h
    src/the_introspectable.cc
    )
  list_prepend(geomtools_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")

  message (STATUS "Build geomtools reflection DLL...")

  add_library(geomtools_reflection SHARED ${geomtools_reflection_SOURCES})

  target_link_libraries(geomtools_reflection
                        geomtools
                        #datatools_reflection
                       )

  if(APPLE)
    set_property(TARGET geomtools_reflection APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND GEOMTOOLS_TARGETS geomtools_reflection)
endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature
install(TARGETS ${GEOMTOOLS_TARGETS}
  EXPORT        geomtools-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${GEOMTOOLS_FRAMEWORK_LOCATION}
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT geomtools-libraries
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT geomtools-libraries
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/geomtools-${geomtools_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for geomtools/src subdir
