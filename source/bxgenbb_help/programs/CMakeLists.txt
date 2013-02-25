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
  ${geomtools_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

foreach ( pro_program ${genbb_help_programs_list_Fortran} )

  unset ( pro_libraries )
  get_filename_component ( pro_executable ${pro_program} NAME_WE )
  message ( STATUS "programs:  Building Fortran target '${pro_executable}'" )
  add_executable ( ${pro_executable} ${pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  if ( GENBB_HELP_WITH_DEBUG )
    add_definitions ( "-g" )
  endif ()

  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  target_link_libraries ( ${pro_executable} 
    genbb_help_f77
    ${mygsl_LIBRARIES} 
    )

endforeach ()

foreach ( pro_program ${genbb_help_programs_list_CXX} )

  unset ( pro_libraries )
  get_filename_component ( pro_executable ${pro_program} NAME_WE )
  message ( STATUS "programs: Building C++ target '${pro_executable}'" )
  add_executable ( ${pro_executable} ${pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  if ( GENBB_HELP_WITH_DEBUG )
    add_definitions ( "-g" )
  endif ()
  
  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  set ( _pro_libraries 
    genbb_help 
    genbb_help_f77
    ${geomtools_LIBRARIES} 
    )
  
  if ( datatools_WITH_BIO )
    list ( INSERT _pro_libraries 0 ${datatools_BIO_LIBRARY} ) 
  endif ()
  if ( geomtools_WITH_BIO )
    list ( INSERT _pro_libraries 0 ${geomtools_BIO_LIBRARY} ) 
  endif ()
  if ( GENBB_HELP_WITH_BIO )
    list ( INSERT _pro_libraries 0 genbb_help_bio ) 
  endif ()
  
  target_link_libraries ( ${pro_executable} ${_pro_libraries} )
  
endforeach ()


install ( FILES 
  ${PROJECT_BINARY_DIR}/programs/decay0 
  ${PROJECT_BINARY_DIR}/programs/genbb1 
  ${PROJECT_BINARY_DIR}/programs/genbbmgr 
  ${PROJECT_BINARY_DIR}/programs/g2b
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE 
  GROUP_READ GROUP_EXECUTE 
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt
