# - Top level CMakeLists.txt for cuts ex_manager example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(cuts_ex_manager)

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

# - cuts :
if (NOT cuts_DIR)
  execute_process(COMMAND cuts-config --prefix
                  OUTPUT_VARIABLE cuts_DIR )
endif()

find_package(cuts 4.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${cuts_INCLUDE_DIRS})

add_executable(ex_manager data.cc
			  sphere_cut.cc
			  range_cut.cc
			  color_cut.cc
			  ex_manager.cxx)

target_link_libraries(ex_manager ${cuts_LIBRARY} )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex_manager
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt