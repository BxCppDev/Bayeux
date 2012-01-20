###############################################################################
##
## Copyright (C) 2011 François Mauger, Université de Caen Basse-Normandie
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


###############################
# doc
###############################

find_package(Doxygen)
if(DOXYGEN_FOUND)
    # setup variables to be replaced in the source Doxyfile
    set(DOXYGEN_INPUT_DIR ${CMAKE_SOURCE_DIR})
   
    ###################################
    # local documentation
    ###################################

    set(DOXYGEN_OUTPUT_TYPE html)
    set(DOXYGEN_CONFIGURED_INPUT ${CMAKE_BINARY_DIR}/doc/api/datatoolsapi.doxy)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc/api)

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

    # configure the source Doxyfile by copying it and replacing all @variables@
    configure_file ( ${DOXYGEN_INPUT_DIR}/doc/api/datatools.doxygen.in.cmake 
                     ${DOXYGEN_CONFIGURED_INPUT} @ONLY )

    # copy necessary doc files
    file(GLOB image_files ${DOXYGEN_INPUT_DIR}/doc/api/images/*)
    foreach ( file ${image_files} )
        if ( NOT IS_DIRECTORY ${file} )
          get_filename_component( filename ${file} NAME )
          configure_file (${file} ${DOXYGEN_OUTPUT_DIR}/${DOXYGEN_OUTPUT_TYPE}/${filename} COPYONLY )
        endif ()
    endforeach ()

    # target setup
    add_custom_target(doc
        COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR}
    )

    # install###${INSTALL_MISC_DIR}/doc
    install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME} 
        COMPONENT apidoc
    )

endif ()

if ( EXISTS ${CMAKE_SOURCE_DIR}/doc/Memos/DatatoolsContainerTutorial.pdf )
   
    install(FILES ${CMAKE_SOURCE_DIR}/doc/Memos/DatatoolsContainerTutorial.pdf
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}/Memos/pdf
        COMPONENT memo
    )

endif ()

if ( EXISTS ${CMAKE_SOURCE_DIR}/doc/Memos/DatatoolsSerializationTutorial.pdf )
   
    install(FILES ${CMAKE_SOURCE_DIR}/doc/Memos/DatatoolsSerializationTutorial.pdf
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}/Memos/pdf
        COMPONENT memo
    )

endif ()

# end of datatools/doc/CMakeLists.txt
