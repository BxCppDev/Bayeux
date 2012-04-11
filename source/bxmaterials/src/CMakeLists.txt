# - CMakeLists.txt for materials/src subdir

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB materials_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/materials/*.h
       ${PROJECT_BINARY_DIR}/include/materials/materials_config.h
     )

file ( GLOB materials_detail_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/materials/detail/*.h
     )

file ( GLOB materials_SOURCES
       ${materials_PUBLIC_HEADERS}
       ${materials_detail_PUBLIC_HEADERS}
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

# if ( MATERIALS_WITH_BIO_SUPPORT )
# 
#   file ( GLOB materials_serialization_PUBLIC_HEADERS
#          ${PROJECT_SOURCE_DIR}/include/materials/serialization/*.h
#        )
#
#   install ( FILES ${materials_serialization_PUBLIC_HEADERS}
#             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/materials/serialization
#           )
# 
#   file ( GLOB materials_ipp_PUBLIC_HEADERS
#          ${PROJECT_SOURCE_DIR}/include/materials/serialization/*.ipp
#        )
#
#   install ( FILES ${materials_ipp_PUBLIC_HEADERS}
#             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/materials/serialization
#           )
#
# endif ()

#----------------------------------------------------------------------------

include_directories ( ${PROJECT_BINARY_DIR}/include
		      ${PROJECT_SOURCE_DIR}/include	
                     )

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${geomtools_INCLUDE_DIRS} )

#----------------------------------------------------------------------------
# Add the library
#

add_library ( materials SHARED  
  ${materials_PUBLIC_HEADERS} 
  ${materials_SOURCES}
  )

#set ( _link_libraries ${geomtools_LIBRARIES} )
set ( _link_libraries ${geomtools_CORE_LIBRARY} )

# Link dynamic library :
target_link_libraries ( materials
  ${_link_libraries}
  )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS materials
          EXPORT  materials-libraries
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        )

install ( FILES ${materials_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/materials
        )

install ( FILES ${materials_detail_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/materials/detail
        )

# - Bio stuff :
# if ( MATERIALS_WITH_BIO_SUPPORT AND MATERIALS_WITH_BIO ) 
#
#    set ( materials_bio_SOURCES 
#          ${PROJECT_SOURCE_DIR}/include/materials/serialization/the_serializable.ipp
#          ${PROJECT_SOURCE_DIR}/include/materials/serialization/the_serializable.h
#          ${PROJECT_SOURCE_DIR}/src/the_serializable.cc 
#        )
#
#    add_library ( materials_bio SHARED 
#                  ${materials_PUBLIC_HEADERS} 
#                  ${materials_ipp_PUBLIC_HEADERS} 
#                  ${materials_bio_SOURCES}
#                )
#
#    # Link dynamic library :
#    target_link_libraries ( materials_bio 
#                            materials
#                          )
# 
#    ### target_link_libraries ( materials_bio LINK_INTERFACE_LIBRARIES "" )
#
#    # Install library and associate it with an export
#    install ( TARGETS materials_bio
#              EXPORT  materials-libraries
#              RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
#              LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
#              ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
#            )
#
# endif ()

# - end of CMakeLists.txt for materials/src subdir
