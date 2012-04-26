# -*- mode: cmake; -*-
# - CMakeLists.txt for emfield/src subdir

set ( _emfield_has_detail_includes 0 )
if (EXISTS ${PROJECT_SOURCE_DIR}/include/emfield/detail )
  set ( _emfield_has_detail_includes 1 )
endif ()

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB emfield_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/emfield/*.h
  ${PROJECT_BINARY_DIR}/include/emfield/emfield_config.h
  )

if ( _emfield_has_detail_includes )
  file ( GLOB emfield_detail_PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/emfield/detail/*.h
    )
endif ()

file ( GLOB emfield_SOURCES
  ${emfield_PUBLIC_HEADERS}
  ${emfield_detail_PUBLIC_HEADERS}
  ${PROJECT_SOURCE_DIR}/src/*.cc
  )

if ( EMFIELD_WITH_BIO_SUPPORT )
  if ( EXISTS ${PROJECT_SOURCE_DIR}/src/the_serializable.cc )
    list (REMOVE_ITEM emfield_SOURCES 
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
  set (_emfield_list_of_dependencies_definitions  )
  # List of include directories to be used at build time :
  set (_emfield_list_of_dependencies_include_dirs )
  # List of library targets to be linked against :
  set (_emfield_list_of_dependencies_libraries    )

  if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: " 
      "_emfield_list_of_dependencies_infos = "
      "'${_emfield_list_of_dependencies_infos}' " )
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: " 
      "_emfield_list_of_third_party_dependencies_infos = "
      "'${_emfield_list_of_third_party_dependencies_infos}' " )
  endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)

  # Compute all dependencies directives in one shot :
  if ( _emfield_number_of_dependencies GREATER 0 )   
    emfield_home_made_package_dependencies_populate_build_lists (
      "${_emfield_list_of_dependencies_infos}"
      _emfield_list_of_dependencies_definitions
      _emfield_list_of_dependencies_include_dirs
      _emfield_list_of_dependencies_libraries
      )
  endif ()

  if ( _emfield_number_of_third_party_dependencies GREATER 0 )   
    emfield_third_party_dependencies_populate_build_lists (
      "${_emfield_list_of_third_party_dependencies_infos}"
      _emfield_list_of_dependencies_definitions
      _emfield_list_of_dependencies_include_dirs
      _emfield_list_of_dependencies_libraries
      )
  endif ()

  if (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: DEFINITIONS  = '${_emfield_list_of_dependencies_definitions}' " )
    message ( STATUS "${PROJECT_NAME}: src: DEVEL: INCLUDE_DIRS = '${_emfield_list_of_dependencies_include_dirs}' " )
    message ( STATUS "${PROJECT_NAME}: src: DEVEL:  LIBRARIES    = '${_emfield_list_of_dependencies_libraries}' " )
  endif (AUTOMATIC_CMAKE_DEPENDENCIES_DEVEL)
  add_definitions ( ${_emfield_list_of_dependencies_definitions} )
  include_directories ( ${_emfield_list_of_dependencies_include_dirs} )
endif (AUTOMATIC_CMAKE_DEPENDENCIES)

#----------------------------------------------------------------------------
# Add the library
#

add_library ( emfield SHARED  
  ${emfield_PUBLIC_HEADERS} 
  ${emfield_SOURCES}
  )

set ( _emfield_link_libraries "" )

if (AUTOMATIC_CMAKE_DEPENDENCIES)
  # Apply automatic dependencies build directives to the compiler :
  list ( APPEND _emfield_link_libraries 
    ${_emfield_list_of_dependencies_libraries} )
endif (AUTOMATIC_CMAKE_DEPENDENCIES)

# Link dynamic library :
target_link_libraries ( emfield 
  ${_emfield_link_libraries} 
  )

# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS emfield
  EXPORT emfield-libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
  )

install ( FILES ${emfield_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/emfield
  )

if ( _emfield_has_detail_includes )
  install ( FILES ${emfield_detail_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/emfield/detail
    )
endif ()


###############################################################################
# - Bio stuff :
#
if ( EMFIELD_WITH_BIO_SUPPORT AND EMFIELD_WITH_BIO ) 

  file ( GLOB emfield_serialization_PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/emfield/serialization/*.h
    )

  install ( FILES ${emfield_serialization_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/emfield/serialization
    )
  
  file ( GLOB emfield_ipp_PUBLIC_HEADERS
    ${PROJECT_SOURCE_DIR}/include/emfield/serialization/*.ipp
    )

  install ( FILES ${emfield_ipp_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/emfield/serialization
    )

  set ( emfield_bio_SOURCES 
    ${PROJECT_SOURCE_DIR}/include/emfield/serialization/the_serializable.ipp
    ${PROJECT_SOURCE_DIR}/include/emfield/serialization/the_serializable.h
    ${PROJECT_SOURCE_DIR}/src/the_serializable.cc 
    )

  add_library ( emfield_bio SHARED 
    ${emfield_PUBLIC_HEADERS} 
    ${emfield_detail_PUBLIC_HEADERS} 
    ${emfield_ipp_PUBLIC_HEADERS} 
    ${emfield_bio_SOURCES}
    )

  set (_emfield_bio_link_libraries )

  # Link dynamic library :
  target_link_libraries ( emfield_bio 
    ${_emfield_bio_link_libraries}
    emfield
    )
  
  ### target_link_libraries ( emfield_bio LINK_INTERFACE_LIBRARIES "" )

  # Install library and associate it with an export
  install ( TARGETS emfield_bio
    EXPORT  emfield-libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    )

endif ()

# - end of CMakeLists.txt for emfield/src subdir
