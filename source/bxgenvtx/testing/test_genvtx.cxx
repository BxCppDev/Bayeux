// -*- mode: c++ ; -*-
// test_genvtx.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genvtx/genvtx_config.h>
#include <genvtx/version.h>

using namespace std;

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'genvtx' program library." << endl;

      clog << "genvtx version : " << GENVTX_LIB_VERSION << endl;

      clog << "The end." << endl;
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}
