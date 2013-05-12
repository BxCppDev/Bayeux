# - Top level CMakeLists.txt for materials ex_manager example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(materials_ex_manager)

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

# - materials :
if (NOT materials_DIR)
  execute_process(COMMAND materials-config --prefix
                  OUTPUT_VARIABLE materials_DIR )
endif()

find_package(materials 4.0.0 REQUIRED)
message (STATUS "XXX=${materials_INCLUDE_DIRS}" )

include_directories(${PROJECT_SOURCE_DIR} ${materials_INCLUDE_DIRS})

add_executable(ex_manager ex_manager.cxx)

target_link_libraries(ex_manager ${materials_LIBRARY} )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex_manager
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt