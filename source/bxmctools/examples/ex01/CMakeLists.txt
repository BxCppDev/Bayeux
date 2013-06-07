# - Top level CMakeLists.txt for mctools ex01 example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(mctools_ex01)

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

# - mctools :
if (NOT mctools_DIR)
  execute_process(COMMAND mctools-config --prefix
                  OUTPUT_VARIABLE mctools_DIR )
endif()

find_package(mctools 1.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${mctools_INCLUDE_DIRS})

add_executable(ex01_read_plain_simdata ex01_read_plain_simdata.cxx)

target_link_libraries(ex01_read_plain_simdata ${mctools_LIBRARY}
					      ${mctools_BIO_LIBRARY}
					      ${geomtools_BIO_LIBRARY}
					      ${datatools_BIO_LIBRARY} )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex01_read_plain_simdata
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt