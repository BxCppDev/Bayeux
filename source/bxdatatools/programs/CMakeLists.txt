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

#-----------------------------------------------------------------------
# Configure build of programs...
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${CLHEP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  )

set(_prod_libraries datatools) 
if(DATATOOLS_WITH_REFLECTION)
  list(INSERT _prod_libraries 0 datatools_reflection)
endif()

#-----------------------------------------------------------------------
# The ocd_manual app...
# We refind Boost to add in the program options library
find_package(Boost QUIET COMPONENTS ${datatools_Boost_COMPONENTS} program_options)

add_executable(ocd_manual ocd_manual.cxx)
target_link_libraries(ocd_manual ${_prod_libraries} ${Boost_LIBRARIES})

install(TARGETS ocd_manual
  DESTINATION bin
  COMPONENT Runtime
  )
  
