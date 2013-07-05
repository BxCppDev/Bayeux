# - Build datatools documentation
#
#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
#
# Copyright (C) 2011 François Mauger, Université de Caen Basse-Normandie
# Contact: mauger@lpccaen.in2p3.fr
#
# This file is part of the datatools library.
#
# datatools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# datatools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with datatools.  If not, see <http://www.gnu.org/licenses/>.
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

  set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_OUTPUT_DIR}datatoolsapi.doxy)
  set(DOXYGEN_TAGFILE ${DOXYGEN_OUTPUT_DIR}/datatools-${datatools_VERSION}.doxytag)
  set(DOXYGEN_EXTRA_INPUT 
    ${DOXYGEN_INPUT_DIR}/doc/user_guides/Version.markdown
    ${DOXYGEN_INPUT_DIR}/doc/api/HandlingApiChanges.markdown
    )

  if(APPLE)
    set(DATATOOLS_GENERATE_DOCSET NO)
    option(DATATOOLS_WITH_DOCSET "Create Xcode Docset for datatools" OFF)

    if(DATATOOLS_WITH_DOCSET)
      set(DOXYGEN_GENERATE_DOCSET YES)
    endif()
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
    ${DOXYGEN_INPUT_DIR}/doc/api/datatools.doxygen.in.cmake 
    ${DOXYGEN_CONFIGURED_INPUT} 
    @ONLY
    )

  # Run Doxygen...
  add_custom_command(
    OUTPUT  ${DOXYGEN_OUTPUT_DIR}/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
    WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR}
    DEPENDS datatools ${DOXYGEN_CONFIGURED_INPUT} ${DOXYGEN_EXTRA_INPUT}
    COMMENT "Doxygenating datatools"
    )
  set(DATATOOLS_DOC_DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html)

  # Build the docset on Apple
  if(APPLE AND DATATOOLS_WITH_DOCSET)
    add_custom_command(
      OUTPUT  ${DOXYGEN_OUTPUT_DIR}/org.supernemo.datatools.docset
      COMMAND make -C ${DOXYGEN_OUTPUT_DIR}/html docset > /dev/null 2>&1
      COMMAND rm -Rf ${DOXYGEN_OUTPUT_DIR}/org.supernemo.datatools.docset
      COMMAND mv -f  ${DOXYGEN_OUTPUT_DIR}/html/org.supernemo.datatools.docset ${DOXYGEN_OUTPUT_DIR}
      DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html
      COMMENT "Building Xcode Docset"
      )
    list(APPEND DATATOOLS_DOC_DEPENDS ${DOXYGEN_OUTPUT_DIR}/org.supernemo.datatools.docset)
  endif()

  add_custom_target(doc ALL DEPENDS ${DATATOOLS_DOC_DEPENDS})

  # Installation
  install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT   Documentation
    )
endif()

#-----------------------------------------------------------------------
# Build memos?
# Better to integrate with doxygen/pandoc
set(datatools_MEMOS
  ${PROJECT_SOURCE_DIR}/doc/Memos/DatatoolsContainerTutorial.pdf
  ${PROJECT_SOURCE_DIR}/doc/Memos/DatatoolsSerializationTutorial.pdf
  )

foreach(_memo ${datatools_MEMOS})
  if(EXISTS ${_memo})
    install(FILES ${_memo} 
      DESTINATION ${CMAKE_INSTALL_DOCDIR}/Memos
      COMPONENT   Documentation
      )
  endif()
endforeach()

# end of datatools/doc/CMakeLists.txt
