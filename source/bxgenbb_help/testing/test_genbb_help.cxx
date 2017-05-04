// -*- mode: c++ ; -*-
// test_genbb_help.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/version.h>

int main (/*int argc_, char ** argv_*/)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for the 'genbb_help' program library." << endl;

    clog << "genbb_help version : " << GENBB_HELP_LIB_VERSION << endl;

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
