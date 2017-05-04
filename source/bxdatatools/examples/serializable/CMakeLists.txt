# - Top level CMakeLists.txt for datatools ex_serializable example program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(datatools_ex_serializable)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

# - Bayeux :
find_package(Bayeux 2.1.0 REQUIRED)
set(_incs ${Bayeux_INCLUDE_DIRS})
set(_libs ${Bayeux_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR} ${_incs})

add_library(datatools_ex_serializable SHARED
            raw_hit.h
	    raw_hit.cc
            raw_data.h
	    raw_data.cc
	    )

target_link_libraries(datatools_ex_serializable ${_libs} )

install( FILES
         ${PROJECT_BINARY_DIR}/libdatatools_ex_serializable${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )


add_library(datatools_ex_serializable_bio SHARED raw_hit.h
                                                 raw_hit.ipp
						 raw_data.h
                                                 raw_data.ipp
						 the_serializable.h
						 the_serializable.cc)

target_link_libraries(datatools_ex_serializable_bio datatools_ex_serializable )

install( FILES
         ${PROJECT_BINARY_DIR}/libdatatools_ex_serializable_bio${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )

add_executable(ex_serializable_1 ex_serializable_1.cxx)

target_link_libraries(ex_serializable_1
                      datatools_ex_serializable_bio
		      ${datatools_BIO_LIBRARY}
                     )

install ( FILES
    ${PROJECT_BINARY_DIR}/ex_serializable_1
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt