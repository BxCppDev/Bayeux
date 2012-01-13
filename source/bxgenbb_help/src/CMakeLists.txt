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
     )
file ( GLOB genbb_help_f77_SOURCES
       ${PROJECT_SOURCE_DIR}/include/genbb_help/*.inc
       ${PROJECT_SOURCE_DIR}/include/genbb_help/fermi.h
       ${PROJECT_SOURCE_DIR}/include/genbb_help/rng.h
       ${PROJECT_SOURCE_DIR}/src/fermi.cc
       ${PROJECT_SOURCE_DIR}/src/rng.cc
       ${PROJECT_SOURCE_DIR}/src/*.f
     )
file ( GLOB genbb_help_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/genbb_help/*.h
       ${PROJECT_SOURCE_DIR}/include/genbb_help/genbb_help.h
       ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h
     )
list ( REMOVE_ITEM genbb_help_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/genbb_help/fermi.h" )
list ( REMOVE_ITEM genbb_help_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/genbb_help/rng.h"   )

file ( GLOB genbb_help_ipp_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/genbb_help/*.ipp
     )

file ( GLOB genbb_help_serialization_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/genbb_help/serialization/*.h
     )

file ( GLOB genbb_help_SOURCES
       ${PROJECT_SOURCE_DIR}/include/genbb_help/*.h
       ${PROJECT_BINARY_DIR}/include/genbb_help/genbb_help_config.h
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )
list ( REMOVE_ITEM genbb_help_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/src/fermi.cc" )
list ( REMOVE_ITEM genbb_help_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/src/rng.cc" )

# remove this because it go in the BIO DLL :
list ( REMOVE_ITEM genbb_help_SOURCES "${PROJECT_SOURCE_DIR}/src/the_serializable.cc" )

message ( STATUS "genbb_help_SOURCES=${genbb_help_SOURCES}" )

if ( GENBB_HELP_WITH_BIO )
  set ( genbb_help_bio_SOURCES 
        ${PROJECT_SOURCE_DIR}/include/genbb_help/the_serializable.ipp
        ${PROJECT_SOURCE_DIR}/include/genbb_help/the_serializable.h
        ${PROJECT_SOURCE_DIR}/src/the_serializable.cc )
  message ( STATUS "genbb_help_bio_SOURCES=${genbb_help_bio_SOURCES}" )
endif ( GENBB_HELP_WITH_BIO )

include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                     )

# ---------------------------------------------------------------------------- 
# Dependencies
#
include_directories ( ${GEOMTOOLS_INCLUDE_DIRS_DEPEND} )

# ----------------------------------------------------------------------------
# Add the libraries
#

### add_definitions (CONFIG=Debug;Release;RelWithDebInfo;MinSizeRel
### -DGEN_OUTFILES)
### 

set ( CMAKE_Fortran_FLAGS 
    "-Dfermi_wrap=fermi_wrap_ -Drng_shoot_reset=rng_shoot_reset_ -Drng_shoot_flat=rng_shoot_flat_ "	 
    ) 

set ( CMAKE_CXX_FLAGS 
    "-Dfermi_wrap=fermi_wrap_ -Drng_shoot_reset=rng_shoot_reset_ -Drng_shoot_flat=rng_shoot_flat_ -Dgenbbsub=genbbsub_ -Dset_genbb_random_seed=set_genbb_random_seed_ -Dgrndm=grndm_ -Dgenbb_init_enrange=genbb_init_enrange_ -Dgenevent=genevent_ -Denrange=enrange_"	 
    ) 

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

target_link_libraries ( genbb_help_f77 ${MYGSL_LIBRARIES_DEPEND})

# ----------------------------------------------------------------------------
# C++ :
#

add_library ( genbb_help SHARED  
              ${genbb_help_PUBLIC_HEADERS} 
	      ${genbb_help_SOURCES}
            )

# Link dynamic library :
target_link_libraries ( genbb_help
                        ${GEOMTOOLS_LIBRARIES} 
			genbb_help_f77
                        ${MYGSL_LIBRARIES_DEPEND}
                      )
# ----------------------------------------------------------------------
# Installation :
#

install ( FILES ${PROJECT_SOURCE_DIR}/scripts/genbb
	  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
          PERMISSIONS OWNER_READ OWNER_EXECUTE 
                      GROUP_READ GROUP_EXECUTE 
                      WORLD_READ WORLD_EXECUTE
       )

install ( TARGETS genbb_help_f77
          RUNTIME DESTINATION bin COMPONENT bin
          LIBRARY DESTINATION lib COMPONENT bin 
          ARCHIVE DESTINATION lib COMPONENT devel
        )

install ( TARGETS genbb_help
          RUNTIME DESTINATION bin COMPONENT bin
          LIBRARY DESTINATION lib COMPONENT bin 
          ARCHIVE DESTINATION lib COMPONENT devel
        )

install ( FILES ${genbb_help_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/genbb_help
        )

install ( FILES ${genbb_help_ipp_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/genbb_help
        )

install ( FILES ${genbb_help_serialization_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/genbb_help/serialization
        )

if ( GENBB_HELP_WITH_BIO )

   add_library( genbb_help_bio SHARED 
                ${genbb_help_PUBLIC_HEADERS} 
                ${genbb_help_detail_PUBLIC_HEADERS} 
                ${genbb_help_bio_SOURCES}
              )

   # Link dynamic library :
   target_link_libraries ( genbb_help_bio
                           genbb_help
                           ${GEOMTOOLS_LIBRARIES} 
			   genbb_help_f77
                           ${MYGSL_LIBRARIES_DEPEND}
                         )

   # target_link_libraries ( genbb_help_bio LINK_INTERFACE_LIBRARIES "" )

   install ( TARGETS genbb_help_bio
             RUNTIME DESTINATION bin COMPONENT bin
             LIBRARY DESTINATION lib COMPONENT bin 
             ARCHIVE DESTINATION lib COMPONENT devel
           )

endif ( GENBB_HELP_WITH_BIO )

# - end of CMakeLists.txt for genbb_help/src subdir
