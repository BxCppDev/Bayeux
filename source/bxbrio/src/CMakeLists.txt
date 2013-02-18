# - CMakeLists.txt for brio/src subdir

message (STATUS "DEVEL: GenerateRootDict...")
include(GenerateRootDict)
message (STATUS "DEVEL: GenerateRootDict is done.")

#-----------------------------------------------------------------------
# function list_prepend(<list> <value>)
#          Prepend value to each element in list, in place
function(list_prepend _list _value)
  set(_tmp_list)
  foreach(_elem ${${_list}})
    list(APPEND _tmp_list "${_value}${_elem}")
  endforeach()
  set(${_list} ${_tmp_list} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources,
# public headers, private headers and resources.
#-----------------------------------------------------------------------
# PUBLIC HEADERS
# Because we're in the awkward position of separate headers and sources,
# use a simple listing and then prepend the actual source dir root

set(brio_PUBLIC_HEADERS
  include/brio/brio.h
  include/brio/reader.h
  include/brio/utils.h
  include/brio/writer.h
  )
list_prepend(brio_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/brio/brio_config.h.in
  ${PROJECT_BINARY_DIR}/include/brio/brio_config.h
  )
list(APPEND brio_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/brio/brio_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS
## - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(brio_detail_HEADERS
  include/brio/detail/base_io.h
  include/brio/detail/brio_record.h
  include/brio/detail/TArrayCMod.h
  )
list_prepend(brio_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${brio_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if brio 
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail)
    install(FILES ${brio_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brio/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()


#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(brio_SOURCES
  base_io.cc
  brio_record.cc
  reader.cc
  TArrayCMod.cc
  utils.cc
  writer.cc
  ${_brio_dict_sources} 
  )

#-----------------------------------------------------------------------
# CMAKE RESOURCE FILES
# We do this here because it allows us to to add the config files as
# resources on Mac. This allows them to be easily imported into the
# Framework with minimal platform specific commands by adding them
# as sources of the framework/library.
# We have to deal with the library depends part of this later because 
# their generation require the targets to be present...
#
if(APPLE)
  set(TO_BRIO_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_BRIO_INCLUDEDIR 
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/brio-${brio_VERSION}" 
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(brio_RESOURCES
  cmake/brio-config.cmake
  cmake/brio-config-version.cmake
  )

foreach(_brioresource ${brio_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_brioresource}.in 
    ${PROJECT_BINARY_DIR}/${_brioresource}
    @ONLY
    )
  list(APPEND brio_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_brioresource}.in)
  list(APPEND brio_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_brioresource})
endforeach()

set_source_files_properties(${brio_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${brio_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/brio-${brio_VERSION}"
    COMPONENT Development
    )
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/brio-config.in 
    ${PROJECT_BINARY_DIR}/cmake/brio-config
    @ONLY
    )

install(FILES ${PROJECT_BINARY_DIR}/cmake/brio-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS OWNER_READ OWNER_EXECUTE 
    		GROUP_READ GROUP_EXECUTE 
                WORLD_READ WORLD_EXECUTE    
    COMPONENT Development
    )

if ( BRIO_WITH_BASH_SETUP )
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/brio_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/brio_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/brio_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
  )
endif ()

#-----------------------------------------------------------------------
# BUILD THE BRIO LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  ${ROOT_INCLUDE_DIR}
  ${datatools_INCLUDE_DIRS}
  )

add_library(brio SHARED  
  ${brio_PUBLIC_HEADERS} 
  ${brio_detail_HEADERS}
  ${brio_SOURCES}
  ${brio_RESOURCES_OUTPUTS}
  )

if ( _generate_root_dict )
  message ( STATUS "brio depends on generate_brio_dict...")
  add_dependencies ( brio generate_brio_dict )
endif ()

set_target_properties(brio
  PROPERTIES 
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.brio
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${brio_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${brio_VERSION}
    VERSION                               ${brio_VERSION}
    SOVERSION                             ${brio_VERSION_MAJOR}
    PUBLIC_HEADER                         "${brio_PUBLIC_HEADERS}"
  )

target_link_libraries(brio ${datatools_LIBRARIES} ${ROOT_LIBRARIES} ${CMAKE_DL_LIBS})

set(BRIO_TARGETS brio)

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature 
install(TARGETS ${BRIO_TARGETS}
  EXPORT        brio-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR} 
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${BRIO_FRAMEWORK_LOCATION} 
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brio 
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT brio-libraries 
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT brio-libraries 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/brio-${brio_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for brio/src subdir
