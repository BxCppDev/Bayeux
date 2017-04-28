# - Top level CMakeLists.txt for dpp ex_01 example library

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(dpp_ex01)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

# - Bayeux :
find_package(Bayeux 3.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})

add_library(dpp_ex01 SHARED
  dpp_ex01/hit.h
  dpp_ex01/hit.ipp
  dpp_ex01/raw_data.h
  dpp_ex01/raw_data.ipp
  dpp_ex01/raw_generator_module.h
  src/hit.cc
  src/raw_data.cc
  src/raw_generator_module.cc
  src/the_serializable.cc
  )

target_link_libraries(dpp_ex01 Bayeux::Bayeux)

install( FILES
  ${PROJECT_BINARY_DIR}/libdpp_ex01${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

add_executable(test_dpp_ex01 testing/test_dpp_ex01.cxx )

target_link_libraries(test_dpp_ex01 dpp_ex01 )

install( FILES
  ${PROJECT_BINARY_DIR}/test_dpp_ex01
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt