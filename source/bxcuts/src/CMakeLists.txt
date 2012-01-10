# - CMakeLists.txt for cuts/src subdir

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file ( GLOB cuts_PUBLIC_HEADERS
       ${PROJECT_SOURCE_DIR}/include/cuts/*.h
       ${PROJECT_SOURCE_DIR}/include/cuts/cuts.h
       ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h
     )

file ( GLOB cuts_SOURCES
       ${PROJECT_SOURCE_DIR}/include/cuts/*.h
       ${PROJECT_BINARY_DIR}/include/cuts/cuts_config.h
       ${PROJECT_SOURCE_DIR}/src/*.cc
     )

# Last source files built in the DLL for proper static stuff initialization:
list ( REMOVE_ITEM cuts_SOURCES "${PROJECT_SOURCE_DIR}/src/cut_tools.cc" )
list ( APPEND cuts_SOURCES "${PROJECT_SOURCE_DIR}/src/cut_tools.cc" )


include_directories ( ${PROJECT_SOURCE_DIR}/include	
                      ${PROJECT_BINARY_DIR}/include	
                     )

#----------------------------------------------------------------------------
# Dependencies
#
include_directories ( ${DATATOOLS_INCLUDE_DIRS_DEPEND} )

#----------------------------------------------------------------------------
# Add the library
#

add_library ( cuts SHARED  
              ${cuts_PUBLIC_HEADERS} 
	      ${cuts_SOURCES}
            )

# Link dynamic library :
target_link_libraries ( cuts
                        ${DATATOOLS_LIBRARIES_DEPEND}
                      )
# ----------------------------------------------------------------------
# Installation
#

install ( TARGETS cuts
          RUNTIME DESTINATION bin COMPONENT bin
          LIBRARY DESTINATION lib COMPONENT bin 
          ARCHIVE DESTINATION lib COMPONENT devel
        )

install ( FILES ${cuts_PUBLIC_HEADERS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/cuts
        )

# - end of CMakeLists.txt for cuts/src subdir
