# - Top level CMakeLists.txt for mctools ex00 example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(mctools_ex00)

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

# - Boost
set(Boost_COMPONENTS program_options)
find_package(Boost 1.47 QUIET COMPONENTS ${Boost_COMPONENTS} NO_MODULE)
find_package(Boost 1.47 REQUIRED ${Boost_COMPONENTS})

# - mctools :
if (NOT mctools_DIR)
  execute_process(COMMAND mctools-config --prefix
                  OUTPUT_VARIABLE mctools_DIR )
endif()

find_package(mctools 1.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${mctools_INCLUDE_DIRS})

add_library(mctools_ex00 SHARED ex00_inspector.h ex00_inspector.cc)

install( FILES
         ${PROJECT_BINARY_DIR}/libmctools_ex00${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )

add_executable(ex00_read_plain_simdata ex00_read_plain_simdata.cxx)

target_link_libraries(ex00_read_plain_simdata mctools_ex00
					      ${mctools_LIBRARY}
					      ${mctools_BIO_LIBRARY}
					      ${geomtools_BIO_LIBRARY}
					      ${Boost_LIBRARIES}
                     )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex00_read_plain_simdata
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

add_executable(ex00_read_pipeline_simdata ex00_read_pipeline_simdata.cxx)

target_link_libraries(ex00_read_pipeline_simdata mctools_ex00
					      ${mctools_LIBRARY}
					      ${mctools_BIO_LIBRARY}
					      ${geomtools_BIO_LIBRARY}
					      ${Boost_LIBRARIES} )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex00_read_pipeline_simdata
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt