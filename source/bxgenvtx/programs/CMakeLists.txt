###############################################################################
##
## Copyright (C) 2013 François Mauger, Université de Caen Basse-Normandie
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the genvtx library.
##
## genvtx is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## genvtx is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with genvtx.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################


set (genvtx_prod_programs_list genvtx_production.cxx )

# include files search paths
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${geomtools_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/programs
  )

foreach(prod_program ${genvtx_prod_programs_list})
  get_filename_component(prod_executable ${prod_program} NAME_WE)
  add_executable(${prod_executable} ${prod_program})
  # define d suffix on windows
  if(WIN32)
    set_target_properties(${prod_executable} PROPERTIES DEBUG_POSTFIX d)
  endif()

  set(_prod_libraries genvtx)

  # # Special bio DLL plugins :
  # if(datatools_WITH_BIO)
  #   list(INSERT _prod_libraries 0 datatools_bio)
  # endif()
  # if(mygsl_WITH_BIO)
  #   list(INSERT _prod_libraries 0 mygsl_bio )
  # endif()
  # if(GENVTX_WITH_BIO)
  #   list(INSERT _prod_libraries 0 genvtx_bio )
  # endif()

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
