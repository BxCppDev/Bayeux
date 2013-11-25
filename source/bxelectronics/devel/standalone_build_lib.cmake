
# -----------------------------------------------------------------------
# Configure library
#

set(electronics_BASE_DIR ${PROJECT_SOURCE_DIR}/..)
message( STATUS "electronics_BASE_DIR='${electronics_BASE_DIR}'")

include_directories(
  ${electronics_BASE_DIR}
  ${electronics_BUILDPRODUCT_DIR}/include
  )

set(electronics_SOURCE_DIR ${electronics_BASE_DIR}/electronics)
message( STATUS "electronics_SOURCE_DIR='${electronics_SOURCE_DIR}'")

set(electronicsLibrary_HEADERS
  ${electronics_SOURCE_DIR}/config.h.in
  ${electronics_SOURCE_DIR}/version.h.in
  ${electronics_SOURCE_DIR}/component_utils.h
  ${electronics_SOURCE_DIR}/component_types.h
  ${electronics_SOURCE_DIR}/component_types-reflect.h
  ${electronics_SOURCE_DIR}/component_base.h
  ${electronics_SOURCE_DIR}/bitset_desc.h
  ${electronics_SOURCE_DIR}/register_base.h
  ${electronics_SOURCE_DIR}/board_base.h
  ${electronics_SOURCE_DIR}/crate_base.h
  ${electronics_SOURCE_DIR}/rack_base.h
  ${electronics_SOURCE_DIR}/component_manager.h
  #${electronics_SOURCE_DIR}/digitized_signal.h
 )

# - Publish headers
foreach(_hdrin ${electronicsLibrary_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REPLACE "${electronics_SOURCE_DIR}/" "" _hdrout "${_hdrout}")
  message( STATUS "_hdrout='${_hdrout}'")
  configure_file(${_hdrin} ${electronics_BUILDPRODUCT_DIR}/include/electronics/${_hdrout} @ONLY)
endforeach()

#return()

#configure_file(test.cc.in ${CMAKE_CURRENT_BINARY_DIR}/test.cc @ONLY)

# - Configure source
set(electronicsLibrary_SOURCES
  ${electronics_SOURCE_DIR}/version.cc
  ${electronics_SOURCE_DIR}/component_types.cc
  ${electronics_SOURCE_DIR}/component_utils.cc
  ${electronics_SOURCE_DIR}/component_base.cc
  ${electronics_SOURCE_DIR}/bitset_desc.cc
  ${electronics_SOURCE_DIR}/register_base.cc
  ${electronics_SOURCE_DIR}/board_base.cc
  ${electronics_SOURCE_DIR}/crate_base.cc
  ${electronics_SOURCE_DIR}/rack_base.cc
  ${electronics_SOURCE_DIR}/component_manager.cc
  ${electronics_SOURCE_DIR}/the_introspectable.cc
  #${electronics_SOURCE_DIR}/digitized_signal.cc
  #${CMAKE_CURRENT_BINARY_DIR}/test.cc
)

#-----------------------------------------------------------------------
# Build/Link Library
#

include_directories(
  ${Bayeux_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  )
add_library(electronics SHARED
  ${electronicsLibrary_HEADERS}
  ${electronicsLibrary_SOURCES}
  )
set_target_properties(electronics PROPERTIES COMPILE_DEFINITIONS ENABLE_BINRELOC)

target_link_libraries(electronics
  ${Bayeux_LIBRARIES}
  ${Boost_LIBRARIES}
  )

# - On Apple, ensure dynamic_lookup of undefined symbols
if(APPLE)
  set_target_properties(electronics PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

install(TARGETS electronics
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

#-----------------------------------------------------------------------
# Configure/Install support files
# Done here so that external package variables are visible
#
# - Targets
# export(TARGETS electronics FILE ${electronics_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/electronics-${electronics_VERSION}/electronicsTargets.cmake)

# install(EXPORT electronicsTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/electronics-${electronics_VERSION})

# # - Versioning (build and install trees)
# configure_file(${PROJECT_SOURCE_DIR}/cmake/electronicsConfigVersion.cmake.in
#   ${electronics_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/electronics-${electronics_VERSION}/electronicsConfigVersion.cmake
#   @ONLY
#   )

# # - Config (build and install trees *at present*)
# configure_file(${PROJECT_SOURCE_DIR}/cmake/electronicsConfig.cmake.in
#   ${electronics_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/electronics-${electronics_VERSION}/electronicsConfig.cmake
#   @ONLY
#   )

# #
# #-----------------------------------------------------------------------
# # Install targets and resources
# #
# install(TARGETS electronics
#   EXPORT  electronicsTargets
#   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#   )

#end