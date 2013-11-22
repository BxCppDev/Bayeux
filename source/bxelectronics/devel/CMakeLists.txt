# - Top level CMakeLists.txt for electronics example program

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
#
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

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(electronics)

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

# - Versioning
set(electronics_VERSION_MAJOR 0)
set(electronics_VERSION_MINOR 1)
set(electronics_VERSION_PATCH 0)
set(electronics_VERSION "${electronics_VERSION_MAJOR}.${electronics_VERSION_MINOR}.${electronics_VERSION_PATCH}")

# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
# Validate that certain paths are relative, otherwise relocation may fail
include(GNUInstallDirs)
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR DATADIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
#   We allow a parent project to change the output root so that electronics
#   can be embedded in another project
if(NOT electronics_EMBEDDED OR NOT electronics_BUILDPRODUCT_DIR)
  set(electronics_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
endif()

set(electronics_EMBEDDED 0)
if (DEFINED electronics_EMBEDDED)
  if (electronics_EMBEDDED)
    set(electronics_EMBEDDED 1)
  endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${electronics_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${electronics_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${electronics_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")

set(electronics_DATA_TAG "electronics-${electronics_VERSION}")

set(electronics_DATAROOTDIR ${electronics_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATAROOTDIR})
set(electronics_DATADIR     ${electronics_DATAROOTDIR}/${electronics_DATA_TAG})

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - Boost
find_package(Boost REQUIRED filesystem)
# - Bayeux
find_package(Bayeux REQUIRED)

#-----------------------------------------------------------------------
# Build the lib and associated testing programs
#
include(standalone_build_lib.cmake)

#-----------------------------------------------------------------------
# Build the test programs
#
enable_testing()
include(standalone_build_tests.cmake)

# end