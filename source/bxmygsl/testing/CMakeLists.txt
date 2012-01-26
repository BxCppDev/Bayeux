###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the mygsl library.
##
## mygsl is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## mygsl is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with mygsl.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set ( mygsl_tests_programs_list
      test_mygsl.cxx 
test_best_value.cxx
test_constants.cxx
test_datapoint.cxx
#test_error.cxx
#test_fft_real2.cxx
#test_fft_real.cxx
test_histogram.cxx
test_interval.cxx
test_ioutils.cxx
test_linear_regression.cxx
test_linear_system_solver.cxx
test_mean.cxx
test_min_max.cxx
#test_multidimensional_minimization.cxx
test_multi_eval.cxx
test_multimin.cxx
test_multiparameter_system.cxx
test_numerical_differentiation.cxx
test_ode.cxx
test_one_dimensional_minimization.cxx
test_one_dimensional_root_finding.cxx
test_permutation.cxx
test_polynomial.cxx
test_rng_2.cxx
test_rng.cxx
test_tabfunc_2.cxx
test_tabfunc_3.cxx
test_tabfunc.cxx
test_unary_eval.cxx
test_von_neumann.cxx
    )

# include files search paths
include_directories ( ${PROJECT_BINARY_DIR}/include
                      ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_SOURCE_DIR}/testing
                    )

if ( _depends_on_datatools ) 
  include_directories ( ${datatools_INCLUDE_DIRS} )
endif ()
include_directories ( ${GSL_INCLUDE_DIR} )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

option ( MYGSL_ALL_TESTS "Perform all possible tests" 1 )
message ( STATUS "testing: Performing all available tests : ${MYGSL_ALL_TESTS}" )

if ( MYGSL_ALL_TESTS )
  set ( _test_tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/tmp" )
  #message ( STATUS "testing:  _test_tmp_dir : ${_test_tmp_dir}" )
  foreach ( test_program ${mygsl_tests_programs_list} )
    unset ( test_libraries )
    get_filename_component ( test_executable ${test_program} NAME_WE )
    message ( STATUS "testing:  Building test target '${test_executable}'" )
    add_executable ( ${test_executable} ${test_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${test_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

    # last thing we have to do is to tell CMake what libraries 
    # our executable needs,
    list ( APPEND test_libraries "mygsl" )
    if ( _depends_on_datatools ) 
      list ( APPEND test_libraries "${datatools_LIBRARIES}" )
    endif ()
    list ( APPEND test_libraries "${GSL_LIBRARIES}" )
	 
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

endif ()

option ( MYGSL_INSTALL_TEST_PROGRAMS "Install test programs" 1 )
message ( STATUS "testing: Performing all available tests : ${MYGSL_INSTALL_TEST_PROGRAMS}" )

# - Installation :
if (MYGSL_INSTALL_TEST_PROGRAMS) 
  set ( _test_install_dir ${CMAKE_INSTALL_DATADIR}/testing )

  #file ( GLOB mygsl_tests_config config/* )
  #install ( FILES ${mygsl_tests_config} 
  #   	      DESTINATION ${_test_install_dir}/config  
  #           PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
  #         )

  install ( FILES ${mygsl_tests_programs} 
            #  mygsl_test_data.h 
            #  mygsl_test_data.cc 
            #  mygsl_test_data.ipp 
	    DESTINATION ${_test_install_dir}
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
          )

endif ()

# end of CMakeLists.txt
