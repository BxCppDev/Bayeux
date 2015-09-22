// test_version.cxx - unit tests for datatools/version.h
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick

// Ourselves
#include "datatools/version.h"

// Standard Library
#include <iostream>

// This Project
#include "datatools/exception.h"

int main(/*int argc, const char *argv[]*/)
{
  try {
    //! Check that version symbols and method return values match
    DT_THROW_IF(DATATOOLS_VERSION_MAJOR != datatools::version::get_major(),
                std::runtime_error,
                "DATATOOLS_VERSION_MAJOR != datatools::version::get_major()");
    DT_THROW_IF(DATATOOLS_VERSION_MINOR != datatools::version::get_minor(),
                std::runtime_error,
                "DATATOOLS_VERSION_MINOR != datatools::version::get_minor()");
    DT_THROW_IF(DATATOOLS_VERSION_PATCH != datatools::version::get_patch(),
                std::runtime_error,
                "DATATOOLS_VERSION_PATCH != datatools::version::get_patch()");

    //! Check that compile and runtime is_at_least return true for
    //  the current version
    // - Compile
    DT_THROW_IF(
        ! (DATATOOLS_IS_AT_LEAST(DATATOOLS_VERSION_MAJOR,
                                 DATATOOLS_VERSION_MINOR,
                                 DATATOOLS_VERSION_PATCH) ),
        std::runtime_error,
        "DATATOOLS_IS_AT_LEAST expands to false for current compiled version");
    // - Run
    DT_THROW_IF(!datatools::version::is_at_least(DATATOOLS_VERSION_MAJOR,
                                                 DATATOOLS_VERSION_MINOR,
                                                 DATATOOLS_VERSION_PATCH),
                std::runtime_error,
                "datatools::version::is_at_least returns false for current compiled version");

    //! is_at_least behaves correctly for versions ether side of the
    //  current version.
    // Higher versions must return false:
    // - Compile
    DT_THROW_IF(DATATOOLS_IS_AT_LEAST(DATATOOLS_VERSION_MAJOR+1,
                                      DATATOOLS_VERSION_MINOR,
                                      DATATOOLS_VERSION_PATCH),
        std::runtime_error,
        "DATATOOLS_IS_AT_LEAST expands to true when supplied major version is greater than current");

    DT_THROW_IF(DATATOOLS_IS_AT_LEAST(DATATOOLS_VERSION_MAJOR,
                                      DATATOOLS_VERSION_MINOR+1,
                                      DATATOOLS_VERSION_PATCH),
        std::runtime_error,
        "DATATOOLS_IS_AT_LEAST expands to true when supplied minor version is greater than current");

    DT_THROW_IF(DATATOOLS_IS_AT_LEAST(DATATOOLS_VERSION_MAJOR,
                                      DATATOOLS_VERSION_MINOR,
                                      DATATOOLS_VERSION_PATCH+1),
        std::runtime_error,
        "DATATOOLS_IS_AT_LEAST expands to true when supplied patch version is greater than current");

    // - Run
    DT_THROW_IF(datatools::version::is_at_least(DATATOOLS_VERSION_MAJOR+1,
                                                DATATOOLS_VERSION_MINOR,
                                                DATATOOLS_VERSION_PATCH),
                std::runtime_error,
                "datatools::version::is_at_least returns true when supplied major version is greater than current");

    DT_THROW_IF(datatools::version::is_at_least(DATATOOLS_VERSION_MAJOR,
                                                DATATOOLS_VERSION_MINOR+1,
                                                DATATOOLS_VERSION_PATCH),
                std::runtime_error,
                "datatools::version::is_at_least returns true when supplied minor version is greater than current");

    DT_THROW_IF(datatools::version::is_at_least(DATATOOLS_VERSION_MAJOR,
                                                DATATOOLS_VERSION_MINOR,
                                                DATATOOLS_VERSION_PATCH+1),
                std::runtime_error,
                "datatools::version::is_at_least returns true when supplied patch version is greater than current");

    //! Test that an invalid feature returns false
    DT_THROW_IF(datatools::version::has_feature("foobarbaz"),
                std::runtime_error,
                "datatools::version::has_feature returned true for bogus feature 'foobarbaz'");
  }
  catch (std::exception& e) {
    std::cerr << "datatools::test_version test emitted exception: \n"
              << e.what()
              << std::endl;
    return 1;
  }
  return 0;
}
