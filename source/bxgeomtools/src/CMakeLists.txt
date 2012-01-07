# - CMakeLists.txt for geomtools/src subdir


set (_ld_library_path_env "LD_LIBRARY_PATH" )
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
  set (_ld_library_path_env "DYLD_LIBRARY_PATH" )
endif (CMAKE_SYSTEM_NAME MATCHES Darwin)
###message ( STATUS "DEVEL ***** ${_ld_library_path_env}" )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file(GLOB geomtools_detail_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/geomtools/detail/*.h
)
 
file(GLOB geomtools_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/geomtools/*.h
  ${PROJECT_SOURCE_DIR}/include/geomtools/geomtools.h
  ${PROJECT_BINARY_DIR}/include/geomtools/geomtools_config.h
 )

file (GLOB geomtools_SOURCES
  ${PROJECT_SOURCE_DIR}/include/geomtools/detail/*.h
  ${PROJECT_SOURCE_DIR}/include/geomtools/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cc
  )

# remove this because it go in the BIO DLL :
list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/the_serializable.cc" )

# Last source files built in the DLL for proper static stuff initialization:
list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_model.cc" )
list ( REMOVE_ITEM geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_object_3d.cc" )
list ( APPEND geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_model.cc" )
list ( APPEND geomtools_SOURCES "${PROJECT_SOURCE_DIR}/src/i_object_3d.cc" )

#message ( AUTHOR_WARNING "datatools_SOURCES=${datatools_SOURCES}" )

if ( GEOMTOOLS_WITH_BIO )
  set ( geomtools_bio_SOURCES 
        ${PROJECT_SOURCE_DIR}/include/geomtools/the_serializable.ipp
        ${PROJECT_SOURCE_DIR}/include/geomtools/the_serializable.h
        ${PROJECT_SOURCE_DIR}/src/the_serializable.cc )
  #message ( AUTHOR_WARNING "geomtools_bio_SOURCES=${geomtools_bio_SOURCES}" )
endif ( GEOMTOOLS_WITH_BIO )

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                      ${MYGSL_INCLUDE_DIRS_DEPEND}
                      ${DATATOOLS_INCLUDE_DIRS_DEPEND}
                  )

#----------------------------------------------------------------------------
# Add the library
#
###message ( STATUS "DEVEL ************** geomtools_SOURCES='${geomtools_SOURCES}'" )

add_library (geomtools SHARED  
		      ${geomtools_PUBLIC_HEADERS} 
		      ${geomtools_detail_PUBLIC_HEADERS} 
		      ${geomtools_SOURCES}
            )

# Link dynamic library :

target_link_libraries ( geomtools
                        ${MYGSL_LIBRARIES_DEPEND}
                        ${DATATOOLS_LIBRARIES_DEPEND}
                      )
# ----------------------------------------------------------------------
# Installation
#

install(TARGETS geomtools
    RUNTIME DESTINATION bin COMPONENT bin
    LIBRARY DESTINATION lib COMPONENT bin 
    ARCHIVE DESTINATION lib COMPONENT devel
)

install (FILES ${geomtools_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/geomtools
        )

install (FILES ${geomtools_detail_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/geomtools/detail
        )

if ( GEOMTOOLS_WITH_BIO )

   add_library( geomtools_bio SHARED 
                ${geomtools_PUBLIC_HEADERS} 
                ${geomtools_detail_PUBLIC_HEADERS} 
                ${geomtools_bio_SOURCES}
              )

   # Link dynamic library :
   target_link_libraries ( geomtools_bio
                           ${MYGSL_LIBRARIES_DEPEND}
                           ${DATATOOLS_LIBRARIES_DEPEND}
                         )

   target_link_libraries ( geomtools_bio LINK_INTERFACE_LIBRARIES "" )

   install ( TARGETS geomtools_bio
             RUNTIME DESTINATION bin COMPONENT bin
             LIBRARY DESTINATION lib COMPONENT bin 
             ARCHIVE DESTINATION lib COMPONENT devel
           )

endif ( GEOMTOOLS_WITH_BIO )

# - end of CMakeLists.txt for geomtools/src subdir
