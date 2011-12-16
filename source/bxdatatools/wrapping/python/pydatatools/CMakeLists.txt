# - CMakeLists.txt for datatools Python wrapper
#
#----------------------------------------------------------------------------
# Copyright 2011 Ben Morgan <Ben.Morgan@warwick.ac.uk>
# Copyright 2011 University of Warwick
#----------------------------------------------------------------------------
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Create the shared module
#

add_library ( pydatatools MODULE pydatatools.cc )

#message ( STATUS "DEVEL====> ${PYTHON_INCLUDE_DIRS}" )
#message ( STATUS "DEVEL====> ${PYTHON_LIBRARIES}" )

target_link_libraries ( pydatatools datatools ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES} )

#----------------------------------------------------------------------------
# Since we've created a module, we need to change the name appropriately.
#
set_target_properties ( pydatatools PROPERTIES PREFIX "" )

# A python shared library module has to be ".pyd" on Windows (all win32)?
if ( WIN32 )
    set_target_properties ( pydatatools PROPERTIES SUFFIX ".pyd" )
endif ()

#----------------------------------------------------------------------------
# Finally, need to install the module...
#
install ( TARGETS pydatatools DESTINATION ${CMAKE_INSTALL_PYTHONDIR} )

