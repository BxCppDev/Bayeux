// -*- mode: c++ ; -*- 
// test_dummy_cuts.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <cuts/dummy_cuts.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'cuts::dummy_cuts'!" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
	      string option = token; 
	      if ((option == "-d") || (option == "--debug")) 
		{
		  debug = true;
		}
              else 
		{ 
		  clog << "warning: ignoring option '" 
		       << option << "'!" << endl; 
		}
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" 
		     << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
      cuts::dummy_cuts my_dummy_cuts;

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_dummy_cuts.cxx
