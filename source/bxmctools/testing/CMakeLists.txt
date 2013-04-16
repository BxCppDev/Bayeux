###############################################################################
##
## Copyright (C) 2013 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the mctools library.
##
## mctools is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## mctools is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with mctools.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################


###file(GLOB mctools_tests_programs test_*.cxx)
set ( mctools_tests_programs
     test_base_step_hit.cxx 
     test_simulated_data_1.cxx 
     test_step_hit_processor_factory.cxx 
)    
if (MCTOOLS_WITH_G4)
   list(APPEND mctools_tests_programs
     test_g4_prng.cxx
     test_g4_detector_construction.cxx
     test_g4_manager.cxx
   )
endif()

###message (STATUS "mctools_tests_programs=${mctools_tests_programs}" )
# list(REMOVE_ITEM mctools_tests_programs ${CMAKE_CURRENT_SOURCE_DIR}/test_reflection_0.cxx)
# if(MCTOOLS_WITH_REFLECTION)
#   list(APPEND mctools_tests_programs ${CMAKE_CURRENT_SOURCE_DIR}/test_reflection_0.cxx)
# endif() 

set(mctools_tests_programs_list "${mctools_tests_programs}") 
list(SORT mctools_tests_programs_list)

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${genvtx_INCLUDE_DIRS}
  ${genbb_help_INCLUDE_DIRS}
  ${materials_INCLUDE_DIRS}
  ${emfield_INCLUDE_DIRS}
  ${dpp_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )
if (MCTOOLS_WITH_G4)
   include_directories(
     ${Geant4_INCLUDE_DIRS}
   )
endif()

set(_test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/testing_output")

set(_mctools_test_additional_options )

# depends on materials data dir:
if (DEFINED materials_DATA_DIR
    AND NOT "x${materials_DATA_DIR}" STREQUAL "x" )
  list (APPEND _mctools_test_additional_options --materials-data-dir ${materials_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_mctools_test_additional_options}'" )
endif ()

# depends on geomtools data dir:
if (DEFINED geomtools_DATA_DIR
    AND NOT "x${geomtools_DATA_DIR}" STREQUAL "x" )
  list (APPEND _mctools_test_additional_options --geomtools-data-dir ${geomtools_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_mctools_test_additional_options}'" )
endif ()

# depends on genvtx data dir:
if (DEFINED genvtx_DATA_DIR
    AND NOT "x${genvtx_DATA_DIR}" STREQUAL "x" )
  list (APPEND _mctools_test_additional_options --genvtx-data-dir ${genvtx_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_mctools_test_additional_options}'" )
endif ()

# depends on genbb_help data dir:
if (DEFINED genbb_help_DATA_DIR
    AND NOT "x${genbb_help_DATA_DIR}" STREQUAL "x" )
  list (APPEND _mctools_test_additional_options --genbb_help-data-dir ${genbb_help_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_mctools_test_additional_options}'" )
endif ()

# depends on emfield data dir:
if (DEFINED emfield_DATA_DIR
    AND NOT "x${emfield_DATA_DIR}" STREQUAL "x" )
  list (APPEND _mctools_test_additional_options --emfield-data-dir ${emfield_DATA_DIR} )
  message ( STATUS "${PROJECT_NAME}: testing: Using option '${_mctools_test_additional_options}'" )
endif ()

execute_process(COMMAND cmake -E remove_directory ${_test_tmp_dir} )

foreach(test_program ${mctools_tests_programs_list})
  get_filename_component(test_executable ${test_program} NAME_WE)
  add_executable(${test_executable} ${test_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${test_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_test_libraries mctools) 

  # Special bio DLL plugins :
  if(datatools_WITH_BIO)
    list(INSERT _test_libraries 0 datatools_bio)
  endif() 
  if(mygsl_WITH_BIO)
    list(INSERT _test_libraries 0 mygsl_bio )
  endif() 
  if(geomtools_WITH_BIO)
    list(INSERT _test_libraries 0 geomtools_bio )
  endif() 
  if(genbb_help_WITH_BIO)
    list(INSERT _test_libraries 0 genbb_help_bio )
  endif() 
  if(MCTOOLS_WITH_BIO)
    list(INSERT _test_libraries 0 mctools_bio )
  endif() 

  # # Special reflection DLL plugins :
  # if(datatools_WITH_REFLECTION)
  #   list(INSERT _test_libraries 0 datatools_reflection)
  # endif() 
  # if(MCTOOLS_WITH_REFLECTION)
  #   list(INSERT _test_libraries 0 mctools_reflection )
  # endif() 

  if (MCTOOLS_WITH_G4)
    list(APPEND _test_libraries mctools_g4 )
    #list(APPEND _test_libraries ${Geant4_LIBRAIRIES} )
  endif()

  target_link_libraries(${test_executable} ${_test_libraries})

  add_test(NAME ${test_executable}.run
    COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
    --tmp-dir ${_test_tmp_dir}
    --data-dir ${PROJECT_SOURCE_DIR} 
    --prefix ${PROJECT_SOURCE_DIR} 
    --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
    ${_mctools_test_additional_options}
    run
    )	 

  unset(_test_libraries)
endforeach ()

add_custom_target(clean_test
  COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
  --prefix ${PROJECT_SOURCE_DIR} 
  --tmp-dir ${_test_tmp_dir}
  clean
  )

# end of CMakeLists.txt
