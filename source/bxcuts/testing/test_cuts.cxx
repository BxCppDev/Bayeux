// test_cuts.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <cuts/cuts_config.h>
#include <cuts/version.h>

int main (int /*argc_*/, char ** /*argv_*/)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
      clog << "Test program for the 'cuts' program library." << endl;
      clog << "cuts version : " << CUTS_LIB_VERSION << endl;
      clog << "The end." << endl;
    }
  catch (exception & x) {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...) {
      cerr << "error: " << "unexpected error !" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}
