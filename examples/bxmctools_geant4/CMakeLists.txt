cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bxmctoolsg4)

find_package(Bayeux REQUIRED)
if(NOT Bayeux_MCTOOLS_GEANT4_FOUND)
  message(FATAL_ERROR "Bayeux does not supply the mctools/geant4 module")
endif()
if (BAYEUX_WITH_QT_GUI)
  find_package(Qt5Core ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Gui ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Widgets ${BAYEUX_QT5_VERSION} REQUIRED)
  find_package(Qt5Svg ${BAYEUX_QT5_VERSION} REQUIRED)
endif()

include_directories(${Bayeux_INCLUDE_DIRS})

add_executable(bxmctoolsg4 bxmctoolsg4.cc)
target_link_libraries(bxmctoolsg4 ${Bayeux_MCTOOLS_GEANT4_LIBRARY} ${Bayeux_LIBRARIES})

enable_testing()
add_test(runcheck bxmctoolsg4)
