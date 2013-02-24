# - Build emfield documentation
#
#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
#
# Copyright (C) 2011 François Mauger, Université de Caen Basse-Normandie
# Contact: mauger@lpccaen.in2p3.fr
#
# This file is part of the emfield library.
#
# emfield is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# emfield is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with emfield.  If not, see <http://www.gnu.org/licenses/>.
#

#-----------------------------------------------------------------------
# Doxygen
#
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  # setup variables to be replaced in the source Doxyfile
  set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR})
  set(DOXYGEN_OUTPUT_TYPE html)
  set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/doc/api)

  set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_OUTPUT_DIR}emfieldapi.doxy)
  set(DOXYGEN_TAGFILE ${DOXYGEN_OUTPUT_DIR}/emfield-${emfield_VERSION}.doxytag)

  if(APPLE)
    set(DOXYGEN_GENERATE_DOCSET YES)
  else()
    set(DOXYGEN_GENERATE_DOCSET NO)
  endif()

  if(WIN32)
    set(DOXYGEN_GENERATE_MAN NO)
    find_program(HHC_PROGRAM 
      NAMES hhc.exe 
      PATHS "c:/Program Files/HTML Help Workshop"
      DOC "HTML Help Compiler program")
    if(HHC_PROGRAM)
      set(DOXYGEN_GENERATE_HTMLHELP YES)
    else()
      set(DOXYGEN_GENERATE_HTMLHELP NO)
    endif()
  else()
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_GENERATE_HTMLHELP NO)
    set(HHC_PROGRAM)
  endif()

  # configure the source Doxyfile by copying it and replacing 
  # all @variables@
  configure_file(
    ${DOXYGEN_INPUT_DIR}/doc/api/emfield.doxygen.in.cmake 
    ${DOXYGEN_CONFIGURED_INPUT} 
    @ONLY
    )

  # Run Doxygen...
  add_custom_command(
    OUTPUT  ${DOXYGEN_OUTPUT_DIR}/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
    WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR}
    DEPENDS emfield ${DOXYGEN_CONFIGURED_INPUT}
    COMMENT "Doxygenating emfield"
    )
  set(EMFIELD_DOC_DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html)

  # Build the docset on Apple
  if(APPLE)
    add_custom_command(
      OUTPUT  ${DOXYGEN_OUTPUT_DIR}/org.supernemo.emfield.docset
      COMMAND make -C ${DOXYGEN_OUTPUT_DIR}/html docset
      COMMAND rm -Rf ${DOXYGEN_OUTPUT_DIR}/org.supernemo.emfield.docset
      COMMAND mv -f  ${DOXYGEN_OUTPUT_DIR}/html/org.supernemo.emfield.docset ${DOXYGEN_OUTPUT_DIR}
      DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html
      COMMENT "Building Xcode Docset"
      )
    list(APPEND EMFIELD_DOC_DEPENDS ${DOXYGEN_OUTPUT_DIR}/org.supernemo.emfield.docset)
  endif()

  add_custom_target(doc ALL DEPENDS ${EMFIELD_DOC_DEPENDS})

  # Installation
  install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT   Documentation
    )
endif()

# end of emfield/doc/CMakeLists.txt
