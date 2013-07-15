###############################################################################
##
## Copyright (C) 2013 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the materials library.
##
## materials is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## materials is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with materials.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

set (materials_prod_programs_list materials_inspector.cxx )

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${datatools_INCLUDE_DIRS}
  ${materials_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/programs
  )

foreach(prod_program ${materials_prod_programs_list})
  get_filename_component(prod_executable ${prod_program} NAME_WE)
  add_executable(${prod_executable} ${prod_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${prod_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_prod_libraries materials)

  target_link_libraries(${prod_executable} ${_prod_libraries})

    install ( FILES
    ${PROJECT_BINARY_DIR}/programs/${prod_executable}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

  unset(_prod_libraries)
endforeach ()

# end of CMakeLists.txt
