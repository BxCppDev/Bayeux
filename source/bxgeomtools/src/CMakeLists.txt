# - CMakeLists.txt for geomtools/src subdir

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB geomtools_detail_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/geomtools/detail/*.h
  )

file ( GLOB geomtools_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/geomtools/*.h
  ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
  )

# orderer set of source files :
set (_geomtools_source_files
  ${PROJECT_SOURCE_DIR}/src/blur_spot.cc 
  ${PROJECT_SOURCE_DIR}/src/address_set.cc
  ${PROJECT_SOURCE_DIR}/src/any_shape_3d.cc
  ${PROJECT_SOURCE_DIR}/src/gdml_export.cc 
  ${PROJECT_SOURCE_DIR}/src/gdml_writer.cc 
  ${PROJECT_SOURCE_DIR}/src/geom_id.cc
  ${PROJECT_SOURCE_DIR}/src/geom_info.cc 
  ${PROJECT_SOURCE_DIR}/src/geom_map.cc 
  ${PROJECT_SOURCE_DIR}/src/gnuplot_draw.cc 
  ${PROJECT_SOURCE_DIR}/src/gnuplot_drawer.cc 
  ${PROJECT_SOURCE_DIR}/src/gnuplot_i.cc 
  ${PROJECT_SOURCE_DIR}/src/hexagon_box.cc 
  ${PROJECT_SOURCE_DIR}/src/id_mgr.cc 
  ${PROJECT_SOURCE_DIR}/src/id_selector.cc 
  ${PROJECT_SOURCE_DIR}/src/i_locator.cc  
  ${PROJECT_SOURCE_DIR}/src/intersection.cc  
  ${PROJECT_SOURCE_DIR}/src/i_placement.cc 
  ${PROJECT_SOURCE_DIR}/src/i_stackable.cc 
  ${PROJECT_SOURCE_DIR}/src/logical_volume.cc 
  ${PROJECT_SOURCE_DIR}/src/mapping.cc 
  ${PROJECT_SOURCE_DIR}/src/mapping_utils.cc  
  ${PROJECT_SOURCE_DIR}/src/model_factory.cc 
  ${PROJECT_SOURCE_DIR}/src/model_with_internal_items_tools.cc 
  ${PROJECT_SOURCE_DIR}/src/multiple_items_model.cc 
  ${PROJECT_SOURCE_DIR}/src/physical_volume.cc 
  ${PROJECT_SOURCE_DIR}/src/multiple_placement.cc 
  ${PROJECT_SOURCE_DIR}/src/regular_grid_placement.cc 
  ${PROJECT_SOURCE_DIR}/src/regular_linear_placement.cc 
  ${PROJECT_SOURCE_DIR}/src/placement.cc 
  ${PROJECT_SOURCE_DIR}/src/regular_polygon.cc 
  ${PROJECT_SOURCE_DIR}/src/sensitive.cc 
  ${PROJECT_SOURCE_DIR}/src/smart_id_locator.cc 
  ${PROJECT_SOURCE_DIR}/src/helix_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/line_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/polyline_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/circle.cc 
  ${PROJECT_SOURCE_DIR}/src/rectangle.cc 
  ${PROJECT_SOURCE_DIR}/src/disk.cc 
  ${PROJECT_SOURCE_DIR}/src/plate_with_hole_model.cc 
  ${PROJECT_SOURCE_DIR}/src/spherical_extrusion_cylinder_model.cc 
  ${PROJECT_SOURCE_DIR}/src/spherical_extrusion_box_model.cc 
  ${PROJECT_SOURCE_DIR}/src/cylindric_extrusion_boxed_model.cc 
  ${PROJECT_SOURCE_DIR}/src/replicated_boxed_model.cc 
  ${PROJECT_SOURCE_DIR}/src/rotated_boxed_model.cc 
  ${PROJECT_SOURCE_DIR}/src/simple_boxed_model.cc 
  ${PROJECT_SOURCE_DIR}/src/simple_shaped_model.cc 
  ${PROJECT_SOURCE_DIR}/src/simple_world_model.cc 
  ${PROJECT_SOURCE_DIR}/src/stacked_model.cc 
  ${PROJECT_SOURCE_DIR}/src/i_boxed_model.cc
  ${PROJECT_SOURCE_DIR}/src/surrounded_boxed_model.cc 
  ${PROJECT_SOURCE_DIR}/src/cylinder.cc 
  ${PROJECT_SOURCE_DIR}/src/box.cc
  ${PROJECT_SOURCE_DIR}/src/sphere.cc 
  ${PROJECT_SOURCE_DIR}/src/tube.cc 
  ${PROJECT_SOURCE_DIR}/src/tessellation.cc 
  ${PROJECT_SOURCE_DIR}/src/polycone.cc 
  ${PROJECT_SOURCE_DIR}/src/polyhedra.cc 
  ${PROJECT_SOURCE_DIR}/src/subtraction_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/union_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/intersection_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/i_composite_shape_3d.cc
  ${PROJECT_SOURCE_DIR}/src/geomtools_config.cc 
  ${PROJECT_SOURCE_DIR}/src/i_model.cc
  ${PROJECT_SOURCE_DIR}/src/i_shape_1d.cc 
  ${PROJECT_SOURCE_DIR}/src/i_shape_2d.cc 
  ${PROJECT_SOURCE_DIR}/src/i_shape_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/i_object_3d.cc 
  ${PROJECT_SOURCE_DIR}/src/visibility.cc 
  ${PROJECT_SOURCE_DIR}/src/color.cc 
  ${PROJECT_SOURCE_DIR}/src/material.cc 
  ${PROJECT_SOURCE_DIR}/src/utils.cc
)

file ( GLOB geomtools_SOURCES
  ${geomtools_PUBLIC_HEADERS}
  ${geomtools_detail_PUBLIC_HEADERS}
  ${_geomtools_source_files}
  )

### ${PROJECT_SOURCE_DIR}/src/*.cc

# Last source files built in the DLL for proper static stuff initialization:
# list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_model.cc" )
# list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_object_3d.cc" )
# list ( APPEND geomtools_SOURCES      "${PROJECT_SOURCE_DIR}/src/i_model.cc" )
# list ( APPEND geomtools_SOURCES      "${PROJECT_SOURCE_DIR}/src/i_object_3d.cc" )

# remove this because it goes in the BIO DLL if any :
list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/the_serializable.cc" )

# specific to the BIO serialization stuff :
file ( GLOB geomtools_serialization_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/geomtools/serialization/*.h
  ${PROJECT_SOURCE_DIR}/include/geomtools/serialization/*.ipp
  )

file ( GLOB geomtools_ipp_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/geomtools/*.ipp
  )

#----------------------------------------------------------------------------

include_directories ( ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  )

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${mygsl_INCLUDE_DIRS} )

#----------------------------------------------------------------------------
# Add the library
#

add_library ( geomtools SHARED  
  ${geomtools_PUBLIC_HEADERS} 
  ${geomtools_SOURCES}
  )

set ( _link_libraries ${mygsl_LIBRARIES} )

# Link dynamic library :
target_link_libraries ( geomtools
  ${_link_libraries}
  )

# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS geomtools
  EXPORT  geomtools-libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
  )

install ( FILES ${geomtools_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools
  )

install ( FILES ${geomtools_serialization_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools/serialization
  )

install ( FILES ${geomtools_ipp_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools
  )

install ( FILES ${geomtools_detail_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools/detail
  )

# - Bio stuff :
if ( GEOMTOOLS_WITH_BIO ) 

  set ( geomtools_bio_SOURCES 
    ${PROJECT_SOURCE_DIR}/include/geomtools/serialization/the_serializable.ipp
    ${PROJECT_SOURCE_DIR}/include/geomtools/serialization/the_serializable.h
    ${PROJECT_SOURCE_DIR}/src/the_serializable.cc 
    )

  add_library ( geomtools_bio SHARED 
    ${geomtools_PUBLIC_HEADERS} 
    ${geomtools_ipp_PUBLIC_HEADERS} 
    ${geomtools_bio_SOURCES}
    )

  # Link dynamic library :
  target_link_libraries ( geomtools_bio 
    geomtools
    ${_link_libraries}
    )
  
  ###target_link_libraries ( geomtools_bio LINK_INTERFACE_LIBRARIES "" )

  # Install library and associate it with an export
  install ( TARGETS geomtools_bio
    EXPORT  geomtools-libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    )

endif ()

# - end of CMakeLists.txt for geomtools/src subdir
