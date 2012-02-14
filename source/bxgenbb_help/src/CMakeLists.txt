# - CMakeLists.txt for genbb_help/src subdir

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# ----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB genbb_help_f77_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genbb_help/*.inc
  ${PROJECT_SOURCE_DIR}/include/genbb_help/fermi.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/rng.h
  ${PROJECT_BINARY_DIR}/include/genbb_help/__genbb_help_FC.h
  )
file ( GLOB genbb_help_f77_SOURCES
  ${genbb_help_f77_PUBLIC_HEADERS}
  ${PROJECT_SOURCE_DIR}/src/fermi.cc
  ${PROJECT_SOURCE_DIR}/src/rng.cc
  ${PROJECT_SOURCE_DIR}/src/*.f
  )

file ( GLOB genbb_help_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genbb_help/*.h
  ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h
  ${PROJECT_BINARY_DIR}/include/genbb_help/__genbb_help_FC.h
  )
list ( REMOVE_ITEM genbb_help_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/genbb_help/fermi.h" )
list ( REMOVE_ITEM genbb_help_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/genbb_help/rng.h"   )

file ( GLOB genbb_help_ipp_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genbb_help/*.ipp
  )

file ( GLOB genbb_help_serialization_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/genbb_help/serialization/*.h
  ${PROJECT_SOURCE_DIR}/include/genbb_help/serialization/*.ipp
  )

file ( GLOB genbb_help_SOURCES
  ${PROJECT_SOURCE_DIR}/include/genbb_help/*.h
  ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h
  ${PROJECT_SOURCE_DIR}/src/*.cc
  )

#### FORCE BUG
#list ( REMOVE_ITEM genbb_help_SOURCES "${PROJECT_SOURCE_DIR}/src/fermi.cc" )
#list ( REMOVE_ITEM genbb_help_SOURCES "${PROJECT_SOURCE_DIR}/src/rng.cc" )

# remove this because it go in the BIO DLL :
list ( REMOVE_ITEM genbb_help_SOURCES "${PROJECT_SOURCE_DIR}/src/the_serializable.cc" )

message ( STATUS "genbb_help_SOURCES=${genbb_help_SOURCES}" )

if ( GENBB_HELP_WITH_BIO )
  set ( genbb_help_bio_SOURCES 
    ${PROJECT_SOURCE_DIR}/include/genbb_help/serialization/the_serializable.ipp
    ${PROJECT_SOURCE_DIR}/include/genbb_help/serialization/the_serializable.h
    ${PROJECT_SOURCE_DIR}/src/the_serializable.cc )
  message ( STATUS "genbb_help_bio_SOURCES=${genbb_help_bio_SOURCES}" )
endif ( GENBB_HELP_WITH_BIO )

include_directories ( ${PROJECT_SOURCE_DIR}/include	
  ${PROJECT_BINARY_DIR}/include	
  )

# ---------------------------------------------------------------------------- 
# Dependencies
#
include_directories ( ${geomtools_INCLUDE_DIRS} )

if ( GENBB_HELP_WITH_DEBUG )
  add_definitions ( "-g" )
endif ()

# ----------------------------------------------------------------------------
# Add the libraries
#

### add_definitions (CONFIG=Debug;Release;RelWithDebInfo;MinSizeRel
### -DGEN_OUTFILES)
### 

# set ( CMAKE_Fortran_FLAGS "${_genbb_help_fortran_definitions}"  )


# set ( CMAKE_CXX_FLAGS "${_genbb_help_fortran_definitions} ${_genbb_help_definitions} "  )

# -Dgenbbsub=genbbsub_ 
# -Dset_genbb_random_seed=set_genbb_random_seed_ 
# -Dgrndm=grndm_ 
# -Dgenbb_init_enrange=genbb_init_enrange_ 
# -Dgenevent=genevent_ 
# -Denrange=enrange_

# ----------------------------------------------------------------------------
# Fortran :
#

add_library ( genbb_help_f77 SHARED  
  ${genbb_help_f77_PUBLIC_HEADERS} 
  ${genbb_help_f77_SOURCES}
  )

# ----------------------------------------------------------------------------
# Link dynamic library :
#
set ( _link_libraries ${geomtools_LIBRARIES} )

target_link_libraries ( genbb_help_f77 
  ${_link_libraries}
  )

# ----------------------------------------------------------------------------
# C++ :
#

add_library ( genbb_help SHARED  
  ${genbb_help_PUBLIC_HEADERS} 
  ${genbb_help_SOURCES}
  )

# Link dynamic library :
target_link_libraries ( genbb_help
  genbb_help_f77
  ${_link_libraries}
  )
# ----------------------------------------------------------------------
# Installation :
#

install ( FILES ${PROJECT_SOURCE_DIR}/scripts/genbb
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE 
  GROUP_READ GROUP_EXECUTE 
  WORLD_READ WORLD_EXECUTE
  )

install ( TARGETS genbb_help_f77
  EXPORT  genbb_help-libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
  )

install ( TARGETS genbb_help
  EXPORT  genbb_help-libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
  )

install ( FILES ${genbb_help_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help
  )

install ( FILES ${genbb_help_ipp_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help
  )

install ( FILES ${genbb_help_serialization_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/genbb_help/serialization
  )

# - Bio stuff :
if ( GENBB_HELP_WITH_BIO )

  add_library( genbb_help_bio SHARED 
    ${genbb_help_PUBLIC_HEADERS} 
    ${genbb_help_detail_PUBLIC_HEADERS} 
    ${genbb_help_bio_SOURCES}
    )

  # Link dynamic library :
  target_link_libraries ( genbb_help_bio
    genbb_help
    genbb_help_f77
    ${_link_libraries}
    )

  ###target_link_libraries ( genbb_help_bio LINK_INTERFACE_LIBRARIES "" )

  install ( TARGETS genbb_help_bio
    EXPORT  genbb_help-libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    )

endif ( GENBB_HELP_WITH_BIO )

# - end of CMakeLists.txt for genbb_help/src subdir
