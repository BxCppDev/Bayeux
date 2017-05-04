// test_version.cxx - unit tests for lahague/version.h
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

// Ourselves:
#include <lahague/version.h>

// Standard Library:
#include <iostream>

// Third Party:
// - Datatools:
#include <datatools/exception.h>

int main(int /* argc_ */, const char ** /* argv_ */ )
{
  try {
    //! Check that version symbols and method return values match
    DT_THROW_IF(LAHAGUE_VERSION_MAJOR != lahague::version::get_major(),
                std::runtime_error,
                "LAHAGUE_VERSION_MAJOR != lahague::version::get_major()");
    DT_THROW_IF(LAHAGUE_VERSION_MINOR != lahague::version::get_minor(),
                std::runtime_error,
                "LAHAGUE_VERSION_MINOR != lahague::version::get_minor()");
    DT_THROW_IF(LAHAGUE_VERSION_PATCH != lahague::version::get_patch(),
                std::runtime_error,
                "LAHAGUE_VERSION_PATCH != lahague::version::get_patch()");

    //! Check that compile and runtime is_at_least return true for
    //  the current version
    // - Compile
    DT_THROW_IF(! (LAHAGUE_IS_AT_LEAST(LAHAGUE_VERSION_MAJOR,
                                       LAHAGUE_VERSION_MINOR,
                                       LAHAGUE_VERSION_PATCH) ),
                std::runtime_error,
                "LAHAGUE_IS_AT_LEAST expands to false for current compiled version");
    // - Run
    DT_THROW_IF(!lahague::version::is_at_least(LAHAGUE_VERSION_MAJOR,
                                               LAHAGUE_VERSION_MINOR,
                                               LAHAGUE_VERSION_PATCH),
                std::runtime_error,
                "lahague::version::is_at_least returns false for current compiled version");

    //! is_at_least behaves correctly for versions ether side of the
    //  current version.
    // Higher versions must return false:
    // - Compile
    DT_THROW_IF(LAHAGUE_IS_AT_LEAST(LAHAGUE_VERSION_MAJOR+1,
                                    LAHAGUE_VERSION_MINOR,
                                    LAHAGUE_VERSION_PATCH),
                std::runtime_error,
                "LAHAGUE_IS_AT_LEAST expands to true when supplied major version is greater than current");

    DT_THROW_IF(LAHAGUE_IS_AT_LEAST(LAHAGUE_VERSION_MAJOR,
                                    LAHAGUE_VERSION_MINOR+1,
                                    LAHAGUE_VERSION_PATCH),
                std::runtime_error,
                "LAHAGUE_IS_AT_LEAST expands to true when supplied minor version is greater than current");

    DT_THROW_IF(LAHAGUE_IS_AT_LEAST(LAHAGUE_VERSION_MAJOR,
                                    LAHAGUE_VERSION_MINOR,
                                    LAHAGUE_VERSION_PATCH+1),
                std::runtime_error,
                "LAHAGUE_IS_AT_LEAST expands to true when supplied patch version is greater than current");

    // - Run
    DT_THROW_IF(lahague::version::is_at_least(LAHAGUE_VERSION_MAJOR+1,
                                              LAHAGUE_VERSION_MINOR,
                                              LAHAGUE_VERSION_PATCH),
                std::runtime_error,
                "lahague::version::is_at_least returns true when supplied major version is greater than current");

    DT_THROW_IF(lahague::version::is_at_least(LAHAGUE_VERSION_MAJOR,
                                              LAHAGUE_VERSION_MINOR+1,
                                              LAHAGUE_VERSION_PATCH),
                std::runtime_error,
                "lahague::version::is_at_least returns true when supplied minor version is greater than current");

    DT_THROW_IF(lahague::version::is_at_least(LAHAGUE_VERSION_MAJOR,
                                              LAHAGUE_VERSION_MINOR,
                                              LAHAGUE_VERSION_PATCH+1),
                std::runtime_error,
                "lahague::version::is_at_least returns true when supplied patch version is greater than current");

    //! Test that an invalid feature returns false
    DT_THROW_IF(lahague::version::has_feature("foobarbaz"),
                std::runtime_error,
                "lahague::version::has_feature returned true for bogus feature 'foobarbaz'");
  }
  catch (std::exception & e) {
    std::cerr << "lahague::test_version test emitted exception: \n"
              << e.what()
              << std::endl;
    return 1;
  }
  return 0;
}
