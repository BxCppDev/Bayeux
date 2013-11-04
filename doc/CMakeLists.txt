# - CMake script for Bayeux's Documentation
#

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
#
# This file is part of Bayeux.
#
# Bayeux is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Bayeux is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------
if(NOT Bayeux_BUILD_DOCS)
  return()
endif()

#-----------------------------------------------------------------------
# Set up build of documentation
#
find_package(Doxygen 1.8.0 REQUIRED)

set(Bayeux_DOXYFILE_IN  bayeux_doxygen.conf.in)
set(Bayeux_DOXYFILE_OUT ${PROJECT_BINARY_DIR}/doc/bayeux_doxygen.conf)
set(Bayeux_DOXYGEN_OUTPUT_DIR  ${Bayeux_BUILDPRODUCT_DIR}/share/${Bayeux_DOCUMENTATION_DIR}/API)
set(Bayeux_DOXYGEN_OUTPUT_FILE ${Bayeux_DOXYGEN_OUTPUT_DIR}/html/index.html)

#-----------------------------------------------------------------------
# Build doxygen docs
#
configure_file(${Bayeux_DOXYFILE_IN} ${Bayeux_DOXYFILE_OUT} @ONLY)

add_custom_command(
  OUTPUT  ${Bayeux_DOXYGEN_OUTPUT_FILE}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${Bayeux_DOXYGEN_OUTPUT_DIR}
  COMMAND ${DOXYGEN_EXECUTABLE} ${Bayeux_DOXYFILE_OUT}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS Bayeux ${Bayeux_DOXYFILE_OUT}
  COMMENT "Doxygenating Bayeux"
  )

add_custom_target(bayeux_docs ALL DEPENDS ${Bayeux_DOXYGEN_OUTPUT_FILE})

configure_file(BayeuxDocs.cmake.in
  ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG}/BayeuxDocs.cmake
  @ONLY
  )
