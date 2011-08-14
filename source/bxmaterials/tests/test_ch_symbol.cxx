// -*- mode: c++ ; -*- 
// test_dummy_materials.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <materials/ch_symbol.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'dummy_materials'!" << endl; 
  
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
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
      for (int i = 0; i < mat::chemical_symbol::NB_CHEMICAL_SYMBOLS; i++)
	{
	  cout << i << " " << mat::chemical_symbol::table[i] << endl;
	}

      string symbol = "H";
      cout << "Symbol='" << symbol << "' Z=" << mat::chemical_symbol::Z_from_symbol (symbol)  << endl;

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

// end of test_dummy_materials.cxx
