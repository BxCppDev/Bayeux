# Build local kwsys
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(bxdatatools_kwsys)

set(KWSYS_NAMESPACE datatools)
set(KWSYS_USE_DynamicLoader 1)
set(KWSYS_HEADER_ROOT ${PROJECT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR})
add_subdirectory(kwsys-master)

# - end