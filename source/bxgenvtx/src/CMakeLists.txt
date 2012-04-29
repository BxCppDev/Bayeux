# - CMakeLists.txt for genvtx/src subdir

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB genvtx_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genvtx/*.h
  ${PROJECT_BINARY_DIR}/include/genvtx/genvtx_config.h
  )

file ( GLOB genvtx_detail_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genvtx/detail/*.h
  )

file ( GLOB genvtx_SOURCES
  ${genvtx_PUBLIC_HEADERS}
  ${genvtx_detail_PUBLIC_HEADERS}
  ${PROJECT_SOURCE_DIR}/src/i_vertex_generator.cc
  ${PROJECT_SOURCE_DIR}/src/spot_vertex_generator.cc
  ${PROJECT_SOURCE_DIR}/src/from_file_vg.cc
  ${PROJECT_SOURCE_DIR}/src/placement_vg.cc
  ${PROJECT_SOURCE_DIR}/src/box_vg.cc
  ${PROJECT_SOURCE_DIR}/src/cylinder_vg.cc
  ${PROJECT_SOURCE_DIR}/src/tube_vg.cc
  ${PROJECT_SOURCE_DIR}/src/vg_tools.cc
  #${PROJECT_SOURCE_DIR}/src/*.cc
  )

# Last source files built in the DLL for proper static stuff initialization:
list ( REMOVE_ITEM genvtx_SOURCES "${PROJECT_SOURCE_DIR}/src/i_vertex_generator.cc" )
list ( APPEND genvtx_SOURCES "${PROJECT_SOURCE_DIR}/src/i_vertex_generator.cc" )

# if ( GENVTX_WITH_BIO_SUPPORT )
#   file ( GLOB genvtx_serialization_PUBLIC_HEADERS
#          ${PROJECT_SOURCE_DIR}/include/genvtx/serialization/*.h
#        )
#   install ( FILES ${genvtx_serialization_PUBLIC_HEADERS}
#             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genvtx/serialization
#           )
#   file ( GLOB genvtx_ipp_PUBLIC_HEADERS
#          ${PROJECT_SOURCE_DIR}/include/genvtx/serialization/*.ipp
#        )
#   install ( FILES ${genvtx_ipp_PUBLIC_HEADERS}
#             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genvtx/serialization
#           )
# endif ()

#----------------------------------------------------------------------------

include_directories ( 
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  )

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${geomtools_INCLUDE_DIRS} )

#----------------------------------------------------------------------------
# Add the library
#
add_library ( genvtx SHARED  
  ${genvtx_PUBLIC_HEADERS} 
  ${genvtx_SOURCES}
  )

#set ( _link_libraries ${geomtools_LIBRARIES} )
set ( _link_libraries ${geomtools_CORE_LIBRARY} )

# Link dynamic library :
target_link_libraries ( genvtx
  ${_link_libraries}
  )

# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS genvtx
  EXPORT  genvtx-libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
  )

install ( FILES ${genvtx_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genvtx
  )

install ( FILES ${genvtx_detail_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genvtx/detail
  )

# - Bio stuff :
# if ( GENVTX_WITH_BIO_SUPPORT AND GENVTX_WITH_BIO ) 
#    set ( genvtx_bio_SOURCES 
#          ${PROJECT_SOURCE_DIR}/include/genvtx/serialization/the_serializable.ipp
#          ${PROJECT_SOURCE_DIR}/include/genvtx/serialization/the_serializable.h
#          ${PROJECT_SOURCE_DIR}/src/the_serializable.cc 
#        )
#    add_library ( genvtx_bio SHARED 
#                  ${genvtx_PUBLIC_HEADERS} 
#                  ${genvtx_ipp_PUBLIC_HEADERS} 
#                  ${genvtx_bio_SOURCES}
#                )
#    # Link dynamic library :
#    target_link_libraries ( genvtx_bio 
#                            genvtx
#                          )
#    target_link_libraries ( genvtx_bio LINK_INTERFACE_LIBRARIES "" )
#    # Install library and associate it with an export
#    install ( TARGETS genvtx_bio
#              EXPORT  genvtx-libraries
#              RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
#              LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
#              ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
#            )
# endif ()

# - end of CMakeLists.txt for genvtx/src subdir
