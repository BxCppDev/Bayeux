// -*- mode: c++; -*- 
// test_geomtools.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/geomtools.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
  {

    cerr << "Hello, this is a sample program for class 'geomtools'!" << endl; 
  
    bool debug = false;

    int iarg = 1;
    while (iarg < argc_)
    {
    
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      iarg++;
    }
    
    clog << "Welcome in the 'geomtools' library version " << GEOMTOOLS_LIB_VERSION << " !" << endl;

  }
  catch (exception & x)
  {
    cerr << "error: " << x.what() << endl; 
    error_code = EXIT_FAILURE;
  }
  catch (...)
  {
    cerr << "error: " << "unexpected error!" << endl; 
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_geomtools.cxx
