//! \file  test_resource.cxx
//! \brief Unit tests for the materials::resource API

// Standard Library
#include <iostream>
#include <fstream>
#include <cstdlib>

// Third Party
// - datatools
#include <datatools/exception.h>

// This Project
#include <materials/resource.h>

//! Return false if get_resource passed an invalid resource
bool test_invalid_resource() {
  std::string rspath;

  // Absolute resource
  try {
    rspath = materials::get_resource("/foo");
    return false;
  }
  catch (std::logic_error& e) {
    std::cout << "o.k. : exception thrown for absolute resource name" << std::endl;
  }

  // non-existant resource
  try {
    rspath = materials::get_resource("foo");
    return false;
  }
  catch (std::runtime_error& e) {
    std::cout << "o.k. : exception thrown for nonexistant resource name" << std::endl;
  }

  // Success!!
  return true;
}

//! Return false if get_resource fails to return a valid resource
bool test_valid_resource() {
  std::string rspath;

  try {
    // The 'true' argument allows the use of the MATERIAL_DATA_DIR environement
    // variable to be used in place of the standard installation path.
    // Mandatory for testing before installation.
    rspath = materials::get_resource("data/std_isotopes.def", true);
    std::ifstream check(rspath.c_str());
    if(!check.good()) {
      std::cerr << "FAIL : returned path to resource is unreadable ("
                << rspath
                << std::endl;
      return false;
    }
    std::cout << "o.k. : got valid path to resource" << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "FAIL : exception thrown : " << e.what() << std::endl;
    return false;
  }

  return true;
}



//! group tests
int main(int argc, const char *argv[]) {
  if(!test_invalid_resource()) return EXIT_FAILURE;
  if(!test_valid_resource()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}


