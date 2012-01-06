###############################################################################
##
## Copyright (C) 2011 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the datatools library.
##
## datatools is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## datatools is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with datatools.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################


file ( GLOB datatools_tests_programs *.cxx )

set ( datatools_tests_programs_list "${datatools_tests_programs}" ) 
list ( SORT datatools_tests_programs_list )
#message ( STATUS "************ PROJECT_BINARY_DIR   : ${PROJECT_BINARY_DIR}" )

# include files search paths
include_directories (
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
    ${CLHEP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/testing
)

if ( NOT DATATOOLS_WITH_EMBEDDED_KWSYS )
  include_directories (
      ${KWSYS_INCLUDE_DIRS}
  )
endif ()

# linker search paths
link_directories( ${PROJECT_BINARY_DIR}/src )

#message ( STATUS "CLHEP libraries : ${CLHEP_LIBRARIES}" )
#message ( STATUS "KWSYS libraries : ${KWSYS_LIBRARIES}" )

option (DATATOOLS_ALL_TESTS "Perform all possible tests" ON)
message ( STATUS "Performing all available tests : ${DATATOOLS_ALL_TESTS}" )

if (DATATOOLS_ALL_TESTS)
set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
#message ( STATUS "_test_tmp_dir : ${_test_tmp_dir}" )
foreach ( test_program ${datatools_tests_programs_list} )
  unset ( test_libraries )
  get_filename_component ( test_executable ${test_program} NAME_WE )
  message ( STATUS "Building test target '${test_executable}'" )
  add_executable ( ${test_executable} ${test_program} )
  # define d suffix on windows
  if(WIN32)
    set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
  endif()

  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  list (APPEND test_libraries "datatools" ${CLHEP_LIBRARIES} ${Boost_LIBRARIES} )
  #message ( STATUS "DEVEL ====== Boost_LIBRARIES=${Boost_LIBRARIES}" )
  #message ( STATUS "DEVEL ====== Boost_PYTHON_LIBRARY=${Boost_PYTHON_LIBRARY}" )
  if ( Boost_PYTHON_FOUND )
     list (APPEND test_libraries ${PYTHON_LIBRARIES} )
  endif () 
  #${PYTHON_LIBRARIES}

  if ( NOT DATATOOLS_WITH_EMBEDDED_KWSYS )
    list (APPEND test_libraries ${KWSYS_LIBRARIES} )
  else ()
    list (INSERT test_libraries 0 "kwsys_c" )
    list (INSERT test_libraries 0 "kwsys" )
    list (APPEND test_libraries "dl" )
  endif ()
	 
  if ( DATATOOLS_WITH_BIO )
    list (INSERT test_libraries 0 "datatools_bio" )
  endif ()

  #message ( STATUS "DEVEL ===== Test libraries : ${test_libraries}" )

  target_link_libraries ( ${test_executable} ${test_libraries} )

  add_test ( NAME ${test_executable}.run
             COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                     --tmp-dir ${_test_tmp_dir}
                     --prefix ${PROJECT_SOURCE_DIR} 
                     --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
		     run
           )	 	

endforeach ()
  
add_custom_target ( clean_test
             COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
                     --prefix ${PROJECT_SOURCE_DIR} 
                     --tmp-dir ${_test_tmp_dir}
                     clean
                  )

set ( _test_install_dir ${INSTALL_MISC_DIR}/testing )

file ( GLOB datatools_tests_config config/*.conf )

install ( FILES ${datatools_tests_config} 
	        config/test_smart_filename_2.lis
	  DESTINATION ${_test_install_dir}/config
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
        )

install ( FILES ${datatools_tests_programs} 
                datatools_test_data.h
                datatools_test_my_data.h 
                datatools_test_my_data.cc 
                datatools_test_my_data.ipp 
                dummy_service.h 
                dummy_service.cc 
	  DESTINATION ${_test_install_dir}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
        )

endif ()

# end of CMakeLists.txt
