# - CMakeLists.txt for genvtx/src subdir

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB genvtx_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/genvtx/*.h
       ${PROJECT_SOURCE_DIR}/include/genvtx/genvtx.h
       ${PROJECT_BINARY_DIR}/include/genvtx/genvtx_config.h
     )

file ( GLOB genvtx_SOURCES
       ${PROJECT_SOURCE_DIR}/include/genvtx/*.h
       ${PROJECT_BINARY_DIR}/include/genvtx/genvtx_config.h
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

# Last source files built in the DLL for proper static stuff initialization:
list ( REMOVE_ITEM genvtx_SOURCES "${PROJECT_SOURCE_DIR}/src/i_vertex_generator.cc" )
list ( APPEND genvtx_SOURCES "${PROJECT_SOURCE_DIR}/src/i_vertex_generator.cc" )

include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                     )

#----------------------------------------------------------------------------
# Dependencies
#
include_directories ( ${GEOMTOOLS_INCLUDE_DIRS_DEPEND} )

#----------------------------------------------------------------------------
# Add the library
#
message ( STATUS "DEVEL: genvtx_SOURCES='${genvtx_SOURCES}'" )

add_library ( genvtx SHARED  
              ${genvtx_PUBLIC_HEADERS} 
	      ${genvtx_SOURCES}
            )

# Link dynamic library :
target_link_libraries ( genvtx
                        ${GEOMTOOLS_LIBRARIES_DEPEND}
                      )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS genvtx
          RUNTIME DESTINATION bin COMPONENT bin
          LIBRARY DESTINATION lib COMPONENT bin 
          ARCHIVE DESTINATION lib COMPONENT devel
        )

install ( FILES ${genvtx_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/genvtx
        )

# - end of CMakeLists.txt for genvtx/src subdir
