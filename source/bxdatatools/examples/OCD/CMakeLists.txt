# - Top level CMakeLists.txt for datatools ex_OCD example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(datatools_ex_OCD)

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

add_library(datatools_ex_OCD SHARED foo.cc)

target_link_libraries(datatools_ex_OCD Bayeux::Bayeux)

install( FILES
         ${PROJECT_BINARY_DIR}/libdatatools_ex_OCD${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )

add_executable(ex_OCD ex_OCD.cxx)

target_link_libraries(ex_OCD datatools_ex_OCD )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex_OCD
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt