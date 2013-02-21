# - CMakeLists.txt for datatools/src subdir

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

set(datatools_PUBLIC_HEADERS
  include/datatools/archives_instantiation.h
  include/datatools/archives_list.h
  include/datatools/base_service.h
  include/datatools/bio_guard.h
  include/datatools/bit_mask.h
  include/datatools/caster_utils.h
  include/datatools/clhep_units.h
  include/datatools/datatools.h
  include/datatools/event_id.h
  include/datatools/factory.h
  include/datatools/factory_macros.h
  include/datatools/handle.h
  include/datatools/handle_macros.h
  include/datatools/handle_pool.h
  include/datatools/i_clear.h
  include/datatools/i_cloneable.h
  include/datatools/i_named.h
  include/datatools/integer_range.h
  include/datatools/io_factory.h
  include/datatools/ioutils.h
  include/datatools/i_predicate.h
  include/datatools/i_serializable.h
  include/datatools/i_tree_dump.h
  include/datatools/library_loader.h
  include/datatools/memory_streambuf.h
  include/datatools/multi_properties.h
  include/datatools/properties.h
  include/datatools/range_tools.h
  include/datatools/real_range.h
  include/datatools/reflection_guard.h
  include/datatools/reflection_macros.h
  include/datatools/safe_serial.h
  include/datatools/serialization_macros.h
  include/datatools/service_macros.h
  include/datatools/service_manager.h
  include/datatools/service_tools.h
  include/datatools/smart_filename.h
  include/datatools/smart_ref.h
  include/datatools/temporary_files.h
  include/datatools/the_introspectable.h
  include/datatools/the_serializable.h
  include/datatools/things.h
  include/datatools/things_macros.h
  include/datatools/time_tools.h
  include/datatools/units.h
  include/datatools/utils.h
  include/datatools/version_check.h
  include/datatools/version_id.h
  include/datatools/i_serializable.ipp
  include/datatools/event_id.ipp
  include/datatools/properties.ipp
  include/datatools/multi_properties.ipp
  include/datatools/things.ipp
  include/datatools/the_serializable.ipp
  )
list_prepend(datatools_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/datatools/datatools_config.h.in
  ${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h
  )
list(APPEND datatools_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS
# - EOS headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(datatools_eos_HEADERS
  include/datatools/eos/polymorphic_portable_archive.hpp
  include/datatools/eos/portable_archive.hpp
  include/datatools/eos/portable_archive_exception.hpp
  include/datatools/eos/portable_iarchive.hpp
  include/datatools/eos/portable_oarchive.hpp
  )
list_prepend(datatools_eos_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${datatools_eos_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/eos
  )

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(datatools_detail_HEADERS
  include/datatools/detail/Configure.h
  include/datatools/detail/DynamicLoader.h
  include/datatools/detail/bio_link_guard.h
  include/datatools/detail/reflection_link_guard.h
  include/datatools/detail/reflection_export.h
  include/datatools/detail/reflection_utils.h
  include/datatools/detail/reflection_version.h
  )
list_prepend(datatools_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${datatools_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if datatools 
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail eos)
    install(FILES ${datatools_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/datatools/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(datatools_SOURCES
  DynamicLoader.cc
  event_id.cc
  factory.cc
  i_named.cc
  i_tree_dump.cc
  i_serializable.cc
  integer_range.cc
  io_factory.cc
  ioutils.cc
  library_loader.cc
  multi_properties.cc
  properties.cc
  real_range.cc
  service_manager.cc
  service_tools.cc
  smart_filename.cc
  temporary_files.cc
  things.cc
  time_tools.cc
  units.cc
  utils.cc
  version_check.cc
  version_id.cc
  base_service.cc # MUST go at the end....
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
  set(TO_DATATOOLS_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_DATATOOLS_INCLUDEDIR 
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/datatools-${datatools_VERSION}" 
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(datatools_RESOURCES
  cmake/datatools-config.cmake
  cmake/datatools-config-version.cmake
  )

foreach(_dtresource ${datatools_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in 
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND datatools_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND datatools_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${datatools_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${datatools_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/datatools-${datatools_VERSION}"
    COMPONENT Development
    )
endif()

#-----------------------------------------------------------------------
# BUILD THE DATATOOLS LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  ${CLHEP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  )

if(DATATOOLS_WITH_REFLECTION)
  include_directories(${CAMP_INCLUDE_DIR})
endif()

add_library(datatools SHARED  
  ${datatools_PUBLIC_HEADERS} 
  ${datatools_eos_HEADERS}
  ${datatools_detail_HEADERS}
  ${datatools_SOURCES}
  ${datatools_RESOURCES_OUTPUTS}
  )

set_target_properties(datatools
  PROPERTIES 
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.datatools
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${datatools_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${datatools_VERSION}
    VERSION                               ${datatools_VERSION}
    SOVERSION                             ${datatools_VERSION_MAJOR}
    PUBLIC_HEADER                         "${datatools_PUBLIC_HEADERS}"
  )

if(DATATOOLS_WITH_REFLECTION)
  target_link_libraries(datatools ${Boost_LIBRARIES} ${CAMP_LIBRARIES} ${CMAKE_DL_LIBS})
else()
  target_link_libraries(datatools ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
endif()

set(DATATOOLS_TARGETS datatools)

#-----------------------------------------------------------------------
# BUILD THE DATATOOLS BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(DATATOOLS_WITH_BIO)
  set(datatools_bio_SOURCES
    include/datatools/the_serializable.h
    include/datatools/the_serializable.ipp
    src/the_serializable.cc
    )
  list_prepend(datatools_bio_SOURCES "${PROJECT_SOURCE_DIR}/")
  
  add_library(datatools_bio SHARED ${datatools_bio_SOURCES})

  if(APPLE)
    set_property(TARGET datatools_bio APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND DATATOOLS_TARGETS datatools_bio)
endif()

#-----------------------------------------------------------------------
# BUILD THE DATATOOLS REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(DATATOOLS_WITH_REFLECTION)
  set(datatools_reflection_SOURCES
    include/datatools/i_serializable-reflect.h
    include/datatools/event_id-reflect.h
    include/datatools/properties-reflect.h
    include/datatools/multi_properties-reflect.h
    include/datatools/things-reflect.h
    include/datatools/the_introspectable.h
    src/the_introspectable.cc
    )
  list_prepend(datatools_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")
  
  add_library(datatools_reflection SHARED ${datatools_reflection_SOURCES})

  if(APPLE)
    set_property(TARGET datatools_reflection APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND DATATOOLS_TARGETS datatools_reflection)
endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature 
install(TARGETS ${DATATOOLS_TARGETS}
  EXPORT        datatools-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR} 
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${DATATOOLS_FRAMEWORK_LOCATION} 
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/datatools 
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT datatools-libraries 
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT datatools-libraries 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/datatools-${datatools_VERSION}
    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for datatools/src subdir
