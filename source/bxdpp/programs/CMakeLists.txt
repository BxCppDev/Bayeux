###############################################################################
##
## Copyright (C) 2012-2013 François Mauger 
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the dpp library.
##
## dpp is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## dpp is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with dpp.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set ( dpp_programs_list_CXX
  dpp_processing.cxx
  )

# include files search paths
include_directories ( ${PROJECT_SOURCE_DIR}/include	
  ${PROJECT_BINARY_DIR}/include	
  ${datatools_INCLUDE_DIRS}
  ${brio_INCLUDE_DIRS}
  ${cuts_INCLUDE_DIRS}
  ${mygsl_INCLUDE_DIRS}
  )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

foreach ( pro_program ${dpp_programs_list_CXX} )

  unset ( pro_libraries )
  get_filename_component ( pro_executable ${pro_program} NAME_WE )
  message ( STATUS "programs: Building C++ target '${pro_executable}'" )
  add_executable ( ${pro_executable} ${pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  if ( DPP_WITH_DEBUG )
    add_definitions ( "-g" )
  endif ()
  
  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  ### message ( STATUS "programs: dpp_LIBRARIES='${dpp_LIBRARIES}'" )
  ### message ( STATUS "programs: Boost_LIBRARIES='${Boost_LIBRARIES}'" )
  set ( _pro_libraries 
    dpp 
    ${dpp_LIBRARIES} 
    ${Boost_LIBRARIES}
    )
  
  target_link_libraries ( ${pro_executable} ${_pro_libraries} )
  
endforeach ()


install ( FILES 
  ${PROJECT_BINARY_DIR}/programs/dpp_processing
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE 
  GROUP_READ GROUP_EXECUTE 
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt
