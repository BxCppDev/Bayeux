# - CMakeLists.txt for brio/src subdir

include ( GenerateRootDict )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB brio_detail_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/brio/detail/*.h
     )

file ( GLOB brio_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/brio/*.h
       ${PROJECT_SOURCE_DIR}/include/brio/brio.h
       ${PROJECT_BINARY_DIR}/include/brio/brio_config.h
     )

file ( GLOB brio_SOURCES
       ${brio_detail_PUBLIC_HEADERS}
       ${brio_PUBLIC_HEADERS}
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

set (brio_SOURCES ${brio_SOURCES} ${_brio_dict_sources} )
### message ( STATUS "DEVEL brio_SOURCES='${brio_SOURCES}'" )

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${PROJECT_BINARY_DIR}/include
		      ${PROJECT_SOURCE_DIR}/include	
                      ${datatools_INCLUDE_DIRS}
  		      ${ROOT_INCLUDE_DIRS}
                    )

#----------------------------------------------------------------------------
# Add the library
#

add_library ( brio SHARED  
              ${brio_PUBLIC_HEADERS} 
              ${brio_detail_PUBLIC_HEADERS} 
	      ${brio_SOURCES}
            )
if ( _generate_root_dict )
  message ( STATUS "brio depends on generate_brio_dict...")
  add_dependencies ( brio generate_brio_dict )
endif ()

# Link dynamic library :
target_link_libraries ( brio
                        ${datatools_LIBRARIES} 
                        ${ROOT_LIBRARIES}
                      )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS brio
          EXPORT  brio-libraries
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        )

install ( FILES ${brio_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brio
        )

install ( FILES ${brio_detail_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brio/detail
        )

## - end of CMakeLists.txt for brio/src subdir
