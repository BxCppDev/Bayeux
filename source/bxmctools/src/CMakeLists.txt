# - CMakeLists.txt for mctools/src subdir

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

set(mctools_PUBLIC_HEADERS

    include/mctools/base_step_hit.h
    include/mctools/simulated_data.h
    include/mctools/utils.h
    include/mctools/base_step_hit.ipp
    include/mctools/simulated_data.ipp
    include/mctools/bio_guard.h
    include/mctools/base_step_hit_processor.h
    include/mctools/step_hit_processor_factory.h
    include/mctools/calorimeter_step_hit_processor.h

    include/mctools/base_step_hit.ipp
    include/mctools/simulated_data.ipp
    include/mctools/the_serializable.ipp
    include/mctools/the_serializable.h

    include/mctools/base_step_hit-reflect.h
    include/mctools/simulated_data-reflect.h
    include/mctools/the_introspectable.h

    )
list_prepend(mctools_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/mctools/mctools_config.h.in
  ${PROJECT_BINARY_DIR}/include/mctools/mctools_config.h
  )
list(APPEND mctools_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/mctools/mctools_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(mctools_detail_HEADERS
  include/mctools/detail/bio_link_guard.h
  include/mctools/detail/step_hit_processor_macros.h
  )
list_prepend(mctools_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${mctools_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if mctools
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail)
    install(FILES ${mctools_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mctools/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(mctools_SOURCES
    base_step_hit.cc
    base_step_hit_processor.cc
    step_hit_processor_factory.cc
    calorimeter_step_hit_processor.cc
    simulated_data.cc
    utils.cc
  )

# message(STATUS "********** mctools_SOURCES=${mctools_SOURCES}")

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
  set(TO_MCTOOLS_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_MCTOOLS_INCLUDEDIR
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/mctools-${mctools_VERSION}"
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(mctools_RESOURCES
  cmake/mctools-config.cmake
  cmake/mctools-config-version.cmake
  )

foreach(_dtresource ${mctools_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND mctools_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND mctools_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${mctools_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${mctools_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/mctools-${mctools_VERSION}"
    COMPONENT Development
    )
endif()

#-----------------------------------------------------------------------
# BUILD THE MCTOOLS LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${genvtx_INCLUDE_DIRS}
  ${genbb_help_INCLUDE_DIRS}
  ${materials_INCLUDE_DIRS}
  ${emfield_INCLUDE_DIRS}
  ${dpp_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  )

add_library(mctools SHARED
  ${mctools_PUBLIC_HEADERS}
  ${mctools_detail_HEADERS}
  ${mctools_SOURCES}
  ${mctools_RESOURCES_OUTPUTS}
  )

set_target_properties(mctools
  PROPERTIES
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.mctools
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${mctools_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${mctools_VERSION}
    VERSION                               ${mctools_VERSION}
    SOVERSION                             ${mctools_VERSION_MAJOR}
    PUBLIC_HEADER                         "${mctools_PUBLIC_HEADERS}"
  )

target_link_libraries(mctools
                        ${genvtx_LIBRARIES}
                        ${genbb_help_LIBRARIES}
                        ${materials_LIBRARIES}
                        ${emfield_LIBRARIES}
                        ${dpp_LIBRARIES}
			${Boost_LIBRARIES}
                        ${CMAKE_DL_LIBS}
  )

set(MCTOOLS_TARGETS mctools)

#-----------------------------------------------------------------------
# BUILD THE MCTOOLS BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(MCTOOLS_WITH_BIO)
  set(mctools_bio_SOURCES
    include/mctools/the_serializable.h
    include/mctools/the_serializable.ipp
    src/the_serializable.cc
    )
  list_prepend(mctools_bio_SOURCES "${PROJECT_SOURCE_DIR}/")

  add_library(mctools_bio SHARED ${mctools_bio_SOURCES})

  if(APPLE)
    set_property(TARGET mctools_bio APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND MCTOOLS_TARGETS mctools_bio)
endif()

#-----------------------------------------------------------------------
# BUILD THE MCTOOLS G4 INTERFACE COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(MCTOOLS_WITH_G4)

 include_directories(${Geant4_INCLUDE_DIRS})

 set(mctools_g4_PUBLIC_HEADERS
    include/mctools/g4/loggable_support.h
    include/mctools/g4/tracking_action.h
    include/mctools/g4/track_history.h
    include/mctools/g4/stepping_verbose.h
    include/mctools/g4/stepping_action.h
    include/mctools/g4/stacking_action.h
    include/mctools/g4/stacking_action.h
    include/mctools/g4/run_action.h
    include/mctools/g4/event_action.h
    include/mctools/g4/sensitive_hit_collection.h
    include/mctools/g4/sensitive_hit.h
    include/mctools/g4/sensitive_detector.h
    include/mctools/g4/simulation_module.h
    include/mctools/g4/simulation_ctrl.h
    include/mctools/g4/g4_prng.h
    include/mctools/g4/magnetic_field.h
    include/mctools/g4/physics_list.h
    include/mctools/g4/manager_parameters.h
    include/mctools/g4/primary_generator.h
  )

  set(mctools_g4_SOURCES
    ${mctools_g4_PUBLIC_HEADERS}
    src/g4/loggable_support.cc
    src/g4/detector_construction.cc
    src/g4/event_action.cc
    src/g4/g4_prng.cc
    src/g4/magnetic_field.cc
    src/g4/manager.cc
    src/g4/manager_parameters.cc
    src/g4/physics_list.cc
    src/g4/primary_generator.cc
    src/g4/run_action.cc
    src/g4/sensitive_detector.cc
    src/g4/sensitive_hit.cc
    src/g4/sensitive_hit_collection.cc
    src/g4/simulation_ctrl.cc
    src/g4/simulation_module.cc
    src/g4/stacking_action.cc
    src/g4/stepping_action.cc
    src/g4/stepping_verbose.cc
    src/g4/track_history.cc
    src/g4/tracking_action.cc
    )
  list_prepend(mctools_g4_SOURCES "${PROJECT_SOURCE_DIR}/")

  add_library(mctools_g4 SHARED ${mctools_g4_SOURCES})

  set(_mctools_g4_additional_libs)
  if (MCTOOLS_WITH_G4_G4VIS_USE)
     list(APPEND _mctools_g4_additional_libs G4OpenGL)
  endif()
  target_link_libraries(mctools_g4
                        mctools
                        ${Geant4_LIBRARIES}
			${_mctools_g4_additional_libs}
  )
  ### gcc only : -Wl,--no-as-needed G4OpenGL G4visXXX)

  if(APPLE)
    set_property(TARGET mctools_g4 APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND MCTOOLS_TARGETS mctools_g4)
endif()

#-----------------------------------------------------------------------
# BUILD THE MCTOOLS REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(MCTOOLS_WITH_REFLECTION)
  set(mctools_reflection_SOURCES
    include/mctools/base_step_hit-reflect.h
    include/mctools/simulated_data-reflect.h
    include/mctools/the_introspectable.h
    src/the_introspectable.cc
    )
  list_prepend(mctools_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")

  add_library(mctools_reflection SHARED ${mctools_reflection_SOURCES})

  if(APPLE)
    set_property(TARGET mctools_reflection APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND MCTOOLS_TARGETS mctools_reflection)
endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature
install(TARGETS ${MCTOOLS_TARGETS}
  EXPORT        mctools-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${MCTOOLS_FRAMEWORK_LOCATION}
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mctools
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT mctools-libraries
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT mctools-libraries
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mctools-${mctools_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for mctools/src subdir
