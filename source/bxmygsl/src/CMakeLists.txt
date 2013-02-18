# - CMakeLists.txt for mygsl/src subdir

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

set(mygsl_PUBLIC_HEADERS
include/mygsl/best_value.h
include/mygsl/bio_guard.h
include/mygsl/constants.h
include/mygsl/datapoint.h
include/mygsl/error.h
include/mygsl/fft_real.h
include/mygsl/histogram_1d.h
include/mygsl/histogram_2d.h
include/mygsl/histogram.h
include/mygsl/histogram_pool.h
include/mygsl/histogram_utils.h
include/mygsl/interval.h
include/mygsl/ioutils.h
include/mygsl/linear_regression.h
include/mygsl/linear_system_solver.h
include/mygsl/mean.h
include/mygsl/min_max.h
include/mygsl/multidimensional_minimization.h
include/mygsl/multi_eval.h
include/mygsl/multimin.h
include/mygsl/multiparameter_system.h
include/mygsl/numerical_differentiation.h
include/mygsl/ode.h
include/mygsl/one_dimensional_minimization.h
include/mygsl/one_dimensional_root_finding.h
include/mygsl/param_entry.h
include/mygsl/permutation.h
include/mygsl/polynomial.h
include/mygsl/prng_state_manager.h
include/mygsl/random_utils.h
include/mygsl/rng.h
include/mygsl/seed_manager.h
include/mygsl/tabfunc.h
include/mygsl/tabulated_function.h
include/mygsl/the_serializable.h
include/mygsl/unary_eval.h
include/mygsl/unary_eval_with_derivative.h
include/mygsl/von_neumann_method.h
include/mygsl/histogram_2d.ipp
include/mygsl/histogram.ipp
include/mygsl/histogram_pool.ipp
include/mygsl/the_serializable.ipp
  )
list_prepend(mygsl_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/include/mygsl/mygsl_config.h.in
  ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
  )
list(APPEND mygsl_PUBLIC_HEADERS ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h)

#-----------------------------------------------------------------------
# PRIVATE HEADERS

# - DETAIL headers: Again a list, but we also set their Mac OSX
# package location because they are nested...
set(mygsl_detail_HEADERS
  include/mygsl/detail/bio_link_guard.h
  )
list_prepend(mygsl_detail_HEADERS "${PROJECT_SOURCE_DIR}/")

set_source_files_properties(${mygsl_detail_HEADERS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Headers/detail
  )

# We have to install the detail header modules separately if mygsl 
# isn't built as a Framework.
if(NOT APPLE)
  foreach(_module detail)
    install(FILES ${mygsl_${_module}_HEADERS}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl/${_module}
      COMPONENT   Development
      )
  endforeach()
endif()

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(mygsl_SOURCES
  best_value.cc
  constants.cc
  datapoint.cc
  error.cc
  fft_real.cc
  histogram_2d.cc
  histogram.cc
  histogram_pool.cc
  interval.cc
  ioutils.cc
  linear_regression.cc
  linear_system_solver.cc
  mean.cc
  min_max.cc
  multidimensional_minimization.cc
  multi_eval.cc
  multimin.cc
  multiparameter_system.cc
  mygsl_config.cc
  numerical_differentiation.cc
  ode.cc
  one_dimensional_minimization.cc
  one_dimensional_root_finding.cc
  param_entry.cc
  permutation.cc  
  polynomial.cc
  prng_state_manager.cc
  random_utils.cc
  rng.cc
  seed_manager.cc
  tabfunc.cc
  unary_eval.cc
  unary_eval_with_derivative.cc
  von_neumann_method.cc
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
  set(TO_MYGSL_INCLUDEDIR "../..")
else()
  file(RELATIVE_PATH TO_MYGSL_INCLUDEDIR 
    "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/mygsl-${mygsl_VERSION}" 
    "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
    )
endif()

set(mygsl_RESOURCES
  cmake/mygsl-config.cmake
  cmake/mygsl-config-version.cmake
  )

foreach(_dtresource ${mygsl_RESOURCES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_dtresource}.in 
    ${PROJECT_BINARY_DIR}/${_dtresource}
    @ONLY
    )
  list(APPEND mygsl_RESOURCES_INPUTS ${PROJECT_SOURCE_DIR}/${_dtresource}.in)
  list(APPEND mygsl_RESOURCES_OUTPUTS ${PROJECT_BINARY_DIR}/${_dtresource})
endforeach()

set_source_files_properties(${mygsl_RESOURCES_OUTPUTS}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/CMake
  )

if(NOT APPLE)
  install(FILES ${mygsl_RESOURCES_OUTPUTS}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/mygsl-${mygsl_VERSION}"
    COMPONENT Development
    )
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/mygsl-config.in 
    ${PROJECT_BINARY_DIR}/cmake/mygsl-config
    @ONLY
    )

install(FILES ${PROJECT_BINARY_DIR}/cmake/mygsl-config
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PERMISSIONS OWNER_READ OWNER_EXECUTE 
    		GROUP_READ GROUP_EXECUTE 
                WORLD_READ WORLD_EXECUTE    
    COMPONENT Development
    )

if ( MYGSL_WITH_BASH_SETUP )
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/mygsl_setup.sh.in"
    "${PROJECT_BINARY_DIR}/cmake/mygsl_setup.sh"
    @ONLY
    )

  install(FILES ${PROJECT_BINARY_DIR}/cmake/mygsl_setup.sh
    DESTINATION ${CMAKE_INSTALL_ETCDIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT Development
  )
endif ()

#-----------------------------------------------------------------------
# BUILD THE MYGSL LIBRARY
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  ${datatools_INCLUDE_DIRS}
  ${GSL_INCLUDE_DIR}
 )

if ( MYGSL_WITH_REFLECTION )
  include_directories(
    ${CAMP_INCLUDE_DIR}
    )
endif ()

add_library(mygsl SHARED  
  ${mygsl_PUBLIC_HEADERS} 
  ${mygsl_detail_HEADERS}
  ${mygsl_SOURCES}
  ${mygsl_RESOURCES_OUTPUTS}
  )

set_target_properties(mygsl
  PROPERTIES 
    FRAMEWORK                             ON
    MACOSX_FRAMEWORK_IDENTIFIER           org.supernemo.mygsl
    MACOSX_FRAMEWORK_BUNDLE_VERSION       ${mygsl_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${mygsl_VERSION}
    VERSION                               ${mygsl_VERSION}
    SOVERSION                             ${mygsl_VERSION_MAJOR}
    PUBLIC_HEADER                         "${mygsl_PUBLIC_HEADERS}"
  )

if ( MYGSL_WITH_REFLECTION )
  target_link_libraries(mygsl 
    ${datatools_LIBRARIES} 
    ${GSL_LIBRARIES} 
    ${CAMP_LIBRARIES} 
    ${CMAKE_DL_LIBS})
else ()
  target_link_libraries(mygsl 
    ${datatools_LIBRARIES} 
    ${GSL_LIBRARIES} 
    ${CMAKE_DL_LIBS})
endif ()

set(MYGSL_TARGETS mygsl)

#-----------------------------------------------------------------------
# BUILD THE MYGSL BOOST I/O COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(MYGSL_WITH_BIO)
  set(mygsl_bio_SOURCES
    include/mygsl/the_serializable.h
    include/mygsl/the_serializable.ipp
    src/the_serializable.cc
    )
  list_prepend(mygsl_bio_SOURCES "${PROJECT_SOURCE_DIR}/")
  
  add_library(mygsl_bio SHARED ${mygsl_bio_SOURCES})

  if(APPLE)
    set_property(TARGET mygsl_bio APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND MYGSL_TARGETS mygsl_bio)
endif()

#-----------------------------------------------------------------------
# BUILD THE MYGSL REFLECTION COMPONENT
# We build this as a shared library as we can then link to it directly
# On Mac, we use dynamic_lookup for undefined symbols because we want
# the lib to act as a plugin.
#
if(MYGSL_WITH_REFLECTION)
  set(mygsl_reflection_SOURCES
    #include/mygsl/*-reflect.h
    #include/mygsl/the_introspectable.h
    #src/the_introspectable.cc
    )
  list_prepend(mygsl_reflection_SOURCES "${PROJECT_SOURCE_DIR}/")
  
  add_library(mygsl_reflection SHARED ${mygsl_reflection_SOURCES})

  if(APPLE)
    set_property(TARGET mygsl_reflection APPEND
      PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
      )
  endif()

  list(APPEND MYGSL_TARGETS mygsl_reflection)
endif()

#-----------------------------------------------------------------------
# EXPORT AND INSTALL LIBRARY TARGETS
# We make use of the full signature 
install(TARGETS ${MYGSL_TARGETS}
  EXPORT        mygsl-libraries
  RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR} 
                COMPONENT Runtime
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Runtime
  FRAMEWORK     DESTINATION ${MYGSL_FRAMEWORK_LOCATION} 
                COMPONENT Runtime
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR} 
                COMPONENT Development
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl 
                COMPONENT Development
  )

# - Install the lib depends file
# We also need to generate a file for the build tree, but at present
# we leave this out for simplicity.
if(APPLE)
  install(EXPORT mygsl-libraries 
    DESTINATION CMake COMPONENT Development
    )
else()
  install(EXPORT mygsl-libraries 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mygsl-${mygsl_VERSION}    COMPONENT Development
    )
endif()

# - end of CMakeLists.txt for mygsl/src subdir
