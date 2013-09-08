// test_version.cxx - unit tests for materials/version.h
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick

// Ourselves
#include "materials/version.h"

// Standard Library
#include <iostream>

// Third Party
// - Datatools
#include "datatools/exception.h"

int main(int argc, const char *argv[])
{
  try {
    //! Check that version symbols and method return values match
    DT_THROW_IF(MATERIALS_VERSION_MAJOR != materials::version::get_major(),
                std::runtime_error,
                "MATERIALS_VERSION_MAJOR != materials::version::get_major()");
    DT_THROW_IF(MATERIALS_VERSION_MINOR != materials::version::get_minor(),
                std::runtime_error,
                "MATERIALS_VERSION_MINOR != materials::version::get_minor()");
    DT_THROW_IF(MATERIALS_VERSION_PATCH != materials::version::get_patch(),
                std::runtime_error,
                "MATERIALS_VERSION_PATCH != materials::version::get_patch()");

    //! Check that compile and runtime is_at_least return true for
    //  the current version
    // - Compile
    DT_THROW_IF(
        ! (MATERIALS_IS_AT_LEAST(MATERIALS_VERSION_MAJOR,
                                 MATERIALS_VERSION_MINOR,
                                 MATERIALS_VERSION_PATCH) ),
        std::runtime_error,
        "MATERIALS_IS_AT_LEAST expands to false for current compiled version");
    // - Run
    DT_THROW_IF(!materials::version::is_at_least(MATERIALS_VERSION_MAJOR,
                                                 MATERIALS_VERSION_MINOR,
                                                 MATERIALS_VERSION_PATCH),
                std::runtime_error,
                "materials::version::is_at_least returns false for current compiled version");

    //! is_at_least behaves correctly for versions ether side of the
    //  current version.
    // Higher versions must return false:
    // - Compile
    DT_THROW_IF(MATERIALS_IS_AT_LEAST(MATERIALS_VERSION_MAJOR+1,
                                      MATERIALS_VERSION_MINOR,
                                      MATERIALS_VERSION_PATCH),
        std::runtime_error,
        "MATERIALS_IS_AT_LEAST expands to true when supplied major version is greater than current");

    DT_THROW_IF(MATERIALS_IS_AT_LEAST(MATERIALS_VERSION_MAJOR,
                                      MATERIALS_VERSION_MINOR+1,
                                      MATERIALS_VERSION_PATCH),
        std::runtime_error,
        "MATERIALS_IS_AT_LEAST expands to true when supplied minor version is greater than current");

    DT_THROW_IF(MATERIALS_IS_AT_LEAST(MATERIALS_VERSION_MAJOR,
                                      MATERIALS_VERSION_MINOR,
                                      MATERIALS_VERSION_PATCH+1),
        std::runtime_error,
        "MATERIALS_IS_AT_LEAST expands to true when supplied patch version is greater than current");

    // - Run
    DT_THROW_IF(materials::version::is_at_least(MATERIALS_VERSION_MAJOR+1,
                                                MATERIALS_VERSION_MINOR,
                                                MATERIALS_VERSION_PATCH),
                std::runtime_error,
                "materials::version::is_at_least returns true when supplied major version is greater than current");

    DT_THROW_IF(materials::version::is_at_least(MATERIALS_VERSION_MAJOR,
                                                MATERIALS_VERSION_MINOR+1,
                                                MATERIALS_VERSION_PATCH),
                std::runtime_error,
                "materials::version::is_at_least returns true when supplied minor version is greater than current");

    DT_THROW_IF(materials::version::is_at_least(MATERIALS_VERSION_MAJOR,
                                                MATERIALS_VERSION_MINOR,
                                                MATERIALS_VERSION_PATCH+1),
                std::runtime_error,
                "materials::version::is_at_least returns true when supplied patch version is greater than current");

    //! Test that an invalid feature returns false
    DT_THROW_IF(materials::version::has_feature("foobarbaz"),
                std::runtime_error,
                "materials::version::has_feature returned true for bogus feature 'foobarbaz'");
  }
  catch (std::exception& e) {
    std::cerr << "materials::test_version test emitted exception: \n"
              << e.what()
              << std::endl;
    return 1;
  }
  return 0;
}
