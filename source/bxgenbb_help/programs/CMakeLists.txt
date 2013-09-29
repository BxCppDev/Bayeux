###############################################################################
##
## Copyright (C) 2013 François Mauger
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the genbb_help library.
##
## genbb_help is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## genbb_help is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with genbb_help.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

if (GENBB_HELP_WITH_FORTRAN)

  set ( genbb_help_programs_list_Fortran
    decay0.F
    genbb1.F
  )

endif (GENBB_HELP_WITH_FORTRAN)

set ( genbb_help_programs_list_CPP
  g2b.cxx
  genbbmgr.cxx
)

if(GENBB_HELP_WITH_INSPECTOR)
  message ( STATUS "${PROJECT_NAME}: programs: Support the 'genbb_inspector' utility" )
  list (INSERT genbb_help_programs_list_CPP 0 genbb_inspector.cxx)
endif()

# We refind Boost to add in the program options library
set(Boost_COMPONENTS)
find_package(Boost 1.47 QUIET COMPONENTS ${Boost_COMPONENTS} program_options)

if(GENBB_HELP_WITH_INSPECTOR)
  # - dpp
  find_package(dpp 1.0.0 REQUIRED)
endif()

# include files search paths
include_directories ( ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/include
  ${geomtools_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/testing
  )

if(GENBB_HELP_WITH_INSPECTOR)
  include_directories (	${dpp_INCLUDE_DIRS} )
endif()

# linker search paths :
link_directories ( ${PROJECT_BINARY_DIR}/src )

set (_genbb_fortran_libs)
if (GENBB_HELP_WITH_FORTRAN)

  foreach ( pro_program ${genbb_help_programs_list_Fortran} )

    unset ( pro_libraries )
    get_filename_component ( pro_executable ${pro_program} NAME_WE )
    message ( STATUS "${PROJECT_NAME}: programs: Building Fortran target '${pro_executable}'" )
    add_executable ( ${pro_executable} ${pro_program} )
    # define d suffix on windows
    if (WIN32)
      set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
    endif ()

    if ( GENBB_HELP_WITH_DEBUG )
      add_definitions ( "-g" )
    endif ()

    # last thing we have to do is to tell CMake what libraries
    # our executable needs,
    target_link_libraries ( ${pro_executable}
      genbb_help_decay0_fortran
      ${mygsl_LIBRARIES}
      )

  endforeach ()

  install ( FILES
    ${PROJECT_BINARY_DIR}/programs/decay0
    ${PROJECT_BINARY_DIR}/programs/genbb1
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

  set (_genbb_fortran_libs genbb_help_decay0_fortran)

endif (GENBB_HELP_WITH_FORTRAN)

foreach ( pro_program ${genbb_help_programs_list_CPP} )

  unset ( pro_libraries )
  get_filename_component ( pro_executable ${pro_program} NAME_WE )
  message ( STATUS "${PROJECT_NAME}: programs: Building C++ target '${pro_executable}'" )
  add_executable ( ${pro_executable} ${pro_program} )
  # define d suffix on windows
  if (WIN32)
    set_target_properties ( ${pro_executable} PROPERTIES DEBUG_POSTFIX d )
  endif ()

  if ( GENBB_HELP_WITH_DEBUG )
    add_definitions ( "-g" )
  endif ()

  # last thing we have to do is to tell CMake what libraries
  # our executable needs,
  set ( _pro_libraries genbb_help )


  if(GENBB_HELP_WITH_INSPECTOR)
    list ( INSERT _pro_libraries 0 ${dpp_LIBRARIES} )
  endif ()

  # Special bio DLL plugins :
  if ( mygsl_WITH_BIO )
    list ( INSERT _pro_libraries 0 ${mygsl_BIO_LIBRARY} )
  endif ()
  if ( geomtools_WITH_BIO )
    list ( INSERT _pro_libraries 0 ${geomtools_BIO_LIBRARY} )
  endif ()
  if ( GENBB_HELP_WITH_BIO )
    list ( INSERT _pro_libraries 0 genbb_help_bio )
  endif ()

  if ( ${pro_program} STREQUAL "genbb_inspector.cxx" )
    message (STATUS "${PROJECT_NAME}: programs: datatools_REFLECTION_LIBRARY=${datatools_REFLECTION_LIBRARY}")
    message (STATUS "${PROJECT_NAME}: programs: datatools_BIO_LIBRARY=${datatools_BIO_LIBRARY}")
    # Special reflection DLL plugins :
    if(datatools_WITH_REFLECTION)
      list(INSERT _pro_libraries 0 ${datatools_REFLECTION_LIBRARY})
    endif()
    if(geomtools_WITH_REFLECTION)
      LIST(INSERT _pro_libraries 0 ${geomtools_REFLECTION_LIBRARY} )
    endif()
    if(GENBB_HELP_WITH_REFLECTION)
      LIST(INSERT _pro_libraries 0 genbb_help_reflection )
    endif()
  endif()


  message (STATUS "${PROJECT_NAME}: programs: Link libraries='${_pro_libraries}'")
  target_link_libraries ( ${pro_executable} ${_pro_libraries} ${Boost_LIBRARIES})

endforeach ()


install ( FILES
  ${PROJECT_BINARY_DIR}/programs/genbbmgr
  ${PROJECT_BINARY_DIR}/programs/g2b
  ${PROJECT_BINARY_DIR}/programs/genbb_inspector
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt
