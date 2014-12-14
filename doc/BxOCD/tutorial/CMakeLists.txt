cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(BxOCDTutorial)
set(BxOCDTutorial "1.0.0")
find_package(Bayeux REQUIRED NO_MODULE)
find_package(Doxygen 1.8 REQUIRED)

include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

add_library(BxOCDTutorial SHARED my/algo.h my/algo.cc)
target_link_libraries(BxOCDTutorial ${Bayeux_LIBRARIES})

add_executable(run_my_algo programs/run_my_algo.cxx)
target_link_libraries(run_my_algo BxOCDTutorial)

set(bxocdtutorial_DOXYFILE_IN ${PROJECT_SOURCE_DIR}/documentation/doxygen.conf.in)
set(bxocdtutorial_DOXYFILE_OUT ${PROJECT_BINARY_DIR}/documentation/doxygen.conf)
set(bxocdtutorial_DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/documentation/API)
set(bxocdtutorial_DOXYGEN_OUTPUT_FILE ${bxocdtutorial_DOXYGEN_OUTPUT_DIR}/html/index.html)
set(bxocdtutorial_DOXYGEN_OUTPUT_IMAGE_DIR ${bxocdtutorial_DOXYGEN_OUTPUT_DIR}/html/images)

configure_file(${bxocdtutorial_DOXYFILE_IN} ${bxocdtutorial_DOXYFILE_OUT} @ONLY)

add_custom_command(
  OUTPUT  ${bxocdtutorial_DOXYGEN_OUTPUT_FILE}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${bxocdtutorial_DOXYGEN_OUTPUT_DIR}
  COMMAND ${DOXYGEN_EXECUTABLE} ${bxocdtutorial_DOXYFILE_OUT}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  ### DEPENDS Bayeux ${bxocdtutorial_DOXYFILE_OUT} ${bxocdtutorial_MARKDOWN_DOCS}
  COMMENT "Doxygenating Bayeux OCD tutorial"
)

add_custom_target(bxocdtutorial_docs ALL
  DEPENDS ${bxocdtutorial_DOXYGEN_OUTPUT_FILE}
  )