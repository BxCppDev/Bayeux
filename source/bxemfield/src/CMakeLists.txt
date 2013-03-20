# - CMakeLists.txt for emfield/src subdir

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

set(emfield_PUBLIC_HEADERS
    include/emfield/base_electromagnetic_field.h
    include/emfield/electromagnetic_field_macros.h
    include/emfield/electromagnetic_field_manager.h
    include/emfield/linear_combination_field.h
    include/emfield/oscillating_field.h
    include/emfield/placement_field.h
    include/emfield/uniform_electric_field.h
    include/emfield/uniform_magnetic_field.h
    include/emfield/emfield_geom_plugin.h
    include/emfield/geom_map.h
  )
list_prepend(emfield_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/emfield/emfield_config.h.in
  ${PROJECT_BINARY_DIR}/include/emfield/emfield_config.h
  )
list(APPEND emfield_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/emfield/emfield_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(emfield_detail_HEADERS
  )
list_prepend(emfield_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${emfield_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if emfield 
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail)
    install(FILES ${emfield_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/emfield/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(emfield_SOURCES
    base_electromagnetic_field.cc
    electromagnetic_field_manager.cc
    linear_combination_field.cc
    oscillating_field.cc
    placement_field.cc
    uniform_electric_field.cc
    uniform_magnetic_field.cc
    emfield_geom_plugin.cc
    geom_map.cc
  )

# message(STATUS "********** emfield_SOURCES=${emfield_SOURCES}")

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
  set(TO_EMFIELD_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_EMFIELD_INCLUDEDIR 
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/emfield-${emfield_VERSION}" 
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(emfield_RESOURCES
  cmake/emfield-config.cmake
  cmake/emfield-config-version.cmake
  )

foreach(_dtresource ${emfield_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in 
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND emfield_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND emfield_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${emfield_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${emfield_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/emfield-${emfield_VERSION}"
    COMPONENT Development
    )
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/emfield-config.in 
    ${PROJECT_BINARY_DIR}/cmake/emfield-config
    @ONLY
    )

install(FILES ${PROJECT_BINARY_DIR}/cmake/emfield-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS OWNER_READ OWNER_EXECUTE 
    		GROUP_READ GROUP_EXECUTE 
                WORLD_READ WORLD_EXECUTE    
    COMPONENT Development
    )

if ( EMFIELD_WITH_BASH_SETUP )
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/emfield_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/emfield_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/emfield_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
  )
endif ()

#-----------------------------------------------------------------------
# BUILD THE EMFIELD LIBRARY
#
#message(STATUS "XXXXXXXXXXXXXXXXXx mygsl_INCLUDE_DIRS=${mygsl_INCLUDE_DIRS}")
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  ${geomtools_INCLUDE_DIRS}
  )

if ( EMFIELD_WITH_REFLECTION )
  include_directories(
    ${CAMP_INCLUDE_DIR}
    )
endif ()

add_library(emfield SHARED  
  ${emfield_PUBLIC_HEADERS} 
  ${emfield_detail_HEADERS}
  ${emfield_SOURCES}
  ${emfield_RESOURCES_OUTPUTS}
  )

set_target_properties(emfield
  PROPERTIES 
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.emfield
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${emfield_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${emfield_VERSION}
    VERSION                               ${emfield_VERSION}
    SOVERSION                             ${emfield_VERSION_MAJOR}
    PUBLIC_HEADER                         "${emfield_PUBLIC_HEADERS}"
  )

if ( EMFIELD_WITH_REFLECTION )
  target_link_libraries(emfield ${geomtools_LIBRARIES} ${CAMP_LIBRARIES} ${CMAKE_DL_LIBS})
else ()
  target_link_libraries(emfield ${geomtools_LIBRARIES} ${CMAKE_DL_LIBS})
endif ()

set(EMFIELD_TARGETS emfield)

#-----------------------------------------------------------------------
# BUILD THE EMFIELD BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
# if(EMFIELD_WITH_BIO)
#   set(emfield_bio_SOURCES
#     include/emfield/the_serializable.h
#     include/emfield/the_serializable.ipp
#     the_serializable.cc
#     )
#   list_prepend(emfield_bio_SOURCES "${PROJECT_SOURCE_DIR}/")
  
#   add_library(emfield_bio SHARED ${emfield_bio_SOURCES})

#   if(APPLE)
#     set_property(TARGET emfield_bio APPEND
#       PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
#       )
#   endif()

#   list(APPEND EMFIELD_TARGETS emfield_bio)
# endif()

#-----------------------------------------------------------------------
# BUILD THE EMFIELD REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(EMFIELD_WITH_REFLECTION)
  set(emfield_reflection_SOURCES
    #include/emfield/*-reflect.h
    #include/emfield/the_introspectable.h
    #the_introspectable.cc
    )
  list_prepend(emfield_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")
  
  add_library(emfield_reflection SHARED ${emfield_reflection_SOURCES})

  if(APPLE)
    set_property(TARGET emfield_reflection APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND EMFIELD_TARGETS emfield_reflection)
endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature 
install(TARGETS ${EMFIELD_TARGETS}
  EXPORT        emfield-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR} 
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${EMFIELD_FRAMEWORK_LOCATION} 
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/emfield 
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT emfield-libraries 
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT emfield-libraries 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/emfield-${emfield_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for emfield/src subdir
