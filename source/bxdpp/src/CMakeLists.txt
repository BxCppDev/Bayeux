# - CMakeLists.txt for dpp/src subdir

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

set(dpp_PUBLIC_HEADERS
    include/dpp/utils.h
    include/dpp/base_module.h
    include/dpp/brio_common.h
    include/dpp/chain_module.h
    include/dpp/context_service.h
    include/dpp/dummy_module.h
    include/dpp/i_data_sink.h
    include/dpp/i_data_source.h
    include/dpp/if_module.h
    include/dpp/io_module.h
    include/dpp/module_macros.h
    include/dpp/module_manager.h
    include/dpp/module_tools.h
    include/dpp/simple_brio_data_sink.h
    include/dpp/simple_brio_data_source.h
    include/dpp/simple_data_sink.h
    include/dpp/simple_data_source.h
    include/dpp/skip_module.h
    include/dpp/utils_module.h
    include/dpp/dump_module.h
    include/dpp/utils_cut.h
    include/dpp/ocd_support.h
  )
list_prepend(dpp_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/dpp/dpp_config.h.in
  ${PROJECT_BINARY_DIR}/include/dpp/dpp_config.h
  )
list(APPEND dpp_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/dpp/dpp_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
# set(dpp_detail_HEADERS
#   include/dpp/detail/bio_link_guard.h
#   )
# list_prepend(dpp_detail_HEADERS "${PROJECT_SOURCE_DIR}/")
#
# set_source_files_properties(${dpp_detail_HEADERS}
#   PROPERTIES
#     MACOSX_PACKAGE_LOCATION Headers/detail
#   )
#
# # We have to install the detail header modules separately if dpp
# # isn't built as a Framework.
# if(NOT APPLE)
#   foreach(_module detail)
#     install(FILES ${dpp_${_module}_HEADERS}
#       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dpp/${_module}
#       COMPONENT   Development
#       )
#   endforeach()
# endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(dpp_SOURCES
    utils_cut.cc
    base_module.cc
    brio_common.cc
    chain_module.cc
    context_service.cc
    dummy_module.cc
    if_module.cc
    io_module.cc
    module_manager.cc
    module_tools.cc
    i_data_sink.cc
    i_data_source.cc
    simple_brio_data_sink.cc
    simple_brio_data_source.cc
    simple_data_sink.cc
    simple_data_source.cc
    skip_module.cc
    utils_module.cc
    dump_module.cc
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
  set(TO_DPP_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_DPP_INCLUDEDIR
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/dpp-${dpp_VERSION}"
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(dpp_RESOURCES
    cmake/dpp-config.cmake
    cmake/dpp-config-version.cmake
  )

foreach(_dtresource ${dpp_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND dpp_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND dpp_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${dpp_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${dpp_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dpp-${dpp_VERSION}"
    COMPONENT Development
    )
endif()

if ( UNIX )
  configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/dpp-config.in
    ${PROJECT_BINARY_DIR}/cmake/dpp-config
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/dpp-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    		GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )

  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/dpp_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/dpp_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/dpp_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
  )

endif ()

#-----------------------------------------------------------------------
# BUILD THE DPP LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${boost_INCLUDE_DIRS}
  ${cuts_INCLUDE_DIRS}
  ${brio_INCLUDE_DIRS}
  ${mygsl_INCLUDE_DIRS}
  )

add_library(dpp SHARED
  ${dpp_PUBLIC_HEADERS}
  ${dpp_SOURCES}
  ${dpp_RESOURCES_OUTPUTS}
  )

set_target_properties(dpp
  PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.dpp
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${dpp_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${dpp_VERSION}
    VERSION                               ${dpp_VERSION}
    SOVERSION                             ${dpp_VERSION_MAJOR}
    PUBLIC_HEADER                         "${dpp_PUBLIC_HEADERS}"
  )

target_link_libraries(dpp
    ${mygsl_BIO_LIBRARY}
    ${mygsl_LIBRARIES}
    ${cuts_LIBRARIES}
    ${brio_LIBRARIES}
    ${datatools_BIO_LIBRARY}
    ${CMAKE_DL_LIBS})

set(DPP_TARGETS dpp)

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature
install(TARGETS ${DPP_TARGETS}
  EXPORT        dpp-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${DPP_FRAMEWORK_LOCATION}
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dpp
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT dpp-libraries
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT dpp-libraries
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dpp-${dpp_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for dpp/src subdir
