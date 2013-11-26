
set(electronics_TEST_DIR ${electronics_BASE_DIR}/testing)
message( STATUS "electronics_TEST_DIR='${electronics_TEST_DIR}'")

set(electronics_TESTS
  ${electronics_TEST_DIR}/test_component_base.cxx
  ${electronics_TEST_DIR}/test_bitset_desc.cxx
  ${electronics_TEST_DIR}/test_register_desc.cxx
  ${electronics_TEST_DIR}/test_board_base.cxx
  ${electronics_TEST_DIR}/test_crate_base.cxx
  ${electronics_TEST_DIR}/test_rack_base.cxx
  ${electronics_TEST_DIR}/test_component_manager.cxx
  #${electronics_TEST_DIR}/test_digitized_signal.cxx
  )

include_directories(${electronics_TEST_DIR}
  ${Boost_INCLUDE_DIRS}
  ${Bayeux_INCLUDE_DIRS}
  )

set(_electronics_TEST_ENVIRONMENT ELECTRONICS_TESTING_DIR=${electronics_TEST_DIR})

message( STATUS "TEST=${_electronics_TEST_ENVIRONMENT}")

foreach(_testsource ${electronics_TESTS})
  get_filename_component(_testname "${_testsource}" NAME_WE)
  string(REPLACE "${electronics_TEST_DIR}/" "" _testname "${_testname}")

  add_executable(${_testname} ${_testsource})

  target_link_libraries(${_testname} electronics)

  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()

  # Output to testing subdirectory
  set_property(TARGET ${_testname}
    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
    )

  add_test(NAME ${_testname} COMMAND ${_testname})
  set_property(TEST ${_testname}
    APPEND PROPERTY ENVIRONMENT ${_electronics_TEST_ENVIRONMENT}
    )
endforeach()