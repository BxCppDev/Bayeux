# --------------------------------------------------
message( STATUS "[info] Define Bayeux dependencies")

if(Bayeux_WITH_GENBB)
  set(BAYEUX_BXDECAY0_MIN_VERSION "1.1.0")
  find_package(BxDecay0 ${BAYEUX_BXDECAY0_MIN_VERSION} REQUIRED CONFIG)
  message(STATUS "Found BxDecay0 ${BxDecay0_VERSION}")
  message(STATUS " - BxDecay0 include dirs : '${BxDecay0_INCLUDE_DIRS}'")
  message(STATUS " - BxDecay0 library dir  : '${BxDecay0_LIBRARY_DIR}'")
  message(STATUS " - BxDecay0 libraries    : '${BxDecay0_LIBRARIES}'")
endif()

#-----------------------------------------------------------------------
# Configure build of Bayeux library
# - Packages
# - Boost
set(BAYEUX_BOOST_MIN_VERSION "1.69.0")
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
message(STATUS "Boost version ${Boost_VERSION} has been detected.")

# Boost 1.64 has not been explicitely tested, so I remove it for safety
set(Bayeux_UNSUPPORTED_BOOST_VERSIONS 106400 106500 106600 106700 106800)
set(Bayeux_SUPPORTED_BOOST_VERSIONS 106900 107100 107200 107300 107400 107500)
message(STATUS "Unsupported Boost versions : ${Bayeux_UNSUPPORTED_BOOST_VERSIONS}.")
message(STATUS "Supported Boost versions   : ${Bayeux_SUPPORTED_BOOST_VERSIONS}.")
set(_bx_boost_ver_index -1)
list(FIND Bayeux_UNSUPPORTED_BOOST_VERSIONS ${Boost_VERSION} _bx_boost_ver_index)
if (NOT _bx_boost_ver_index EQUAL -1)
  message(FATAL_ERROR "Bayeux does not support Boost version ${Boost_VERSION}\n"
    "Please use preferably Boost version in: ${Bayeux_SUPPORTED_BOOST_VERSIONS}")
endif()
set(_bx_boost_ver_index -1)
list(FIND Bayeux_SUPPORTED_BOOST_VERSIONS ${Boost_VERSION} _bx_boost_ver_index)
if (_bx_boost_ver_index EQUAL -1)
  message(STATUS "Bayeux does not officially support Boost version ${Boost_VERSION}\n"
    "but this version may work.\n"
    "In doubt, please use preferably Boost version in: ${Bayeux_SUPPORTED_BOOST_VERSIONS}")
else()
  message(STATUS "Bayeux does officially support Boost version ${Boost_VERSION}\n")
endif()

message(STATUS "Found Boost ${Boost_VERSION}")
message(STATUS " - Boost include dirs : '${Boost_INCLUDE_DIRS}'")
message(STATUS " - Boost library dirs : '${Boost_LIBRARY_DIRS}'")

set(Bayeux_USE_EOS_ARCHIVES 1)
set(Bayeux_USE_EPA_ARCHIVES 0)
if (Boost_VERSION GREATER_EQUAL 106900)
  set(Bayeux_USE_EOS_ARCHIVES 0)
  set(Bayeux_USE_EPA_ARCHIVES 1)
endif()
message(STATUS "Bayeux_USE_EOS_ARCHIVES = ${Bayeux_USE_EOS_ARCHIVES}")
message(STATUS "Bayeux_USE_EPA_ARCHIVES = ${Bayeux_USE_EPA_ARCHIVES}")

if (Boost_VERSION GREATER_EQUAL 106900)
  add_definitions("-DBOOST_MATH_DISABLE_STD_FPCLASSIFY")
endif()

foreach(_boost_lib ${BAYEUX_BOOST_COMPONENTS})
  list(APPEND Bayeux_Boost_LIBRARIES Boost::${_boost_lib})
endforeach()
get_filename_component(Bayeux_BOOST_ROOT ${Boost_INCLUDE_DIR} DIRECTORY)

# - Camp
set(BAYEUX_CAMP_MIN_VERSION "0.8.0")
# set(BAYEUX_CAMP_MIN_VERSION "0.8.4")
# if (BAYEUX_CAMP_LEGACY)
#   set(BAYEUX_CAMP_MIN_VERSION "0.8.0")
# endif()
find_package(CAMP ${BAYEUX_CAMP_MIN_VERSION} REQUIRED NO_MODULE)
message(STATUS "Found CAMP ${CAMP_VERSION} at CAMP_DIR      = '${CAMP_DIR}'")

# - CLHEP
# Find this at the end because ROOT/Geant4 may depend on it
# and thus refind it for us. We don't want to override their
# found version for compatibility reasons.
set(BAYEUX_CLHEP_MIN_VERSION "2.1.3.1")
option(BAYEUX_WITH_CLHEP_MODULE_TRICK "Bayeux uses a specific FindCLHEP.cmake script to locate CLHEP (workaround for Ubuntu 20.04)" OFF)
if (BAYEUX_WITH_CLHEP_MODULE_TRICK)
  if(NOT CLHEP_ROOT_DIR)
    message( FATAL "Please provide the CLHEP_ROOT_DIR env")
  endif()
  find_package(CLHEP ${BAYEUX_CLHEP_MIN_VERSION} REQUIRED MODULE)
else()
  find_package(CLHEP ${BAYEUX_CLHEP_MIN_VERSION} REQUIRED NO_MODULE)
endif()
message(STATUS "  * CLHEP_VERSION      ='${CLHEP_VERSION}'")
message(STATUS "  * CLHEP_INCLUDE_DIRS ='${CLHEP_INCLUDE_DIRS}'")
message(STATUS "  * CLHEP_LIBRARIES    ='${CLHEP_LIBRARIES}'")

# - GSL
set(BAYEUX_GSL_MIN_VERSION "2.4")
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
    if(EXISTS ${ROOT_DIR}/modules/RootNewMacros.cmake)
      include(${ROOT_DIR}/modules/RootNewMacros.cmake)
    endif()
  endif()
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_PREROOT})
  message(STATUS "Found ROOT at ROOT_DIR      = '${ROOT_DIR}'")
endif()

if(Bayeux_WITH_QT)
  # Minimal system Qt5 version
  #  - Ubuntu 14.04: 5.2.1
  #  - Ubuntu 16.04: 5.5.1
  #  - Ubuntu 18.04: 5.9.5
  #  - Ubuntu 20.04: 5.12.8
  #  - Ubuntu 22.04: 5.15.3
  # Linuxbrew : 5.10.0 (March 2019)
  set(BAYEUX_QT5_MIN_VERSION "5.2.1")
  find_package(Qt5Core    ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
  find_package(Qt5Gui     ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
  find_package(Qt5Widgets ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
  find_package(Qt5Svg ${BAYEUX_QT5_MIN_VERSION} REQUIRED)
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
  message(STATUS "Geant4_DIR                  = '${Geant4_DIR}'")
  message(STATUS "Geant4_VERSION              = '${Geant4_VERSION}'")
  if (Geant4_VERSION VERSION_GREATER_EQUAL 10)
    if (NOT BAYEUX_WITH_GEANT4_EXPERIMENTAL)
      message( FATAL_ERROR "Geant4 version ${Geant4_VERSION} is not supported yet!")
    else()
      message(STATUS "Experimental support for Geant4 version ${Geant4_VERSION}.")
    endif()
  endif()
  get_filename_component(Geant4_LIBRARY_DIR "${Geant4_INCLUDE_DIR}/../../lib" ABSOLUTE)
  message(STATUS "Geant4_DEFINITIONS  = '${Geant4_DEFINITIONS}'")
  message(STATUS "Geant4_CXX_FLAGS    = '${Geant4_CXX_FLAGS}'")
  message(STATUS "Geant4_INCLUDE_DIRS = '${Geant4_INCLUDE_DIRS}'")
  message(STATUS "Geant4_LIBRARY_DIR  = '${Geant4_LIBRARY_DIR}'")
  message(STATUS "Geant4_LIBRARIES    = '${Geant4_LIBRARIES}'")
  message(STATUS "Geant4_USE_FILE     = '${Geant4_USE_FILE}'")
endif()

# - Core Includes for Bayeux + Third Party
message(STATUS "CLHEP_INCLUDE_DIRS    = '${CLHEP_INCLUDE_DIRS}'")
if(Bayeux_WITH_ROOT)
  message(STATUS "ROOT_INCLUDE_DIRS     = '${ROOT_INCLUDE_DIRS}'")
endif()
message(STATUS "CAMP_INCLUDE_DIRS     = '${CAMP_INCLUDE_DIRS}'")
message(STATUS "Boost_INCLUDE_DIRS    = '${Boost_INCLUDE_DIRS}'")
#include_directories(SYSTEM ${CLHEP_INCLUDE_DIRS})
#include_directories(SYSTEM ${ROOT_INCLUDE_DIRS})
include_directories(SYSTEM ${CAMP_INCLUDE_DIRS})
#include_directories(SYSTEM ${Boost_INCLUDE_DIRS})


#---------------------------------------------------
