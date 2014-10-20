# - CMake script for Bayeux's Documentation
#

#-----------------------------------------------------------------------
# Copyright 2012,2014 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2014 University of Warwick
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Bayeux. If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------
if(NOT Bayeux_BUILD_DOCS)
  return()
endif()

if(Bayeux_BUILD_DOCS)
  # - For main HTML docs
  find_package(Doxygen 1.8 REQUIRED)
  # - For man pages, pandoc for now
  #find_package(Pandoc 1.10 REQUIRED)
  #find_package(<something to compress man?>)
  find_program(Pandoc_EXECUTABLE pandoc
    DOC "path to pandoc executable"
    )
  set(Pandoc_FOUND ${Pandoc_EXECUTABLE})
endif()

# Default
set(_bxocd_bayeuxclasses_link
  "OCD documentation for Bayeux's classes in not available.")
set(_bxocd_depends Bayeux)
set(_bxgeant4 0)

# This setup
if(Bayeux_WITH_GEANT4)
  set(_bxgeant4 1)
endif()

if (Bayeux_BUILD_OCD_DOCS)
  configure_file(BxOCD/bxocd_build.bash.in BxOCD/bxocd_build.bash @ONLY)
  set(_bxocd_bayeuxclasses_link
    "[OCD documentation for Bayeux's classes](__ocd__index.html)")
endif()

#-----------------------------------------------------------------------
# Set up build of documentation
#

set(Bayeux_DOXYFILE_IN  bayeux_doxygen.conf.in)
set(Bayeux_DOXYFILE_OUT ${PROJECT_BINARY_DIR}/doc/bayeux_doxygen.conf)
set(Bayeux_DOXYGEN_OUTPUT_DIR  ${Bayeux_BUILDPRODUCT_DIR}/share/${Bayeux_DOCUMENTATION_DIR}/API)
set(Bayeux_DOXYGEN_OUTPUT_FILE ${Bayeux_DOXYGEN_OUTPUT_DIR}/html/index.html)
set(Bayeux_DOXYGEN_OUTPUT_IMAGE_DIR ${Bayeux_DOXYGEN_OUTPUT_DIR}/html/images)

#-----------------------------------------------------------------------
# Individual Documents
#
set(Cadfael_VERSION "1.0.1")
configure_file(BxInstallationGuide.md.in BxInstallationGuide.md @ONLY)

configure_file(BxOCD/BxOCD.md.in BxOCD/BxOCD.md @ONLY)

set(Bayeux_MARKDOWN_DOCS
  BxMainpage.md
  Bayeux.1.md
  ${CMAKE_CURRENT_BINARY_DIR}/BxInstallationGuide.md
  ${CMAKE_CURRENT_BINARY_DIR}/BxOCD/BxOCD.md
  )

# Support Geant4 plugin
if(Bayeux_WITH_GEANT4)
  list(APPEND Bayeux_MARKDOWN_DOCS
    BxG4/BxG4Plugin.md
    )
  list(APPEND _bxocd_depends Bayeux_mctools_geant4)
endif()

set(Bayeux_ABSOLUTE_MARKDOWN_DOCS)
foreach(_mdDoc ${Bayeux_MARKDOWN_DOCS})
  get_filename_component(_absMdDoc ${_mdDoc} ABSOLUTE)
  set(Bayeux_ABSOLUTE_MARKDOWN_DOCS "${Bayeux_ABSOLUTE_MARKDOWN_DOCS} ${_absMdDoc}")
endforeach()


#-----------------------------------------------------------------------
# Build doxygen docs
#
configure_file(${Bayeux_DOXYFILE_IN} ${Bayeux_DOXYFILE_OUT} @ONLY)

add_custom_command(
  OUTPUT  ${Bayeux_DOXYGEN_OUTPUT_FILE}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${Bayeux_DOXYGEN_OUTPUT_DIR}
  COMMAND ${DOXYGEN_EXECUTABLE} ${Bayeux_DOXYFILE_OUT}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS Bayeux ${Bayeux_DOXYFILE_OUT} ${Bayeux_MARKDOWN_DOCS}
  COMMENT "Doxygenating Bayeux"
)

if (Bayeux_BUILDS_OCD_DOCS)
  set(Bayeux_OCD_OUTPUT_FILE ${Bayeux_DOXYGEN_OUTPUT_DIR}/html/__ocd__index.html)
  add_custom_command(
    OUTPUT  ${Bayeux_OCD_OUTPUT_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${Bayeux_DOXYGEN_OUTPUT_DIR}
    COMMAND bash ${CMAKE_CURRENT_BINARY_DIR}/BxOCD/bxocd_build.bash
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS  Bayeux ${_bxocd_depends} ${Bayeux_MARKDOWN_DOCS}
    COMMENT "Generating OCD documentation for Bayeux's classes"
    )
endif()

add_custom_target(bayeux_docs ALL DEPENDS ${Bayeux_DOXYGEN_OUTPUT_FILE} ${Bayeux_OCD_OUTPUT_FILE} )

set(__doc_cmake_dir ${Bayeux_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${Bayeux_TAG})

file(RELATIVE_PATH Bayeux_DOXYGEN_HTML_DIR_RELATIVE ${__doc_cmake_dir} ${Bayeux_DOXYGEN_OUTPUT_DIR}/html)
file(RELATIVE_PATH Bayeux_DOXYGEN_TAGFILE_RELATIVE ${__doc_cmake_dir} ${Bayeux_DOXYGEN_OUTPUT_DIR}/bayeux.doxy.tag)

configure_file(BayeuxDocs.cmake.in ${__doc_cmake_dir}/BayeuxDocs.cmake @ONLY)
