# --------------------------------------------------
message( STATUS "[info] Define Bayeux dependencies")


#-----------------------------------------------------------------------
# Configure build of Bayeux library
# - Packages
# - Boost
set(BAYEUX_BOOST_MIN_VERSION "1.63.0")
set(BAYEUX_BOOST_COMPONENTS
  filesystem
  system
  serialization
  iostreams
  program_options
  regex
  thread
  )
if(Bayeux_WITH_GEANT4_MODULE)
  # Also need Boost.Thread for Geant4 module:
  list(APPEND BAYEUX_BOOST_COMPONENTS thread)
endif()

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost ${BAYEUX_BOOST_MIN_VERSION}
  REQUIRED
  ${BAYEUX_BOOST_COMPONENTS}
  )
if (Boost_VERSION VERSION_EQUAL 106800)
  message(STATUS "Boost version ${Boost_VERSION} has been detected.")
  message(FATAL_ERROR "Bayeux does not support Boost version 1.68.0\n" "Please use Boost 1.63, 1.65.1 or 1.69.0")
endif()
message(STATUS "Found Boost ${Boost_VERSION}")
message(STATUS " - Boost include dirs : '${Boost_INCLUDE_DIRS}'")
message(STATUS " - Boost library dirs : '${Boost_LIBRARY_DIRS}'")

foreach(_boost_lib ${BAYEUX_BOOST_COMPONENTS})
  list(APPEND Bayeux_Boost_LIBRARIES Boost::${_boost_lib})
endforeach()
get_filename_component(Bayeux_BOOST_ROOT ${Boost_INCLUDE_DIR} DIRECTORY)

# - Camp
set(BAYEUX_CAMP_MIN_VERSION "0.8.0")
find_package(CAMP ${BAYEUX_CAMP_MIN_VERSION} REQUIRED NO_MODULE)
message(STATUS "Found CAMP at CAMP_DIR      = '${CAMP_DIR}'")

# - CLHEP
# Find this at the end because ROOT/Geant4 may depend on it
# and thus refind it for us. We don't want to override their
# found version for compatibility reasons.
set(BAYEUX_CLHEP_MIN_VERSION "2.1.3.1")
find_package(CLHEP ${BAYEUX_CLHEP_MIN_VERSION} REQUIRED NO_MODULE)
message(STATUS "Found CLHEP at CLHEP_DIR    = '${CLHEP_DIR}'")

# - GSL
set(BAYEUX_GSL_MIN_VERSION "2.1")
find_package(GSL ${BAYEUX_GSL_MIN_VERSION} REQUIRED)

# - ROOT
if(Bayeux_WITH_ROOT)
  set(BAYEUX_ROOT_MIN_VERSION "6.08.00")
  set(BAYEUX_ROOT_COMPONENTS Core Hist RIO Tree)
  set(CMAKE_MODULE_PATH_PREROOT ${CMAKE_MODULE_PATH})
  find_package(ROOT ${BAYEUX_ROOT_MIN_VERSION} REQUIRED COMPONENTS ${BAYEUX_ROOT_COMPONENTS})
  if(ROOT_VERSION VERSION_LESS 6)
    include(RootNewMacros)
  else()
    include(${ROOT_DIR}/modules/RootNewMacros.cmake)
  endif()
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_PREROOT})
  message(STATUS "Found ROOT at ROOT_DIR      = '${ROOT_DIR}'")
endif()

if(Bayeux_WITH_QT)
  # Minimal system Qt5 version
  #  - Ubuntu 14.04: 5.2.1
  #  - Ubuntu 16.04: 5.5.1
  #  - Ubuntu 18.04: 5.9.5
  # Linuxbrew : 5.10.0 (March 2019)
  set(BAYEUX_QT5_MIN_VERSION "5.2.1")
  find_package(Qt5Core    ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
  find_package(Qt5Gui     ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
  find_package(Qt5Widgets ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
  find_package(Qt5Svg     ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
  message(STATUS "Found Qt5 at Qt5Core_DIR    = '${Qt5Core_DIR}'")
  message(STATUS "Found Qt5 at Qt5Gui_DIR     = '${Qt5Gui_DIR}'")
  message(STATUS "Found Qt5 at Qt5Widgets_DIR = '${Qt5Widgets_DIR}'")
  message(STATUS "Found Qt5 at Qt5Svg_DIR     = '${Qt5Svg_DIR}'")
endif()

# - Geant4 if building the MCTools extension library
# We do this here because we may need to configure files in MCTools
# using the results of the search
if(Bayeux_WITH_GEANT4_MODULE)
  set(BAYEUX_GEANT4_VERSION "9.6.0")
  find_package(Geant4 ${BAYEUX_GEANT4_VERSION} REQUIRED ui_all vis_all gdml NO_MODULE)
  if(Geant4_DATASETS)
    set(MCTOOLS_G4_DATA_ASSIGNMENTS)
    foreach(_ds ${Geant4_DATASETS})
      list(APPEND MCTOOLS_G4_DATA_ASSIGNMENTS "(\"${Geant4_DATASET_${_ds}_ENVVAR}\", \"${Geant4_DATASET_${_ds}_PATH}\")")
    endforeach()
    string(REPLACE ";" "" MCTOOLS_G4_DATA_ASSIGNMENTS ${MCTOOLS_G4_DATA_ASSIGNMENTS})
  endif()
  # message(STATUS "Geant4_DEFINITIONS  = '${Geant4_DEFINITIONS}'")
  # message(STATUS "Geant4_INCLUDE_DIRS = '${Geant4_INCLUDE_DIRS}'")
  # message(STATUS "Geant4_LIBRARIES    = '${Geant4_LIBRARIES}'")
  message(STATUS "Geant4_DIR                  = '${Geant4_DIR}'")
endif()

# - Core Includes for Bayeux + Third Party
#include_directories(SYSTEM ${CLHEP_INCLUDE_DIRS})
#include_directories(SYSTEM ${ROOT_INCLUDE_DIRS})
#include_directories(${CAMP_INCLUDE_DIRS})
#include_directories(SYSTEM ${Boost_INCLUDE_DIRS})


#---------------------------------------------------
