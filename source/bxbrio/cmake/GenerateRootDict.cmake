# - cmake file for ROOT dcitionnary generation from the  brio/src subdir

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

set ( _generate_root_dict 0 )
if (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME MATCHES Darwin)
  set ( _generate_root_dict 1 )
endif ()

if ( _generate_root_dict )
    ADD_CUSTOM_COMMAND(OUTPUT ${_brio_dict_sources}
       COMMAND SOURCE_PATH=${PROJECT_SOURCE_DIR}
               BINARY_PATH=${PROJECT_BINARY_DIR}
               ${PROJECT_SOURCE_DIR}/cmake/Tools/make_dict.sh --rootcint ${ROOT_CINT_EXECUTABLE}
       DEPENDS ${_brio_dict_headers}
       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
       COMMENT "Generate brio ROOT dictionary"
       )
    ADD_CUSTOM_TARGET (generate_brio_dict ALL
    		       SOURCES ${_brio_dict_sources} )
    message ( STATUS "Embedded ROOT dictionnary : OK" )
else ()
     message (FATAL_ERROR "Cannot generate the embedded ROOT dictionnary")
endif ()


# end
