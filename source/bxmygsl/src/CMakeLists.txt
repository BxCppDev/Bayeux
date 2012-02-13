# - CMakeLists.txt for mygsl/src subdir

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB mygsl_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/mygsl/*.h
       ${PROJECT_SOURCE_DIR}/include/mygsl/mygsl.h
       ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
     )

file ( GLOB mygsl_SOURCES
       ${PROJECT_SOURCE_DIR}/include/mygsl/*.h
       ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

if ( MYGSL_WITH_BIO_SUPPORT )
 
  file ( GLOB mygsl_serialization_PUBLIC_HEADERS
         ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/*.h
       )

  install ( FILES ${mygsl_serialization_PUBLIC_HEADERS}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl/serialization
          )
 
  file ( GLOB mygsl_ipp_PUBLIC_HEADERS
         ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/*.ipp
       )

  install ( FILES ${mygsl_ipp_PUBLIC_HEADERS}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl/serialization
          )

endif ()

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${PROJECT_BINARY_DIR}/include
		      ${PROJECT_SOURCE_DIR}/include
  		      ${GSL_INCLUDE_DIRS}	
                     )

if ( _depends_on_datatools ) 
  include_directories ( ${datatools_INCLUDE_DIRS} )
endif ()

#----------------------------------------------------------------------------
# Add the library
#
###message ( STATUS "DEVEL: mygsl_SOURCES='${mygsl_SOURCES}'" )

add_library ( mygsl SHARED  
              ${mygsl_PUBLIC_HEADERS} 
	      ${mygsl_SOURCES}
            )

set ( _link_libraries "" )

if ( _depends_on_datatools ) 
  set ( _link_libraries ${_link_libraries} ${datatools_LIBRARIES} )
endif ()
set ( _link_libraries ${_link_libraries} ${GSL_LIBRARIES} )

# Link dynamic library :
target_link_libraries ( mygsl
                        ${_link_libraries}
                      )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS mygsl
          EXPORT  mygsl-libraries
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        )

install ( FILES ${mygsl_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl
        )

# # - Bio stuff :
# if ( MYGSL_WITH_BIO_SUPPORT AND MYGSL_WITH_BIO ) 
#
#    set ( mygsl_bio_SOURCES 
#          ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/the_serializable.ipp
#          ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/the_serializable.h
#          ${PROJECT_SOURCE_DIR}/src/the_serializable.cc 
#        )
#
#    add_library ( mygsl_bio SHARED 
#                  ${mygsl_PUBLIC_HEADERS} 
#                  ${mygsl_ipp_PUBLIC_HEADERS} 
#                  ${mygsl_bio_SOURCES}
#                )
#
#    # Link dynamic library :
#    target_link_libraries ( mygsl_bio 
#                            mygsl
#                          )
#
#    target_link_libraries ( mygsl_bio LINK_INTERFACE_LIBRARIES "" )
#
#    # Install library and associate it with an export
#    install ( TARGETS mygsl_bio
#              EXPORT  mygsl-libraries
#              RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
#              LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
#              ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
#            )
#
# endif ()

# - end of CMakeLists.txt for mygsl/src subdir
