# - Top level CMakeLists.txt for bxexample01 project
#-----------------------------------------------------------------------
# Copyright (c) 2013, François Mauger <mauger@lpccaen.in2p3.fr>

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "bxexample01 requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# Project configuration
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bxexample01)

# - Versioning
set(bxexample01_VERSION_MAJOR 1)
set(bxexample01_VERSION_MINOR 0)
set(bxexample01_VERSION_PATCH 0)
set(bxexample01_VERSION "${bxexample01_VERSION_MAJOR}.${bxexample01_VERSION_MINOR}.${bxexample01_VERSION_PATCH}")

# - Fixup default install prefix on Mac
if(APPLE AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/Library/Frameworks" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif()

##-----------------------------------------------------------------------
# Find external packages needed to build
#
# - Boost
set(bxexample01_Boost_COMPONENTS filesystem program_options)
find_package(Boost 1.53 REQUIRED COMPONENTS ${bxexample01_Boost_COMPONENTS})

# - Bayeux
find_package(Bayeux 1.0.0 REQUIRED)

include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
  ${Bayeux_INCLUDE_DIRS}
  )

set(_bxexample01_libraries
  ${Boost_LIBRARIES}
  ${Bayeux_LIBRARIES}
  )

# RPATH setup
#SET(CMAKE_SKIP_BUILD_RPATH FALSE)
#SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
# LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
# IF("${isSystemDir}" STREQUAL "-1")
#    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# ENDIF("${isSystemDir}" STREQUAL "-1")

add_executable(bxexample01 ../bxexample01.cxx)

target_link_libraries(bxexample01 ${_bxexample01_libraries})

install(TARGETS bxexample01
        DESTINATION bin
        COMPONENT Runtime
  )

# - end