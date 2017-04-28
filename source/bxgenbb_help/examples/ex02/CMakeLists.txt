# - Top level CMakeLists.txt for genbb_help ex02 example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(genbb_help_ex02)

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

add_library(genbb_help_ex02 SHARED crazy_gun.cc crazy_gun.hh)

target_link_libraries(genbb_help_ex02 Bayeux::Bayeux )

install (FILES
         ${PROJECT_BINARY_DIR}/libgenbb_help_ex02${CMAKE_SHARED_LIBRARY_SUFFIX}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

##############
add_executable(ex02 ex02.cxx)

target_link_libraries(ex02
  genbb_help_ex02 Bayeux::Bayeux
  )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex02
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    )

# end of CMakeLists.txt