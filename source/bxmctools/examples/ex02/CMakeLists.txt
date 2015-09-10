# - Top level CMakeLists.txt for mctools ex00 example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(mctools_ex02)

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
find_package(Bayeux 1.0.0 REQUIRED)
set(_incs ${Bayeux_INCLUDE_DIRS})
set(_libs ${Bayeux_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR} ${_incs})

###########
add_library(mctools_ex02 SHARED ex02_inspector.h ex02_inspector.cc)

install( FILES
         ${PROJECT_BINARY_DIR}/libmctools_ex02${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )

##############
add_executable(ex02_read_plain_simdata ex02_read_plain_simdata.cxx)

target_link_libraries(ex02_read_plain_simdata mctools_ex02
					      ${_libs}
					      ${Boost_LIBRARIES}
                     )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex02_read_plain_simdata
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

##############
# add_executable(ex02_run_sim_module ex02_run_sim_module.cxx)

# target_link_libraries(ex02_run_sim_module
#   ${Bayeux_MCTOOLS_GEANT4_LIBRARY}
#   ${_libs}
#   ${Boost_LIBRARIES} )

# install ( FILES
#     ${PROJECT_BINARY_DIR}/ex02_run_sim_module
#     DESTINATION ${CMAKE_INSTALL_PREFIX}
#     PERMISSIONS OWNER_READ OWNER_EXECUTE
#     GROUP_READ GROUP_EXECUTE
#     WORLD_READ WORLD_EXECUTE
#   )

##############
# add_executable(ex02_read_pipeline_simdata ex02_read_pipeline_simdata.cxx)

# target_link_libraries(ex02_read_pipeline_simdata mctools_ex02
# 					      ${_libs}
# 					      ${Boost_LIBRARIES} )

# install ( FILES
#     ${PROJECT_BINARY_DIR}/ex02_read_pipeline_simdata
#     DESTINATION ${CMAKE_INSTALL_PREFIX}
#     PERMISSIONS OWNER_READ OWNER_EXECUTE
#     GROUP_READ GROUP_EXECUTE
#     WORLD_READ WORLD_EXECUTE
#   )

# end of CMakeLists.txt