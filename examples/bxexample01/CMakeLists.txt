cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bxexample01)

find_package(Bayeux REQUIRED NO_MODULE)

include_directories(${Bayeux_INCLUDE_DIRS})

add_executable(bxexample01 bxexample01.cxx)
target_link_libraries(bxexample01 ${Bayeux_LIBRARIES})

enable_testing()
add_test(runcheck bxexample01)
