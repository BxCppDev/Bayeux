# - CMakeLists.txt for materials/src subdir

set (_ld_library_path_env "LD_LIBRARY_PATH" )
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
  set (_ld_library_path_env "DYLD_LIBRARY_PATH" )
endif (CMAKE_SYSTEM_NAME MATCHES Darwin)
#message ( STATUS "DEVEL ***** ${_ld_library_path_env}" )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB materials_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/materials/*.h
       ${PROJECT_SOURCE_DIR}/include/materials/materials.h
       ${PROJECT_BINARY_DIR}/include/materials/materials_config.h
     )

file ( GLOB materials_SOURCES
       ${PROJECT_SOURCE_DIR}/include/materials/*.h
       ${PROJECT_BINARY_DIR}/include/materials/materials_config.h
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

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
###message ( STATUS "DEVEL: materials_SOURCES='${materials_SOURCES}'" )

add_library ( materials SHARED  
              ${materials_PUBLIC_HEADERS} 
	      ${materials_SOURCES}
            )

# Link dynamic library :
target_link_libraries ( materials
                        ${GEOMTOOLS_LIBRARIES_DEPEND}
                      )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS materials
          RUNTIME DESTINATION bin COMPONENT bin
          LIBRARY DESTINATION lib COMPONENT bin 
          ARCHIVE DESTINATION lib COMPONENT devel
        )

install ( FILES ${materials_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/materials
        )

# - end of CMakeLists.txt for materials/src subdir
