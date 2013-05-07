# - CMakeLists.txt for genbb_help/src subdir

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

if (GENBB_HELP_WITH_FORTRAN)

  message ( STATUS "${PROJECT_NAME}: Preparing special mangling name for the Fortran/C interface..." )
  FortranCInterface_HEADER (${PROJECT_BINARY_DIR}/include/genbb_help/decay0_fortran/__genbb_help_FC.h
    MACRO_NAMESPACE GENBB_HELP_FortranCInterface_
     SYMBOLS fermi_wrap rng_shoot_reset rng_shoot_flat)

  install ( FILES ${PROJECT_BINARY_DIR}/include/genbb_help/decay0_fortran/__genbb_help_FC.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help/decay0_fortran
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
          )

  FortranCInterface_HEADER (${PROJECT_BINARY_DIR}/include/genbb_help/decay0_fortran/__genbb_help_FC2.h
    MACRO_NAMESPACE GENBB_HELP_FortranCInterface_
    SYMBOLS set_genbb_random_seed enrange genevent genbbsub )
    # genbbpar)

  # install ( FILES ${PROJECT_BINARY_DIR}/include/genbb_help/decay0_fortran/__genbb_help_FC2.h
  #           DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help/decay0_fortran
  #           PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  #         )

  file ( GLOB genbb_help_decay0_fortran_HEADERS
    ${PROJECT_SOURCE_DIR}/include/genbb_help/decay0_fortran/dummy.inc
    ${PROJECT_SOURCE_DIR}/include/genbb_help/decay0_fortran/genbb_commons.inc
    ${PROJECT_SOURCE_DIR}/include/genbb_help/decay0_fortran/genbb_commons.h
    ${PROJECT_SOURCE_DIR}/include/genbb_help/decay0_fortran/fermi.h
    ${PROJECT_SOURCE_DIR}/include/genbb_help/decay0_fortran/rng.h
  )

  file ( GLOB genbb_help_decay0_fortran_SOURCES
    ${genbb_help_decay0_fortran_HEADERS}
    ${PROJECT_SOURCE_DIR}/src/decay0_fortran/fermi.cc
    ${PROJECT_SOURCE_DIR}/src/decay0_fortran/rng.cc
    ${PROJECT_SOURCE_DIR}/src/decay0_fortran/genbb_commons.cc
    ${PROJECT_SOURCE_DIR}/src/decay0_fortran/*.f
  )

endif (GENBB_HELP_WITH_FORTRAN)

file ( GLOB genbb_help_decay0_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genbb_help/decay0/*.h
  )

file ( GLOB genbb_help_decay0_SOURCES
  ${genbb_help_decay0_PUBLIC_HEADERS}
  ${PROJECT_SOURCE_DIR}/src/decay0/*.cc
  )

#${PROJECT_SOURCE_DIR}/include/genbb_help/genbb_commons.h

file ( GLOB genbb_help_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genbb_help/bio_guard.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/genbb_macros.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/genbb_mgr.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/genbb_utils.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/genbb_writer.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/i_genbb.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/manager.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/primary_event.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/primary_particle.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/single_particle_generator.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/the_serializable.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/primary_event.ipp
  ${PROJECT_SOURCE_DIR}/include/genbb_help/primary_particle.ipp
  ${PROJECT_SOURCE_DIR}/include/genbb_help/the_serializable.ipp
  ${PROJECT_SOURCE_DIR}/include/genbb_help/wdecay0.h
  )

if (GENBB_HELP_WITH_FORTRAN)
   list (APPEND  genbb_help_PUBLIC_HEADERS
     ${PROJECT_SOURCE_DIR}/include/genbb_help/genbb.h)
   list (APPEND  genbb_help_PUBLIC_HEADERS
     ${PROJECT_SOURCE_DIR}/include/genbb_help/wgenbb.h)
endif (GENBB_HELP_WITH_FORTRAN)

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/genbb_help/genbb_help_config.h.in
  ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h
  )

list(APPEND genbb_help_PUBLIC_HEADERS
     ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS
#

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(genbb_help_detail_HEADERS
  include/genbb_help/detail/bio_link_guard.h
  include/genbb_help/detail/pg_tools.h
   )
list_prepend(genbb_help_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${genbb_help_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if genbb_help
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail decay0 decay0_fortran)
    install(FILES ${genbb_help_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
###      genbb_commons.cc
# Pretty basic!
set(genbb_help_SOURCES
      genbb_utils.cc
      genbb_writer.cc
      pg_tools.cc
      i_genbb.cc
      primary_event.cc
      primary_particle.cc
      wdecay0.cc
      genbb_mgr.cc
      single_particle_generator.cc
      combined_particle_generator.cc
      manager.cc
)

if (GENBB_HELP_WITH_FORTRAN)
   list(APPEND genbb_help_SOURCES genbb.cc)
   list(APPEND genbb_help_SOURCES wgenbb.cc)
endif (GENBB_HELP_WITH_FORTRAN)

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
  set(TO_GENBB_HELP_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_GENBB_HELP_INCLUDEDIR
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/genbb_help-${genbb_help_VERSION}"
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(genbb_help_RESOURCES
  cmake/genbb_help-config.cmake
  cmake/genbb_help-config-version.cmake
  )

foreach(_dtresource ${genbb_help_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND genbb_help_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND genbb_help_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${genbb_help_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${genbb_help_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/genbb_help-${genbb_help_VERSION}"
    COMPONENT Development
    )
endif()

#-----------------------------------------------------------------------
# BUILD THE GENBB_HELP LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${geomtools_INCLUDE_DIRS}
  )

#if(GENBB_HELP_WITH_REFLECTION)
#  include_directories(${CAMP_INCLUDE_DIR})
#endif()

# ----------------------------------------------------------------------------
# Build the Fortran part of the library :
set (_fortran_libs "")
if (GENBB_HELP_WITH_FORTRAN)

  add_library(genbb_help_decay0_fortran SHARED
    ${genbb_help_decay0_fortran_PUBLIC_HEADERS}
    ${genbb_help_decay0_fortran_SOURCES}
    )

  set_target_properties(genbb_help_decay0_fortran
    PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.genbb_help
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${genbb_help_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${genbb_help_VERSION}
    VERSION                               ${genbb_help_VERSION}
    SOVERSION                             ${genbb_help_VERSION_MAJOR}
    PUBLIC_HEADER                         "${genbb_help_decay0_fortran_PUBLIC_HEADERS}"
  )

  target_link_libraries(genbb_help_decay0_fortran ${geomtools_LIBRARIES} ${CMAKE_DL_LIBS})
  set (_fortran_libs genbb_help_decay0_fortran)

endif()

# ----------------------------------------------------------------------------
# Build the C++ decay0 port library :

add_library(genbb_help_decay0_cpp SHARED
  ${genbb_help_decay0_PUBLIC_HEADERS}
  ${genbb_help_decay0_SOURCES}
  )

set_target_properties(genbb_help_decay0_cpp
    PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.genbb_help
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${genbb_help_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${genbb_help_VERSION}
    VERSION                               ${genbb_help_VERSION}
    SOVERSION                             ${genbb_help_VERSION_MAJOR}
    PUBLIC_HEADER                         "${genbb_help_decay0_PUBLIC_HEADERS}"
)

target_link_libraries(genbb_help_decay0_cpp ${geomtools_LIBRARIES} ${CMAKE_DL_LIBS})

# ----------------------------------------------------------------------------
# Build the C++ part of the library :

add_library(genbb_help SHARED
  ${genbb_help_PUBLIC_HEADERS}
  ${genbb_help_detail_HEADERS}
  ${genbb_help_SOURCES}
  ${genbb_help_RESOURCES_OUTPUTS}
  )

set_target_properties(genbb_help
  PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.genbb_help
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${genbb_help_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${genbb_help_VERSION}
    VERSION                               ${genbb_help_VERSION}
    SOVERSION                             ${genbb_help_VERSION_MAJOR}
    PUBLIC_HEADER                         "${genbb_help_PUBLIC_HEADERS}"
  )

if(GENBB_HELP_WITH_REFLECTION)
  target_link_libraries(genbb_help genbb_help_decay0_cpp ${_fortran_libs} ${CAMP_LIBRARIES} ${CMAKE_DL_LIBS})
else()
  target_link_libraries(genbb_help genbb_help_decay0_cpp ${_fortran_libs} ${CMAKE_DL_LIBS})
endif()

set(GENBB_HELP_TARGETS genbb_help genbb_help_decay0_cpp ${_fortran_libs})

#-----------------------------------------------------------------------
# BUILD THE GENBB_HELP BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(GENBB_HELP_WITH_BIO)
  set(genbb_help_bio_SOURCES
    include/genbb_help/the_serializable.h
    include/genbb_help/the_serializable.ipp
    src/the_serializable.cc
    )
  list_prepend(genbb_help_bio_SOURCES "${PROJECT_SOURCE_DIR}/")

  add_library(genbb_help_bio SHARED ${genbb_help_bio_SOURCES})

  if(APPLE)
    set_property(TARGET genbb_help_bio APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND GENBB_HELP_TARGETS genbb_help_bio)
endif()


#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature
install(TARGETS ${GENBB_HELP_TARGETS}
  EXPORT        genbb_help-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${GENBB_HELP_FRAMEWORK_LOCATION}
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT genbb_help-libraries
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT genbb_help-libraries
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/genbb_help-${genbb_help_VERSION}
    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for genbb_help/src subdir
