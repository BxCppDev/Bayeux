# - Top level CMakeLists.txt for mctools ex03 example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(mctools_ex03)

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

# - Bayeux :
find_package(Bayeux 2.1.0 REQUIRED)
set(_incs ${Bayeux_INCLUDE_DIRS})
set(_libs ${Bayeux_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR} ${_incs})

set(mctools_hpge_SOURCES
  hpge/calorimetry.cc
  hpge/calorimeter_hit.cc
  )

add_library(mctools_hpge SHARED ${mctools_hpge_SOURCES})

install( FILES
         ${PROJECT_BINARY_DIR}/libmctools_hpge${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )

add_executable(hpge_analysis programs/hpge_analysis.cxx)

target_link_libraries(hpge_analysis mctools_hpge
  ${_libs}
  ${Boost_LIBRARIES}
  )

install ( FILES
    ${PROJECT_BINARY_DIR}/hpge_analysis
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt
