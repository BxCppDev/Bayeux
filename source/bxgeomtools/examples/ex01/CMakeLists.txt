# - Top level CMakeLists.txt for geomtools ex01 example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(geomtools_ex01)

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

# - geomtools :
if (NOT geomtools_DIR)
  execute_process(COMMAND geomtools-config --prefix
                  OUTPUT_VARIABLE geomtools_DIR )
endif()

find_package(geomtools 4.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${geomtools_INCLUDE_DIRS})

add_executable(ex01 ex01.cxx)

target_link_libraries(ex01 ${geomtools_LIBRARY} )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex01
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt