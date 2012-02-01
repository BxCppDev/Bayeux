# - CMakeLists.txt for cuts/src subdir

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB cuts_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/cuts/*.h
       ${PROJECT_SOURCE_DIR}/include/cuts/cuts.h
       ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h
     )

file ( GLOB cuts_SOURCES
       ${PROJECT_SOURCE_DIR}/include/cuts/*.h
       ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

# Last source files built in the DLL for proper static stuff initialization:
list ( REMOVE_ITEM cuts_SOURCES "${PROJECT_SOURCE_DIR}/src/cut_tools.cc" )
list ( APPEND cuts_SOURCES "${PROJECT_SOURCE_DIR}/src/cut_tools.cc" )

# if ( CUTS_WITH_BIO_SUPPORT )
#   file ( GLOB cuts_serialization_PUBLIC_HEADERS
#          ${PROJECT_SOURCE_DIR}/include/cuts/serialization/*.h
#        )
#   install ( FILES ${cuts_serialization_PUBLIC_HEADERS}
#             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cuts/serialization
#           )
#   file ( GLOB cuts_ipp_PUBLIC_HEADERS
#          ${PROJECT_SOURCE_DIR}/include/cuts/serialization/*.ipp
#        )
#   install ( FILES ${cuts_ipp_PUBLIC_HEADERS}
#             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cuts/serialization
#           )
# endif ()

#----------------------------------------------------------------------------

include_directories ( ${PROJECT_BINARY_DIR}/include
		      ${PROJECT_SOURCE_DIR}/include	
                     )

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${datatools_INCLUDE_DIRS} )

#----------------------------------------------------------------------------
# Add the library
#

add_library ( cuts SHARED  
              ${cuts_PUBLIC_HEADERS} 
	      ${cuts_SOURCES}
            )

set ( _link_libraries "" )

set ( _link_libraries "${_link_libraries}" "${datatools_LIBRARIES}" )

# Link dynamic library :
target_link_libraries ( cuts
                        ${_link_libraries}
                      )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS cuts
          EXPORT cuts-libraries
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        )

install ( FILES ${cuts_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cuts
        )

# - Bio stuff :
# if ( CUTS_WITH_BIO_SUPPORT AND CUTS_WITH_BIO ) 
#
#    set ( cuts_bio_SOURCES 
#          ${PROJECT_SOURCE_DIR}/include/cuts/serialization/the_serializable.ipp
#          ${PROJECT_SOURCE_DIR}/include/cuts/serialization/the_serializable.h
#          ${PROJECT_SOURCE_DIR}/src/the_serializable.cc 
#        )
#
#    add_library ( cuts_bio SHARED 
#                  ${cuts_PUBLIC_HEADERS} 
#                  ${cuts_ipp_PUBLIC_HEADERS} 
#                  ${cuts_bio_SOURCES}
#                )
#
#    # Link dynamic library :
#    target_link_libraries ( cuts_bio 
#                            cuts
#                          )
# 
#    target_link_libraries ( cuts_bio LINK_INTERFACE_LIBRARIES "" )
#
#    # Install library and associate it with an export
#    install ( TARGETS cuts_bio
#              EXPORT  cuts-libraries
#              RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
#              LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
#              ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
#            )
#
# endif ()

# - end of CMakeLists.txt for cuts/src subdir
