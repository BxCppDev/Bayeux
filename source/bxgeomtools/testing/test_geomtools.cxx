// -*- mode: c++ ; -*-
// test_geomtools.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/geomtools_config.h>
#include <geomtools/version.h>
#include <datatools/library_loader.h>

using namespace std;

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'geomtools' program library." << endl;

      clog << "geomtools version : " << GEOMTOOLS_LIB_VERSION << endl;

      datatools::library_loader LL;
      LL.load ("camp");
      LL.load ("datatools_reflection");
      //LL.load ("datatools_bio");
      //LL.load ("mygsl_bio");
      LL.load ("geomtools_reflection");
      LL.load ("geomtools_bio");
      LL.print (clog);

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
