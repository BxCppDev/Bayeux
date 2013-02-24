# - CMakeLists.txt for genvtx/src subdir

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

set(genvtx_PUBLIC_HEADERS
  include/genvtx/box_vg.h
  include/genvtx/cylinder_vg.h
  include/genvtx/from_file_vg.h
  include/genvtx/i_vertex_generator.h
  include/genvtx/placement_vg.h
  include/genvtx/quadrangle_random_tools.h
  include/genvtx/spot_vertex_generator.h
  include/genvtx/triangle_random_tools.h
  include/genvtx/tube_vg.h
  include/genvtx/vg_macros.h
  )
list_prepend(genvtx_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/genvtx/genvtx_config.h.in
  ${PROJECT_BINARY_DIR}/include/genvtx/genvtx_config.h
  )
list(APPEND genvtx_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/genvtx/genvtx_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(genvtx_detail_HEADERS
  include/genvtx/detail/vg_tools.h
  )
list_prepend(genvtx_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${genvtx_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if genvtx 
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail)
    install(FILES ${genvtx_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genvtx/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(genvtx_SOURCES
   #factory.cc
   #manager.cc
   i_vertex_generator.cc
   spot_vertex_generator.cc
   from_file_vg.cc
   placement_vg.cc
   box_vg.cc
   cylinder_vg.cc
   tube_vg.cc
   vg_tools.cc
   triangle_random_tools.cc
   quadrangle_random_tools.cc
  )

# message(STATUS "********** genvtx_SOURCES=${genvtx_SOURCES}")

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
  set(TO_GENVTX_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_GENVTX_INCLUDEDIR 
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/genvtx-${genvtx_VERSION}" 
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(genvtx_RESOURCES
  cmake/genvtx-config.cmake
  cmake/genvtx-config-version.cmake
  )

foreach(_dtresource ${genvtx_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in 
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND genvtx_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND genvtx_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${genvtx_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${genvtx_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/genvtx-${genvtx_VERSION}"
    COMPONENT Development
    )
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/genvtx-config.in 
    ${PROJECT_BINARY_DIR}/cmake/genvtx-config
    @ONLY
    )

install(FILES ${PROJECT_BINARY_DIR}/cmake/genvtx-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS OWNER_READ OWNER_EXECUTE 
    		GROUP_READ GROUP_EXECUTE 
                WORLD_READ WORLD_EXECUTE    
    COMPONENT Development
    )

if ( GENVTX_WITH_BASH_SETUP )
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/genvtx_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/genvtx_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/genvtx_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
  )
endif ()

#-----------------------------------------------------------------------
# BUILD THE GENVTX LIBRARY
#
#message(STATUS "XXXXXXXXXXXXXXXXXx mygsl_INCLUDE_DIRS=${mygsl_INCLUDE_DIRS}")
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  ${geomtools_INCLUDE_DIRS}
  )

if ( GENVTX_WITH_REFLECTION )
  include_directories(
    ${CAMP_INCLUDE_DIR}
    )
endif ()

add_library(genvtx SHARED  
  ${genvtx_PUBLIC_HEADERS} 
  ${genvtx_detail_HEADERS}
  ${genvtx_SOURCES}
  ${genvtx_RESOURCES_OUTPUTS}
  )

set_target_properties(genvtx
  PROPERTIES 
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.genvtx
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${genvtx_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${genvtx_VERSION}
    VERSION                               ${genvtx_VERSION}
    SOVERSION                             ${genvtx_VERSION_MAJOR}
    PUBLIC_HEADER                         "${genvtx_PUBLIC_HEADERS}"
  )

if ( GENVTX_WITH_REFLECTION )
  target_link_libraries(genvtx ${geomtools_LIBRARIES} ${CAMP_LIBRARIES} ${CMAKE_DL_LIBS})
else ()
  target_link_libraries(genvtx ${geomtools_LIBRARIES} ${CMAKE_DL_LIBS})
endif ()

set(GENVTX_TARGETS genvtx)

#-----------------------------------------------------------------------
# BUILD THE GENVTX BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
# if(GENVTX_WITH_BIO)
#   set(genvtx_bio_SOURCES
#     include/genvtx/the_serializable.h
#     include/genvtx/the_serializable.ipp
#     the_serializable.cc
#     )
#   list_prepend(genvtx_bio_SOURCES "${PROJECT_SOURCE_DIR}/")
  
#   add_library(genvtx_bio SHARED ${genvtx_bio_SOURCES})

#   if(APPLE)
#     set_property(TARGET genvtx_bio APPEND
#       PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
#       )
#   endif()

#   list(APPEND GENVTX_TARGETS genvtx_bio)
# endif()

#-----------------------------------------------------------------------
# BUILD THE GENVTX REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(GENVTX_WITH_REFLECTION)
  set(genvtx_reflection_SOURCES
    #include/genvtx/*-reflect.h
    #include/genvtx/the_introspectable.h
    #the_introspectable.cc
    )
  list_prepend(genvtx_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")
  
  add_library(genvtx_reflection SHARED ${genvtx_reflection_SOURCES})

  if(APPLE)
    set_property(TARGET genvtx_reflection APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND GENVTX_TARGETS genvtx_reflection)
endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature 
install(TARGETS ${GENVTX_TARGETS}
  EXPORT        genvtx-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR} 
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${GENVTX_FRAMEWORK_LOCATION} 
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genvtx 
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT genvtx-libraries 
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT genvtx-libraries 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/genvtx-${genvtx_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for genvtx/src subdir
