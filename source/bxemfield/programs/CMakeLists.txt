# -*- mode: cmake; -*-
###############################################################################
##
## Copyright (C) 2012 François Mauger
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the emfield library.
##
## emfield is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## emfield is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with emfield.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

file (GLOB _emfield_programs_list
  ${PROJECT_SOURCE_DIR}/programs/*.cxx )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

if (AUTOMATIC_CMAKE_DEPENDENCIES)
  # List of definitions flags to be passed to the compiler :
  set (_emfield_list_of_dependencies_definitions  )
  # List of include directories to be used at build time :
  set (_emfield_list_of_dependencies_include_dirs )
  # List of library targets to be linked against :
  set (_emfield_list_of_dependencies_libraries    )

  # Compute all dependencies directives in one shot :
  if ( _emfield_number_of_dependencies GREATER 0 )
    emfield_home_made_package_dependencies_populate_build_lists (
      "${_emfield_list_of_dependencies_infos}"
      _emfield_list_of_dependencies_definitions
      _emfield_list_of_dependencies_include_dirs
      _emfield_list_of_dependencies_libraries
      )
    endif ()

  if ( _emfield_number_of_third_party_dependencies GREATER 0 )
    emfield_third_party_dependencies_populate_build_lists (
      "${_emfield_list_of_third_party_dependencies_infos}"
      _emfield_list_of_dependencies_definitions
      _emfield_list_of_dependencies_include_dirs
      _emfield_list_of_dependencies_libraries
      )
  endif ()

  add_definitions ( ${_emfield_list_of_dependencies_definitions} )
  include_directories ( ${_emfield_list_of_dependencies_include_dirs} )
endif (AUTOMATIC_CMAKE_DEPENDENCIES)

foreach ( _emfield_pro_program ${_emfield_programs_list} )

  unset ( _emfield_pro_libraries )
  get_filename_component ( _emfield_pro_executable ${_emfield_pro_program} NAME_WE )
  message ( STATUS "emfield: programs:  Building executable target '${_emfield_pro_executable}'" )
  add_executable ( ${_emfield_pro_executable} ${_emfield_pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${_emfield_pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  # include files search paths
  include_directories ( ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
    )

  # last thing we have to do is to tell CMake what libraries
  # our executable needs,
  set ( _emfield_link_libraries emfield )

  # if ( EMFIELD_WITH_BIO )
  #   set (_emfield_link_libraries emfield_bio )
  # endif ()

  target_link_libraries ( ${_emfield_pro_executable}
    ${_emfield_link_libraries}
    )

  install ( FILES ${PROJECT_BINARY_DIR}/programs/${_emfield_pro_executable}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    )

endforeach ()

# end of CMakeLists.txt
