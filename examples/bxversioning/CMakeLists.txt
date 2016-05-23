cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bxversioning)

find_package(Bayeux REQUIRED)
if (BAYEUX_WITH_QT_GUI)
  find_package(Qt5Core ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Gui ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Widgets ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Svg ${BAYEUX_QT5_VERSION} REQUIRED)
endif()

include_directories(${Bayeux_INCLUDE_DIRS})

add_executable(bxversioning bxversioning.cc)
target_link_libraries(bxversioning ${Bayeux_LIBRARIES})

enable_testing()
add_test(runcheck bxversioning)
