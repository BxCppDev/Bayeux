# - CMakeLists.txt for datatools/src subdir

#----------------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources and
# public headers
#

file(GLOB boost_math_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/boost/math/*.hpp
)

file(GLOB boost_archive_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/boost/archive/*.hpp
)
 
file(GLOB datatools_utils_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/utils/*.h
)
 
file(GLOB datatools_serialization_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/serialization/*.h
)
 
file(GLOB datatools_services_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/services/*.h
)
 
file(GLOB datatools_event_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/event/*.h
)
 
file(GLOB datatools_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/*.h
  ${PROJECT_SOURCE_DIR}/include/datatools/datatools.h
  ${PROJECT_BINARY_DIR}/include/datatools/datatools_config.h
 )

file (GLOB datatools_SOURCES
  ${PROJECT_SOURCE_DIR}/include/datatools/event/*.h
  ${PROJECT_SOURCE_DIR}/include/datatools/utils/*.h
  ${PROJECT_SOURCE_DIR}/include/datatools/services/*.h
  ${PROJECT_SOURCE_DIR}/include/datatools/serialization/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cc
  )

# remove devel files :
list( REMOVE_ITEM datatools_SOURCES "${PROJECT_SOURCE_DIR}/include/datatools/dummy_datatools.h" )
list( REMOVE_ITEM datatools_SOURCES "${PROJECT_SOURCE_DIR}/src/dummy_datatools.cc" )
list( REMOVE_ITEM datatools_SOURCES "${PROJECT_SOURCE_DIR}/include/datatools/service/dummy_service.h" )
list( REMOVE_ITEM datatools_SOURCES "${PROJECT_SOURCE_DIR}/src/dummy_service.cc" )

# remove this because it gors in the BIO DLL :
list( REMOVE_ITEM datatools_SOURCES "${PROJECT_SOURCE_DIR}/src/the_serializable.cc" )

# Last source file built in the DLL :
list( REMOVE_ITEM datatools_SOURCES "${PROJECT_SOURCE_DIR}/src/base_service.cc" )
list (APPEND datatools_SOURCES "${PROJECT_SOURCE_DIR}/src/base_service.cc" )

#message ( AUTHOR_WARNING "datatools_SOURCES=${datatools_SOURCES}" )

if (DATATOOLS_WITH_BIO)
  set (datatools_bio_SOURCES 
       ${PROJECT_SOURCE_DIR}/include/datatools/the_serializable.ipp
       ${PROJECT_SOURCE_DIR}/include/datatools/the_serializable.h
       ${PROJECT_SOURCE_DIR}/src/the_serializable.cc)
  #message ( AUTHOR_WARNING "datatools_bio_SOURCES=${datatools_bio_SOURCES}" )
endif (DATATOOLS_WITH_BIO)

#----------------------------------------------------------------------------
# Add the library - explicitly shared for now since we want to generate 
# Frameworks.
include_directories ( ${CLHEP_INCLUDE_DIRS}
		      ${Boost_INCLUDE_DIRS}
		      ${PROJECT_SOURCE_DIR}/include	
                    )

add_library (datatools SHARED  
		      ${datatools_PUBLIC_HEADERS} 
		      ${datatools_event_PUBLIC_HEADERS} 
		      ${datatools_utils_PUBLIC_HEADERS} 
		      ${datatools_serialization_PUBLIC_HEADERS}
		      ${boost_math_PUBLIC_HEADERS}
		      ${boost_archive_PUBLIC_HEADERS}
		      ${datatools_SOURCES}
            )

# Link dynamic library, and empty the interface lib list

target_link_libraries(datatools
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_IOSTREAMS_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY})

install(TARGETS datatools
    RUNTIME DESTINATION bin COMPONENT bin
    LIBRARY DESTINATION lib COMPONENT bin 
    ARCHIVE DESTINATION lib COMPONENT devel
)

install (FILES ${boost_math_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/boost/math
        )

install (FILES ${boost_archive_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/boost/archive
        )

install (FILES ${datatools_utils_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/utils
        )

install (FILES ${datatools_event_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/event
        )

install (FILES ${datatools_serialization_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/serialization
        )

install (FILES ${datatools_services_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/services
        )

install (FILES ${datatools_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools
        )
 
file(GLOB datatools_ipp_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/*.ipp
)
 
file(GLOB datatools_utils_ipp_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/utils/*.ipp
)
 
file(GLOB datatools_serialization_ipp_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/serialization/*.ipp
)
 
file(GLOB datatools_event_ipp_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/datatools/event/*.ipp
)

install (FILES ${datatools_ipp_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools
        )

install (FILES ${datatools_event_ipp_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/event
        )

install (FILES ${datatools_serialization_ipp_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/serialization
        )

install (FILES ${datatools_utils_ipp_PUBLIC_HEADERS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/datatools/utils
        )

#target_link_libraries(datatools LINK_INTERFACE_LIBRARIES "")

if (DATATOOLS_WITH_BIO)

   add_library(datatools_bio SHARED 
   ${datatools_PUBLIC_HEADERS} 
   ${datatools_event_PUBLIC_HEADERS} 
   ${datatools_utils_PUBLIC_HEADERS} 
   ${datatools_serialization_PUBLIC_HEADERS} 
   ${datatools_bio_SOURCES})

   # Link dynamic library, and empty the interface lib list
   target_link_libraries(datatools_bio
      ${Boost_SERIALIZATION_LIBRARY}
      ${Boost_IOSTREAMS_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY}
      ${Boost_SYSTEM_LIBRARY})

   target_link_libraries(datatools_bio LINK_INTERFACE_LIBRARIES "")

   install(TARGETS datatools_bio
      RUNTIME DESTINATION bin COMPONENT bin
      LIBRARY DESTINATION lib COMPONENT bin 
      ARCHIVE DESTINATION lib COMPONENT devel
)

endif (DATATOOLS_WITH_BIO)

# - end of CMakeLists.txt for datatools/src subdir
