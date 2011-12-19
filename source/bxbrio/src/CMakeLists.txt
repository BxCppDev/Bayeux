# - CMakeLists.txt for brio/src subdir

# ----------------------------------------------------------------------
# Embedded ROOT dictionnary
#

message ( STATUS "Embedded ROOT dictionnary" )
set ( _brio_dict_headers 
      ${PROJECT_SOURCE_DIR}/include/brio/detail/TArrayCMod.h
      ${PROJECT_SOURCE_DIR}/include/brio/detail/brio_record.h
    )
set ( _brio_dict_sources 
      ${PROJECT_BINARY_DIR}/src/brio_dict.cc
      ${PROJECT_BINARY_DIR}/src/brio_dict.h
    )
message ( STATUS "DEVEL _brio_dict_sources='${_brio_dict_sources}'" )

set (_ld_library_path_env "LD_LIBRARY_PATH" )
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
  set (_ld_library_path_env "DYLD_LIBRARY_PATH" )
endif (CMAKE_SYSTEM_NAME MATCHES Darwin)
message ( STATUS "DEVEL ***** ${_ld_library_path_env}" )

set ( _generate_root_dict OFF )
if (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME MATCHES Darwin)
  set ( _generate_root_dict ON )
endif ()

if ( _generate_root_dict )
    message ( STATUS "ADD_CUSTOM_COMMAND ****" )
    ADD_CUSTOM_COMMAND(OUTPUT ${_brio_dict_sources}
       COMMAND ${_ld_library_path_env}=${ROOT_LIBRARY_DIR} 
               ROOTSYS=${ROOTSYS} 
               SOURCE_PATH=${PROJECT_SOURCE_DIR} 
               BINARY_PATH=${PROJECT_BINARY_DIR} 
               ${PROJECT_SOURCE_DIR}/tools/make_dict.sh
       DEPENDS ${_brio_dict_headers}
       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
       COMMENT "Generate brio ROOT dictionary"
       )
    ADD_CUSTOM_TARGET (generate_brio_dict ALL
    		       SOURCES ${_brio_dict_sources} )
endif ()

message ( STATUS "Embedded ROOT dictionnary **** DONE" )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file(GLOB brio_detail_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/brio/detail/*.h
)
 
file(GLOB brio_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/brio/*.h
  ${PROJECT_SOURCE_DIR}/include/brio/brio.h
  ${PROJECT_BINARY_DIR}/include/brio/brio_config.h
 )

file (GLOB brio_SOURCES
  ${PROJECT_SOURCE_DIR}/include/brio/detail/*.h
  ${PROJECT_SOURCE_DIR}/include/brio/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cc
  )
set ( brio_SOURCES ${brio_SOURCES} ${_brio_dict_sources} )

#----------------------------------------------------------------------------
# Dependencies
#

#message ( STATUS "DEVEL ************** Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}" )
include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                      ${DATATOOLS_INCLUDE_DIRS_DEPEND}
  		      ${ROOT_INCLUDE_DIR}
                  )

#----------------------------------------------------------------------------
# Add the library
#
message ( STATUS "DEVEL ************** brio_SOURCES='${brio_SOURCES}'" )

add_library (brio SHARED  
		      ${brio_PUBLIC_HEADERS} 
		      ${brio_detail_PUBLIC_HEADERS} 
		      ${brio_SOURCES}
            )
if ( _generate_root_dict )
  add_dependencies ( brio generate_brio_dict )
endif ()

# Link dynamic library :

target_link_libraries ( brio
                        ${ROOT_LIBRARIES}
                        ${DATATOOLS_LIBRARIES_DEPEND}
                      )
# ----------------------------------------------------------------------
# Installation
#

install(TARGETS brio
    RUNTIME DESTINATION bin COMPONENT bin
    LIBRARY DESTINATION lib COMPONENT bin 
    ARCHIVE DESTINATION lib COMPONENT devel
)

install (FILES ${brio_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/brio
        )

install (FILES ${brio_detail_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/brio/detail
        )

# - end of CMakeLists.txt for brio/src subdir
