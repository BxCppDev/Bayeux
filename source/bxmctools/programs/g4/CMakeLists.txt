# -*- mode: cmake; -*-
###############################################################################
##
## Copyright (C) 2012-2013 François Mauger
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

set (_mctools_g4_programs_list
    ${PROJECT_SOURCE_DIR}/programs/g4/g4_production.cxx
    )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )


# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${genvtx_INCLUDE_DIRS}
  ${genbb_help_INCLUDE_DIRS}
  ${materials_INCLUDE_DIRS}
  ${emfield_INCLUDE_DIRS}
  ${dpp_INCLUDE_DIRS}
  ${Geant4_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

#add_definitions ()

# We refind Boost to add in the program options library
find_package(Boost QUIET COMPONENTS ${datatools_Boost_COMPONENTS} program_options)

foreach ( _mctools_g4_pro_program ${_mctools_g4_programs_list} )

  unset ( _mctools_g4_pro_libraries )
  get_filename_component ( _mctools_g4_pro_executable ${_mctools_g4_pro_program} NAME_WE )
  message ( STATUS "mctools: g4: programs:  Building executable target '${_mctools_g4_pro_executable}'" )
  add_executable ( ${_mctools_g4_pro_executable} ${_mctools_g4_pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${_mctools_g4_pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()


  # last thing we have to do is to tell CMake what libraries
  # our executable needs,
  set ( _mctools_g4_link_libraries mctools )

  # Apply external dependencies :
  list ( APPEND _mctools_g4_link_libraries
	 mctools_g4
       )

  # Special BIO DLL plugins :
  if ( MCTOOLS_WITH_BIO )
    list ( APPEND _mctools_g4_link_libraries mctools_bio )
  endif ()
  if ( genbb_help_WITH_BIO )
    list ( APPEND _mctools_g4_link_libraries genbb_help_bio )
  endif ()
  if ( geomtools_WITH_BIO )
    list ( APPEND _mctools_g4_link_libraries geomtools_bio )
  endif ()
  if ( mygsl_WITH_BIO )
    list ( APPEND _mctools_g4_link_libraries mygsl_bio )
  endif ()

  # Special reflection DLL plugins :
  if(datatools_WITH_REFLECTION)
    list(INSERT mctools_g4_link_libraries 0 datatools_reflection)
  endif()
  if(geomtools_WITH_REFLECTION)
    list(INSERT mctools_g4_link_libraries 0 geomtools_reflection)
  endif()
  if(genbb_help_WITH_REFLECTION)
    list(INSERT mctools_g4_link_libraries 0 genbb_help_reflection)
  endif()
  if(MCTOOLS_WITH_REFLECTION)
    list(INSERT mctools_g4_link_libraries 0 mctools_reflection )
  endif()

  # -Wl,--no-as-needed
  target_link_libraries ( ${_mctools_g4_pro_executable}
    ${Boost_LIBRARIES}
    ${_mctools_g4_link_libraries}
    ${Boost_LIBRARIES}
    )

  install ( FILES ${PROJECT_BINARY_DIR}/programs/g4/${_mctools_g4_pro_executable}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    )

endforeach ()

# end of CMakeLists.txt
