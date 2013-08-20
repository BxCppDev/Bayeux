cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bxversioning)

find_package(Bayeux REQUIRED)

include_directories(${BAYEUX_INCLUDE_DIRS})

add_executable(bxversioning bxversioning.cc)
target_link_libraries(bxversioning ${BAYEUX_LIBRARIES})

enable_testing()
add_test(runcheck bxversioning)
