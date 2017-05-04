# - Top level CMakeLists.txt for genbb_help ex01 example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(genbb_help_ex01)

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
find_package(Bayeux 3.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(ex01 ex01.cxx)

target_link_libraries(ex01 Bayeux::Bayeux)

add_executable(ex01_plain_reader ex01_plain_reader.cxx)

target_link_libraries(ex01_plain_reader Bayeux::Bayeux )

install ( FILES
  ${PROJECT_BINARY_DIR}/ex01
  ${PROJECT_BINARY_DIR}/ex01_plain_reader
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt