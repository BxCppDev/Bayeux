//! \file  test_resource.cxx
//! \brief Unit tests for the lahague::resource API
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

// Standard Library
#include <iostream>
#include <fstream>
#include <cstdlib>

// Third Party
// - datatools
#include <datatools/exception.h>

// This Project
#include <lahague/resource.h>

//! Return false if get_resource passed an invalid resource
bool test_invalid_resource()
{
  std::string rspath;

  // Absolute resource:
  try {
    rspath = lahague::get_resource("/foo");
    return false;
  }
  catch (std::logic_error & e) {
    std::cout << "o.k. : exception thrown for absolute resource name" << std::endl;
  }

  // non-existant resource:
  try {
    rspath = lahague::get_resource("foo");
    return false;
  }
  catch (std::runtime_error & e) {
    std::cout << "o.k. : exception thrown for nonexistant resource name" << std::endl;
  }

  // Success:
  return true;
}

//! Return false if get_resource fails to return a valid resource
bool test_valid_resource()
{
  std::string rspath;

  try {
    // The 'true' argument allows the use of the MATERIAL_RESOURCE_DIR environement
    // variable to be used in place of the standard installation path.
    // Mandatory for testing before installation.
    std::string filename = "data/README.rst";
    rspath = lahague::get_resource(filename, true);
    std::ifstream check(rspath.c_str());
    if(!check.good()) {
      std::cerr << "FAIL : returned path to resource is unreadable ("
                << rspath
                << std::endl;
      return false;
    }
    std::cout << "o.k. : got valid path to resource '" << rspath << "'" << std::endl;
  }
  catch (std::exception & e) {
    std::cerr << "FAIL : exception thrown : " << e.what() << std::endl;
    return false;
  }

  return true;
}

//! group tests
int main(int /* argc_ */, const char ** /*argv_*/ )
{
  if(!test_invalid_resource()) return EXIT_FAILURE;
  if(!test_valid_resource()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
