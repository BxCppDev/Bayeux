# - CMakeLists.txt for geomtools Python wrapping 
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
# Determine a suitable standard install directory for Python modules
# based on the standard directory for the Python we're using.
# Add the variable to the cache to allow tuning, and also create a variable
# for the absolute install path.
#
if ( NOT DEFINED CMAKE_INSTALL_PYTHONDIR OR "${CMAKE_INSTALL_PYTHONDIR}" STREQUAL "")
    ###message ( STATUS "DEVEL 1: CMAKE_INSTALL_PYTHONDIR='${CMAKE_INSTALL_PYTHONDIR}'")
 
    # Find a suitable default directory based on that supplied by the Python
    # we found.
    # Adapted from technique used in plplot
    execute_process (
        COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print sysconfig.get_python_lib(1,0,prefix='')"
        OUTPUT_VARIABLE _DEFAULT_INSTALL_PYTHONDIR
        OUTPUT_STRIP_TRAILING_WHITESPACE )

    set ( CMAKE_INSTALL_PYTHONDIR ${_DEFAULT_INSTALL_PYTHONDIR}
        CACHE PATH "Installation directory for Python modules and packages" )
endif ()

###message ( STATUS "DEVEL: CMAKE_INSTALL_PYTHONDIR='${CMAKE_INSTALL_PYTHONDIR}'")

mark_as_advanced ( CMAKE_INSTALL_PYTHONDIR )

# Now create a full path to the final install dir based on CMAKE_INSTALL_PREFIX
if (NOT IS_ABSOLUTE ${CMAKE_INSTALL_PYTHONDIR} )
    set ( CMAKE_INSTALL_FULL_PYTHONDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PYTHONDIR}" )
else ()
    set ( CMAKE_INSTALL_FULL_PYTHONDIR ${CMAKE_INSTALL_PYTHONDIR} )
    set ( CMAKE_INSTALL_IS_NONRELOCATABLE 1 )
endif ()


#----------------------------------------------------------------------------
# We set up all include paths at top level since lower level packages are
# very simple, so it's easy to do stuff here.
#
if ( _depends_on_datatools )
  include_directories ( ${datatools_INCLUDE_DIRS} )
endif ()

# We also need Python includes...
include_directories ( ${PYTHON_INCLUDE_DIRS} )

# Recurse into subdirs.
add_subdirectory ( pygeomtools )

# Find all .py files and copy to build directory for easy use
file ( COPY pygeomtools
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
       FILES_MATCHING PATTERN *.py
       PATTERN ".svn" EXCLUDE )

# Install and byte-compile the .py files.
# It's easier to install all the .py's from this level, though we can move this
# down to the module should that be needed.
install ( DIRECTORY pygeomtools
          DESTINATION ${CMAKE_INSTALL_PYTHONDIR}
          FILES_MATCHING PATTERN "*.py"
          PATTERN ".svn" EXCLUDE )

# Now configure an run the byte-compiler script
configure_file ( python_byte_compile.cmake.in
                 ${CMAKE_CURRENT_BINARY_DIR}/python_byte_compile.cmake @ONLY )

install ( SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/python_byte_compile.cmake )

# end of CMakeLists.txt
