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
       ${PROJECT_SOURCE_DIR}/include/geomtools/geomtools.h
       ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
     )

file ( GLOB geomtools_SOURCES
       ${PROJECT_SOURCE_DIR}/include/geomtools/*.h
       ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

# Last source files built in the DLL for proper static stuff initialization:
list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_model.cc" )
list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_object_3d.cc" )
list ( APPEND geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_model.cc" )
list ( APPEND geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_object_3d.cc" )

# remove this because it goes in the BIO DLL :
list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/the_serializable.cc" )
  
# specific to the BIO serialization stuff :
file ( GLOB geomtools_serialization_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/geomtools/serialization/*.h
       ${PROJECT_SOURCE_DIR}/include/geomtools/serialization/*.ipp
     )

install ( FILES ${geomtools_serialization_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools/serialization
        )

file ( GLOB geomtools_ipp_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/geomtools/*.ipp
     )

install ( FILES ${geomtools_ipp_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools
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

set ( _link_libraries ${_link_libraries} ${mygsl_LIBRARIES} )

# Link dynamic library :
target_link_libraries ( geomtools
                        ${_link_libraries}
                      )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS geomtools
          EXPORT geomtools-libraries
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        )

install ( FILES ${geomtools_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geomtools
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
                         )
 
   target_link_libraries ( geomtools_bio LINK_INTERFACE_LIBRARIES "" )

   # Install library and associate it with an export
   install ( TARGETS geomtools_bio
             EXPORT  geomtools-libraries
             RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
             ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
           )

endif ()

# - end of CMakeLists.txt for geomtools/src subdir
