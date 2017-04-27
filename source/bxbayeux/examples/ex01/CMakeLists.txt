# - Top level CMakeLists.txt for bayeux ex01 example program

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(bayeux_ex01)

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
message( STATUS "Searching Bayeux...")
find_package(Bayeux 3.0.0 REQUIRED)
message( STATUS "Found Bayeux version : ${Bayeux_VERSION}")
message( STATUS " * Bayeux Qt     : ${Bayeux_qt_FOUND}")
message( STATUS " * Bayeux Geant4 : ${Bayeux_geant4_FOUND}")
message( STATUS " * Bayeux MNCP   : ${Bayeux_mcnp_FOUND}")

# # - Boost :
# message( STATUS "Searching Boost...")
# find_package(Boost 1.60 REQUIRED log regex)
# message( STATUS "Found Boost version : ${Boost_VERSION}")

add_executable(ex01_user ex01_user.cxx)

target_link_libraries(ex01_user PUBLIC Bayeux::Bayeux)

install ( FILES
    ${PROJECT_BINARY_DIR}/ex01_user
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )
