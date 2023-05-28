# - CMakeLists.txt for mctools serialization validation programs

# cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
# project(mctools_validate_serialization)

# if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
#   message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
#   message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
#   message(STATUS "  CMakeCache.txt")
#   message(STATUS "  CMakeFiles")
#   message(STATUS "Once these files are removed, create a separate directory")
#   message(STATUS "and run CMake from there, pointing it to:")
#   message(STATUS "  ${CMAKE_SOURCE_DIR}")
#   message(FATAL_ERROR "in-source build detected")
# endif()

# - Bayeux :
# find_package(Bayeux 3.3.0 REQUIRED geant4)

option(MCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE "Downgrade to serialization version 1" OFF)

if (MCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE) 
  add_definitions("-DMCTOOLS_VALIDATION_SERIALIZATION_DOWNGRADE")
endif() 

##############
add_executable(exbav exbav.cxx)

target_link_libraries(exbav
  PUBLIC
  Bayeux
  )

# Output to testing subdirectory
set_property(TARGET exbav
  PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  )

##############
add_executable(exstore exstore.cxx)

target_link_libraries(exstore
  PUBLIC
  Bayeux
  )

# Output to testing subdirectory
set_property(TARGET exstore
  PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  )

# install ( FILES
#   ${PROJECT_BINARY_DIR}/exstore
#   DESTINATION ${CMAKE_INSTALL_PREFIX}
#   PERMISSIONS OWNER_READ OWNER_EXECUTE
#   GROUP_READ GROUP_EXECUTE
#   WORLD_READ WORLD_EXECUTE
#   )

##############
add_executable(exload exload.cxx)

target_link_libraries(exload
  PUBLIC
  Bayeux
  )

# Output to testing subdirectory
set_property(TARGET exload
  PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  )

# install ( FILES
#   ${PROJECT_BINARY_DIR}/exload
#   DESTINATION ${CMAKE_INSTALL_PREFIX}
#   PERMISSIONS OWNER_READ OWNER_EXECUTE
#   GROUP_READ GROUP_EXECUTE
#   WORLD_READ WORLD_EXECUTE
#   )

if(BAYEUX_ENABLE_TESTING)
  set(_testname mctools-validation_serialization)
  add_test(NAME ${_testname}
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh
    --work-dir "${CMAKE_CURRENT_BINARY_DIR}" --source-dir "${CMAKE_CURRENT_SOURCE_DIR}"
    )
  set(${_testname}_TEST_ENVIRONMENT "PATH=$ENV{PATH}:${PROJECT_BUILD_BINDIR}")
  set_property(TEST ${_testname}
    APPEND PROPERTY ENVIRONMENT "${${_testname}_TEST_ENVIRONMENT}"
    )
endif(BAYEUX_ENABLE_TESTING)


# end of CMakeLists.txt
