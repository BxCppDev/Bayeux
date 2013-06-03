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

# - genbb_help :
if (NOT genbb_help_DIR)
  execute_process(COMMAND genbb_help-config --prefix
                  OUTPUT_VARIABLE genbb_help_DIR )
endif()

find_package(genbb_help 5.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${genbb_help_INCLUDE_DIRS})

add_executable(ex01 ex01.cxx)

target_link_libraries(ex01 ${genbb_help_LIBRARY}
			   ${genbb_help_BIO_LIBRARY}
			   ${geomtools_BIO_LIBRARY}
			   ${datatools_BIO_LIBRARY} )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex01
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt