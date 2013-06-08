# - CMakeLists.txt for materials/src subdir

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

# file ( GLOB materials_PUBLIC_HEADERS
#   ${PROJECT_SOURCE_DIR}/include/materials/*.h
#   ${PROJECT_BINARY_DIR}/include/materials/materials_config.h
#   )

set(materials_PUBLIC_HEADERS
include/materials/chemical_symbol.h
include/materials/element.h
include/materials/factory.h
include/materials/isotope.h
include/materials/manager.h
include/materials/material.h
  )
list_prepend(materials_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/materials/materials_config.h.in
  ${PROJECT_BINARY_DIR}/include/materials/materials_config.h
  )
list(APPEND materials_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/materials/materials_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(materials_detail_HEADERS
  include/materials/detail/tools.h
  )
list_prepend(materials_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${materials_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if materials
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail)
    install(FILES ${materials_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/materials/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(materials_SOURCES
    chemical_symbol.cc
    isotope.cc
    material.cc
    element.cc
    factory.cc
    manager.cc
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
  set(TO_MATERIALS_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_MATERIALS_INCLUDEDIR
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/materials-${materials_VERSION}"
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(materials_RESOURCES
  cmake/materials-config.cmake
  cmake/materials-config-version.cmake
  )

foreach(_dtresource ${materials_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND materials_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND materials_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${materials_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${materials_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/materials-${materials_VERSION}"
    COMPONENT Development
    )
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/materials-config.in
    ${PROJECT_BINARY_DIR}/cmake/materials-config
    @ONLY
    )

install(FILES ${PROJECT_BINARY_DIR}/cmake/materials-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    		GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

if ( MATERIALS_WITH_BASH_SETUP )
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/materials_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/materials_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/materials_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
  )
endif ()

#-----------------------------------------------------------------------
# BUILD THE MATERIALS LIBRARY
#

include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${datatools_INCLUDE_DIRS}
  )

add_library(materials SHARED
  ${materials_PUBLIC_HEADERS}
  ${materials_detail_HEADERS}
  ${materials_SOURCES}
  ${materials_RESOURCES_OUTPUTS}
  )

set_target_properties(materials
  PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.materials
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${materials_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${materials_VERSION}
    VERSION                               ${materials_VERSION}
    SOVERSION                             ${materials_VERSION_MAJOR}
    PUBLIC_HEADER                         "${materials_PUBLIC_HEADERS}"
  )

target_link_libraries(materials
                      ${datatools_LIBRARIES}
                      ${CLHEP_LIBRARIES}
                      ${CMAKE_DL_LIBS})

set(MATERIALS_TARGETS materials)

#-----------------------------------------------------------------------
# BUILD THE MATERIALS BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
# if(MATERIALS_WITH_BIO)
#   set(materials_bio_SOURCES
#     include/materials/the_serializable.h
#     include/materials/the_serializable.ipp
#     the_serializable.cc
#     )
#   list_prepend(materials_bio_SOURCES "${PROJECT_SOURCE_DIR}/")
#
#   add_library(materials_bio SHARED ${materials_bio_SOURCES})
#
#   if(APPLE)
#     set_property(TARGET materials_bio APPEND
#       PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
#       )
#   endif()
#
#   list(APPEND MATERIALS_TARGETS materials_bio)
# endif()

#-----------------------------------------------------------------------
# BUILD THE MATERIALS REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
# if(MATERIALS_WITH_REFLECTION)
#   set(materials_reflection_SOURCES
#     #include/materials/*-reflect.h
#     #include/materials/the_introspectable.h
#     #the_introspectable.cc
#     )
#   list_prepend(materials_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")
#
#   add_library(materials_reflection SHARED ${materials_reflection_SOURCES})
#
#   if(APPLE)
#     set_property(TARGET materials_reflection APPEND
#       PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
#       )
#   endif()
#
#   list(APPEND MATERIALS_TARGETS materials_reflection)
# endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature
install(TARGETS ${MATERIALS_TARGETS}
  EXPORT        materials-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${MATERIALS_FRAMEWORK_LOCATION}
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/materials
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT materials-libraries
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT materials-libraries
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/materials-${materials_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for materials/src subdir
