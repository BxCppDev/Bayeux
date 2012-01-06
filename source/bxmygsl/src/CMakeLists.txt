# - CMakeLists.txt for mygsl/src subdir


set (_ld_library_path_env "LD_LIBRARY_PATH" )
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
  set (_ld_library_path_env "DYLD_LIBRARY_PATH" )
endif (CMAKE_SYSTEM_NAME MATCHES Darwin)
###message ( STATUS "DEVEL ***** ${_ld_library_path_env}" )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file(GLOB mygsl_detail_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/mygsl/detail/*.h
)
 
file(GLOB mygsl_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/mygsl/*.h
  ${PROJECT_SOURCE_DIR}/include/mygsl/mygsl.h
  ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
 )

file (GLOB mygsl_SOURCES
  ${PROJECT_SOURCE_DIR}/include/mygsl/detail/*.h
  ${PROJECT_SOURCE_DIR}/include/mygsl/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cc
  )

#----------------------------------------------------------------------------
# Dependencies
#

include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                      ${DATATOOLS_INCLUDE_DIRS_DEPEND}
  		      ${GSL_INCLUDE_DIR}
                  )

#----------------------------------------------------------------------------
# Add the library
#
###message ( STATUS "DEVEL ************** mygsl_SOURCES='${mygsl_SOURCES}'" )

add_library (mygsl SHARED  
		      ${mygsl_PUBLIC_HEADERS} 
		      ${mygsl_detail_PUBLIC_HEADERS} 
		      ${mygsl_SOURCES}
            )

# Link dynamic library :

target_link_libraries ( mygsl
                        ${GSL_LIBRARIES}
                        ${DATATOOLS_LIBRARIES_DEPEND}
                      )
# ----------------------------------------------------------------------
# Installation
#

install(TARGETS mygsl
    RUNTIME DESTINATION bin COMPONENT bin
    LIBRARY DESTINATION lib COMPONENT bin 
    ARCHIVE DESTINATION lib COMPONENT devel
)

install (FILES ${mygsl_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/mygsl
        )

install (FILES ${mygsl_detail_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/mygsl/detail
        )

# - end of CMakeLists.txt for mygsl/src subdir
