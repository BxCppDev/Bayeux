# - CMakeLists.txt for cuts/src subdir

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

set(cuts_PUBLIC_HEADERS
include/cuts/cut_macros.h
include/cuts/cut_manager.h
include/cuts/cut_service.h
include/cuts/cut_tools.h
include/cuts/i_cut.h
include/cuts/accept_cut.h
include/cuts/reject_cut.h
include/cuts/random_cut.h
include/cuts/i_binary_cut.h
include/cuts/and_cut.h
include/cuts/nand_cut.h
include/cuts/nor_cut.h
include/cuts/not_cut.h
include/cuts/or_cut.h
include/cuts/xnor_cut.h
include/cuts/xor_cut.h
include/cuts/i_multi_cut.h
include/cuts/multi_and_cut.h
include/cuts/multi_or_cut.h
include/cuts/multi_xor_cut.h
include/cuts/ocd_support.h
  )
list_prepend(cuts_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/cuts/cuts_config.h.in
  ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h
  )
list(APPEND cuts_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

set_source_files_properties(${cuts_eos_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/eos
  )

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
# set(cuts_detail_HEADERS
#   include/cuts/detail/bio_link_guard.h
#   )
# list_prepend(cuts_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

# set_source_files_properties(${cuts_detail_HEADERS}
#   PROPERTIES
#     MACOSX_PACKAGE_LOCATION Headers/detail
#   )

# # We have to install the detail header modules separately if cuts
# # isn't built as a Framework.
# if(NOT APPLE)
#   foreach(_module detail eos)
#     install(FILES ${cuts_${_module}_HEADERS}
#       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cuts/${_module}
#       COMPONENT   Development
#       )
#   endforeach()
# endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(cuts_SOURCES
    cut_manager.cc
    cuts_config.cc
    cut_service.cc
    cut_tools.cc
    i_cut.cc
    accept_cut.cc
    reject_cut.cc
    random_cut.cc
    i_binary_cut.cc
    and_cut.cc
    nand_cut.cc
    nor_cut.cc
    not_cut.cc
    or_cut.cc
    xnor_cut.cc
    xor_cut.cc
    i_multi_cut.cc
    multi_and_cut.cc
    multi_or_cut.cc
    multi_xor_cut.cc
    ocd_support.cc
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
  set(TO_CUTS_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_CUTS_INCLUDEDIR
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/cuts-${cuts_VERSION}"
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(cuts_RESOURCES
  cmake/cuts-config.cmake
  cmake/cuts-config-version.cmake
  )

foreach(_dtresource ${cuts_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND cuts_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND cuts_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${cuts_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${cuts_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cuts-${cuts_VERSION}"
    COMPONENT Development
    )
endif()

#-----------------------------------------------------------------------
# BUILD THE CUTS LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${datatools_INCLUDE_DIRS}
  )

if ( CUTS_WITH_REFLECTION )
  include_directories(
    ${CAMP_INCLUDE_DIR}
    )
endif ()

add_library(cuts SHARED
  ${cuts_PUBLIC_HEADERS}
  ${cuts_SOURCES}
  ${cuts_RESOURCES_OUTPUTS}
  )

  #${cuts_detail_HEADERS}

set_target_properties(cuts
  PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.cuts
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${cuts_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${cuts_VERSION}
    VERSION                               ${cuts_VERSION}
    SOVERSION                             ${cuts_VERSION_MAJOR}
    PUBLIC_HEADER                         "${cuts_PUBLIC_HEADERS}"
  )

if ( CUTS_WITH_REFLECTION )
  target_link_libraries(cuts ${datatools_LIBRARIES} ${CAMP_LIBRARIES} ${CMAKE_DL_LIBS})
else ()
  target_link_libraries(cuts ${datatools_LIBRARIES} ${CMAKE_DL_LIBS})
endif ()

set(CUTS_TARGETS cuts)

#-----------------------------------------------------------------------
# BUILD THE CUTS BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
# if(CUTS_WITH_BIO)
#   set(cuts_bio_SOURCES
#     include/cuts/the_serializable.h
#     include/cuts/the_serializable.ipp
#     src/the_serializable.cc
#     )
#   list_prepend(cuts_bio_SOURCES "${PROJECT_SOURCE_DIR}/")

#   add_library(cuts_bio SHARED ${cuts_bio_SOURCES})

#   if(APPLE)
#     set_property(TARGET cuts_bio APPEND
#       PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
#       )
#   endif()

#   list(APPEND CUTS_TARGETS cuts_bio)
# endif()

#-----------------------------------------------------------------------
# BUILD THE CUTS REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
# if(CUTS_WITH_REFLECTION)
#   set(cuts_reflection_SOURCES
#     #include/cuts/*-reflect.h
#     #include/cuts/the_introspectable.h
#     #src/the_introspectable.cc
#     )
#   list_prepend(cuts_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")

#   add_library(cuts_reflection SHARED ${cuts_reflection_SOURCES})

#   if(APPLE)
#     set_property(TARGET cuts_reflection APPEND
#       PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
#       )
#   endif()

#   list(APPEND CUTS_TARGETS cuts_reflection)
# endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature
install(TARGETS ${CUTS_TARGETS}
  EXPORT        cuts-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${CUTS_FRAMEWORK_LOCATION}
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cuts
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT cuts-libraries
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT cuts-libraries
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cuts-${cuts_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for cuts/src subdir
