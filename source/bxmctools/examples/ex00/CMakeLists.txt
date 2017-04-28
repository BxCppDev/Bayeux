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

# - Bayeux :
find_package(Bayeux 3.0.0 REQUIRED geant4)

include_directories(${PROJECT_SOURCE_DIR})
# get_target_property(Bayeux_COMPILE_FEATURES Bayeux::Bayeux INTERFACE_COMPILE_FEATURES)
# get_target_property(Bayeux_INCLUDE_DIRS Bayeux::Bayeux INTERFACE_INCLUDE_DIRECTORIES)
# get_target_property(Bayeux_LINK_LIBRARIES Bayeux::Bayeux INTERFACE_LINK_LIBRARIES)
# message(STATUS "Bayeux_COMPILE_FEATURES = '${Bayeux_INTERFACE_COMPILE_FEATURES}'")
# message(STATUS "Bayeux_INCLUDE_DIRS     = '${Bayeux_INCLUDE_DIRS}'")
# message(STATUS "Bayeux_LINK_LIBRARIES   = '${Bayeux_LINK_LIBRARIES}'")

###########
add_library(mctools_ex00 SHARED ex00_inspector.h ex00_inspector.cc)
target_link_libraries(mctools_ex00
  PUBLIC
  Bayeux::Bayeux
  )

install( FILES
         ${PROJECT_BINARY_DIR}/libmctools_ex00${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )

##############
add_executable(ex00_run_sim_module ex00_run_sim_module.cxx)

target_link_libraries(ex00_run_sim_module
  PUBLIC
  Bayeux::Bayeux
  Bayeux::Bayeux_mctools_geant4
  )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex00_run_sim_module
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

##############
add_executable(ex00_read_plain_simdata ex00_read_plain_simdata.cxx)

target_link_libraries(ex00_read_plain_simdata
  mctools_ex00
  Bayeux::Bayeux
  Bayeux::Bayeux_mctools_geant4
  Boost::program_options
  )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex00_read_plain_simdata
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

##############
add_executable(ex00_read_pipeline_simdata ex00_read_pipeline_simdata.cxx)

target_link_libraries(ex00_read_pipeline_simdata
  mctools_ex00
  Bayeux::Bayeux
  Boost::program_options
  )

install ( FILES
  ${PROJECT_BINARY_DIR}/ex00_read_pipeline_simdata
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt