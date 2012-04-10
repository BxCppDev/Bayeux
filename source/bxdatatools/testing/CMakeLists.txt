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

# include files search paths
include_directories ( ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${CLHEP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

# linker search paths
link_directories( ${PROJECT_BINARY_DIR}/src )

option (DATATOOLS_ALL_TESTS "Perform all possible tests" ON)
message ( STATUS "Performing all available tests : ${DATATOOLS_ALL_TESTS}" )

if (DATATOOLS_ALL_TESTS)
  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )

  execute_process (COMMAND cmake -E remove_directory -f ${_test_tmp_dir} )
  #execute_process (COMMAND cmake -E remove -f ${_test_tmp_dir}/tests.log )

  foreach ( test_program ${datatools_tests_programs_list} )
    get_filename_component ( test_executable ${test_program} NAME_WE )
    message ( STATUS "Building test target '${test_executable}'" )
    add_executable ( ${test_executable} ${test_program} )
    # define d suffix on windows
    if(WIN32)
      set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,

    ## Discard this...
    # list (APPEND test_libraries "datatools" ${CLHEP_LIBRARIES} ${Boost_LIBRARIES} )
    # #message ( STATUS "DEVEL ====== Boost_LIBRARIES=${Boost_LIBRARIES}" )
    # #message ( STATUS "DEVEL ====== Boost_PYTHON_LIBRARY=${Boost_PYTHON_LIBRARY}" )
    # if ( Boost_PYTHON_FOUND )
    #    list (APPEND test_libraries ${PYTHON_LIBRARIES} )
    # endif () 
    # list (INSERT test_libraries 0 "datatools_sys" )
    # #list (INSERT test_libraries 0 "datatools_sys_c" )
    # list (APPEND test_libraries "dl" )
    # if ( DATATOOLS_WITH_BIO )
    #   list (INSERT test_libraries 0 "datatools_bio" )
    # endif ()

    ## And try...
    set ( _test_libraries datatools datatools_sys ${CLHEP_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
    if ( DATATOOLS_WITH_BIO )
      list (INSERT _test_libraries 0 datatools_bio )
      #   set ( _test_libraries datatools_bio datatools datatools_sys )
      # else ()
      #   set ( _test_libraries datatools datatools_sys )
    endif ()
    # if ( DATATOOLS_WITH_BIO )
    #   set ( _test_libraries datatools_bio datatools datatools_sys )
    # else ()
    #   set ( _test_libraries datatools datatools_sys )
    # endif ()

    # if ( DATATOOLS_WITH_BIO )
    #   set ( _test_libraries datatools_bio )
    # else ()
    #   set ( _test_libraries datatools )
    # endif ()

    #message ( STATUS "DEVEL ===== Test libraries : ${test_libraries}" )

    target_link_libraries ( ${test_executable} ${_test_libraries} )

    add_test ( NAME ${test_executable}.run
      COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
      --tmp-dir ${_test_tmp_dir}
      --prefix ${PROJECT_SOURCE_DIR} 
      --exe $<TARGET_FILE_DIR:${test_executable}>/$<TARGET_FILE_NAME:${test_executable}>
      run
      )	 
	
    unset ( _test_libraries )

  endforeach ()
  
  add_custom_target ( clean_test
    COMMAND ${PROJECT_SOURCE_DIR}/testing/testDriver.bash 
    --prefix ${PROJECT_SOURCE_DIR} 
    --tmp-dir ${_test_tmp_dir}
    clean
    )

  set ( _test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )

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
  
  unset ( _test_install_dir )

endif ()

# end of CMakeLists.txt
