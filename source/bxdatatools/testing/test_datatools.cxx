// test_datatools.cxx

// Ourselves:
#include <bayeux/bayeux.h>
#include <datatools/datatools.h>
#include <datatools/datatools_config.h>
#include <datatools/version.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

int main (int /* argc_ */, char ** /*argv_*/)
{
  bayeux::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'datatools' module..." << std::endl;
    std::clog << "Datatools version is: " << datatools::version::get_version() << std::endl;

  } catch (std::exception & x) {
    std::clog << "error: " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}
 
