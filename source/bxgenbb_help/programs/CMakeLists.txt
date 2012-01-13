###############################################################################
##
## Copyright (C) 2012 François Mauger 
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the genbb_help library.
##
## genbb_help is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## genbb_help is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with genbb_help.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set ( CMAKE_Fortran_FLAGS 
    "-Dfermi_wrap=fermi_wrap_ -Drng_shoot_reset=rng_shoot_reset_ -Drng_shoot_flat=rng_shoot_flat_ "	 
    ) 

set ( CMAKE_CXX_FLAGS 
    "-Dfermi_wrap=fermi_wrap_ -Drng_shoot_reset=rng_shoot_reset_ -Drng_shoot_flat=rng_shoot_flat_ -Dgenbbsub=genbbsub_ -Dset_genbb_random_seed=set_genbb_random_seed_ -Dgrndm=grndm_ -Dgenbb_init_enrange=genbb_init_enrange_ -Dgenevent=genevent_ -Denrange=enrange_"	 
    ) 

set ( genbb_help_programs_list_Fortran
      decay0.F
      genbb1.F
     )

set ( genbb_help_programs_list_CXX
      g2b.cxx
      genbbmgr.cxx
     )

# include files search paths
include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                      ${GEOMTOOLS_INCLUDE_DIRS_DEPEND}
                      ${PROJECT_SOURCE_DIR}/testing
                    )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )
  
foreach ( pro_program ${genbb_help_programs_list_Fortran} )

  unset ( pro_libraries )
  get_filename_component ( pro_executable ${pro_program} NAME_WE )
  message ( STATUS "programs:  Building target '${pro_executable}'" )
  add_executable ( ${pro_executable} ${pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  target_link_libraries ( ${pro_executable} 
                          genbb_help_f77
                          ${MYGSL_LIBRARIES_DEPEND}
                        )

endforeach ()
  
foreach ( pro_program ${genbb_help_programs_list_CXX} )

  unset ( pro_libraries )
  get_filename_component ( pro_executable ${pro_program} NAME_WE )
  message ( STATUS "programs:  Building target '${pro_executable}'" )
  add_executable ( ${pro_executable} ${pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  set ( pro_libraries genbb_help
                      ${GEOMTOOLS_LIBRARIES} 
		      genbb_help_f77
                      ${MYGSL_LIBRARIES_DEPEND}
      )

  if ( GENBB_HELP_WITH_BIO )
    list ( INSERT pro_libraries 0 "genbb_help_bio" ) 
  endif ()
	 
  target_link_libraries ( ${pro_executable} ${pro_libraries} )

endforeach ()

# end of CMakeLists.txt
