###############################################################################
##
## Copyright (C) 2011-2013 François Mauger, Université de Caen Basse-Normandie
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

file(GLOB datatools_programs_list *.cxx)

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${CLHEP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

foreach(prod_program ${datatools_programs_list})
  get_filename_component(prod_executable ${prod_program} NAME_WE)
  add_executable(${prod_executable} ${prod_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${prod_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_prod_libraries datatools)
  if(DATATOOLS_WITH_REFLECTION)
    list(INSERT _prod_libraries 0 datatools_reflection)
  endif()

  target_link_libraries(${prod_executable} ${_prod_libraries})

  unset(_prod_libraries)

  install ( FILES
    ${PROJECT_BINARY_DIR}/programs/${prod_executable}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

endforeach ()

# end of CMakeLists.txt
