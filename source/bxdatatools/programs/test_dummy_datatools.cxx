// -*- mode: c++; -*- 
// test_dummy_datatools.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/dummy_datatools.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'dummy_datatools' class..." << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg <  argc_) 
	{
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug =  true;

	  iarg++;
	}
    
      datatools::dummy_datatools my_dummy_datatools;

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

// end of test_dummy_datatools.cxx
