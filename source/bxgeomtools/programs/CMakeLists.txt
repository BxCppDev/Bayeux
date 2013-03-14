###############################################################################
##
## Copyright (C) 2012-2013 François Mauger 
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the geomtools library.
##
## geomtools is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## geomtools is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with geomtools.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set ( geomtools_programs_list_CXX
  geomtools_check_setup.cxx
  )

# include files search paths
include_directories ( ${PROJECT_SOURCE_DIR}/include	
  ${PROJECT_BINARY_DIR}/include	
  ${mygsl_INCLUDE_DIRS}
  ${materials_INCLUDE_DIRS}
  )

if(GEOMTOOLS_WITH_ROOT_DISPLAY)
include_directories ( ${ROOT_INCLUDE_DIR} )	
endif() 

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

foreach ( pro_program ${geomtools_programs_list_CXX} )

  unset ( pro_libraries )
  get_filename_component ( pro_executable ${pro_program} NAME_WE )
  message ( STATUS "programs: Building C++ target '${pro_executable}'" )
  add_executable ( ${pro_executable} ${pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  if ( GEOMTOOLS_WITH_DEBUG )
    add_definitions ( "-g" )
  endif ()
  
  # last thing we have to do is to tell CMake what libraries 
  # our executable needs,
  set ( _pro_libraries 
    geomtools 
    ${geomtools_LIBRARIES} 
    )
   
  # Special bio DLL plugins :
  if(datatools_WITH_BIO)
    list(INSERT _pro_libraries 0 datatools_bio)
  endif() 
  if(mygsl_WITH_BIO)
    list(INSERT _pro_libraries 0 mygsl_bio )
  endif() 
  if(GEOMTOOLS_WITH_BIO)
    list(INSERT _pro_libraries 0 geomtools_bio )
  endif() 

  if(GEOMTOOLS_WITH_ROOT_DISPLAY)
    list(APPEND _pro_libraries ${ROOT_LIBRARIES} -lGeom)
  endif()	

  target_link_libraries ( ${pro_executable} ${_pro_libraries} )
  
endforeach ()


install ( FILES 
  ${PROJECT_BINARY_DIR}/programs/geomtools_check_setup
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE 
  GROUP_READ GROUP_EXECUTE 
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt
