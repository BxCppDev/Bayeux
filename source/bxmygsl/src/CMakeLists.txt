# -*- mode: cmake; -*-
# - CMakeLists.txt for mygsl/src subdir

set ( _mygsl_include_subdirs )

# - Automates the include detail subdir :
set ( _mygsl_has_detail_includes 0 )
if (EXISTS ${PROJECT_SOURCE_DIR}/include/mygsl/detail )
  set ( _mygsl_has_detail_includes 1 )
  list (APPEND _mygsl_include_subdirs detail )
endif ()

# - Explicitely add include subdirectories (one sublevel only) 
#   by convention : 'detail' and 'serialization' are reserved :
#
#list (APPEND _mygsl_include_subdirs test )
#list (APPEND _mygsl_include_subdirs utils )
#list (APPEND _mygsl_include_subdirs foo )
#list (APPEND _mygsl_include_subdirs bar )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

set ( _mygsl_subdirs_PUBLIC_HEADERS )
foreach ( _mygsl_include_subdir ${_mygsl_include_subdirs} )
  file ( GLOB mygsl_${_mygsl_include_subdir}_PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/mygsl/${_mygsl_include_subdir}/*.h
    )   
  install ( FILES ${mygsl_${_mygsl_include_subdir}_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl/${_mygsl_include_subdir}
    )
  set ( _mygsl_subdirs_PUBLIC_HEADERS 
    ${_mygsl_subdirs_PUBLIC_HEADERS} 
    mygsl_${_mygsl_include_subdir}_PUBLIC_HEADERS )
endforeach ()

file ( GLOB mygsl_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/mygsl/*.h
  ${PROJECT_BINARY_DIR}/include/mygsl/mygsl_config.h
  )

# if ( _mygsl_has_detail_includes )
#   file ( GLOB mygsl_detail_PUBLIC_HEADERS
#     ${PROJECT_SOURCE_DIR}/include/mygsl/detail/*.h
#     )
# endif ()

file ( GLOB mygsl_SOURCES
  ${mygsl_PUBLIC_HEADERS}
  #${mygsl_detail_PUBLIC_HEADERS}
  ${_sncore_subdirs_PUBLIC_HEADERS}
  ${PROJECT_SOURCE_DIR}/src/*.cc
  )

if ( MYGSL_WITH_BIO_SUPPORT )
  if ( EXISTS ${PROJECT_SOURCE_DIR}/src/the_serializable.cc )
    list (REMOVE_ITEM mygsl_SOURCES 
      ${PROJECT_SOURCE_DIR}/src/the_serializable.cc )		
  endif ()   	    
endif ()   	    

#----------------------------------------------------------------------------

include_directories ( ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include	
  )

#----------------------------------------------------------------------------
# Dependencies
#

if (AUTOMATIC_CMAKE_DEPENDENCIES)

  # List of definitions flags to be passed to the compiler :
  set (_mygsl_list_of_dependencies_definitions  )
  # List of include directories to be used at build time :
  set (_mygsl_list_of_dependencies_include_dirs )
  # List of library targets to be linked against :
  set (_mygsl_list_of_dependencies_libraries    )

  if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: " 
      "_mygsl_list_of_dependencies_infos = "
      "'${_mygsl_list_of_dependencies_infos}' " )
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: " 
      "_mygsl_list_of_third_party_dependencies_infos = "
      "'${_mygsl_list_of_third_party_dependencies_infos}' " )
  endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)

  # Compute all dependencies directives in one shot :
  if ( _mygsl_number_of_dependencies GREATER 0 )   
    mygsl_home_made_package_dependencies_populate_build_lists (
      "${_mygsl_list_of_dependencies_infos}"
      _mygsl_list_of_dependencies_definitions
      _mygsl_list_of_dependencies_include_dirs
      _mygsl_list_of_dependencies_libraries
      )
  endif ()

  if ( _mygsl_number_of_third_party_dependencies GREATER 0 )   
    mygsl_third_party_dependencies_populate_build_lists (
      "${_mygsl_list_of_third_party_dependencies_infos}"
      _mygsl_list_of_dependencies_definitions
      _mygsl_list_of_dependencies_include_dirs
      _mygsl_list_of_dependencies_libraries
      )
  endif ()

  if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: DEFINITIONS  = '${_mygsl_list_of_dependencies_definitions}' " )
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: INCLUDE_DIRS = '${_mygsl_list_of_dependencies_include_dirs}' " )
    message ( STATUS "${PROJECT_NAME}: src: DEVEL:  LIBRARIES    = '${_mygsl_list_of_dependencies_libraries}' " )
  endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
  add_definitions ( ${_mygsl_list_of_dependencies_definitions} )
  include_directories ( ${_mygsl_list_of_dependencies_include_dirs} )
endif (AUTOMATIC_CMAKE_DEPENDENCIES)

#----------------------------------------------------------------------------
# Add the library
#

add_library ( mygsl SHARED  
  ${mygsl_PUBLIC_HEADERS} 
  ${mygsl_SOURCES}
  )

set ( _mygsl_link_libraries "" )

if (AUTOMATIC_CMAKE_DEPENDENCIES)
  # Apply automatic dependencies build directives to the compiler :
  list ( APPEND _mygsl_link_libraries 
    ${_mygsl_list_of_dependencies_libraries} )
endif (AUTOMATIC_CMAKE_DEPENDENCIES)

# Link dynamic library :
target_link_libraries ( mygsl 
  ${_mygsl_link_libraries} 
  )

# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS mygsl
  EXPORT mygsl-libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
  )

install ( FILES ${mygsl_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl
  )

# if ( _mygsl_has_detail_includes )
#   install ( FILES ${mygsl_detail_PUBLIC_HEADERS}
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl/detail
#     )
# endif ()


###############################################################################
# - Bio stuff :
#
if ( MYGSL_WITH_BIO_SUPPORT AND MYGSL_WITH_BIO ) 

  file ( GLOB mygsl_serialization_PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/*.h
    )

  install ( FILES ${mygsl_serialization_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl/serialization
    )
  
  file ( GLOB mygsl_ipp_PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/*.ipp
    )

  install ( FILES ${mygsl_ipp_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mygsl/serialization
    )

  set ( mygsl_bio_SOURCES 
    ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/the_serializable.ipp
    ${PROJECT_SOURCE_DIR}/include/mygsl/serialization/the_serializable.h
    ${PROJECT_SOURCE_DIR}/src/the_serializable.cc 
    )

  add_library ( mygsl_bio SHARED 
    ${mygsl_PUBLIC_HEADERS} 
    ${mygsl_detail_PUBLIC_HEADERS} 
    ${mygsl_ipp_PUBLIC_HEADERS} 
    ${mygsl_bio_SOURCES}
    )

  set (_mygsl_bio_link_libraries )

  # Link dynamic library :
  target_link_libraries ( mygsl_bio 
    ${_mygsl_bio_link_libraries}
    mygsl
    )
  
  ### target_link_libraries ( mygsl_bio LINK_INTERFACE_LIBRARIES "" )

  # Install library and associate it with an export
  install ( TARGETS mygsl_bio
    EXPORT  mygsl-libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    )

endif ()

# - end of CMakeLists.txt for mygsl/src subdir
