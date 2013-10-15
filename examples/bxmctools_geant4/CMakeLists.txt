cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bxmctoolsg4)

find_package(Bayeux REQUIRED)
if(NOT Bayeux_MCTOOLS_GEANT4_FOUND)
  message(FATAL_ERROR "Bayeux does not supply the mctools/geant4 module")
endif()

include_directories(${Bayeux_INCLUDE_DIRS})

add_executable(bxmctoolsg4 bxmctoolsg4.cc)
target_link_libraries(bxmctoolsg4 ${Bayeux_LIBRARIES} ${Bayeux_MCTOOLS_GEANT4_LIBRARY})

enable_testing()
add_test(runcheck bxmctoolsg4)

