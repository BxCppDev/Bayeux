// test_datatools.cxx

// Ourselves:
#include <datatools/datatools_config.h>
#include <datatools/version.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

int main (/* int argc_, char ** argv_ */) {
  try {
    std::cerr << "Datatools version is: " << datatools::version::get_version() << std::endl;
  }
  catch (std::exception & x)
  {
    std::cerr << "error: " << x.what () << std::endl;
    return EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
