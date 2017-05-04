// -*- mode: c++ ; -*- 
// prog_emfield.cxx
 
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <emfield/emfield_config.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  
      bool debug = false;
      bool verbose = false;
      bool show_intro = true;

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
               else if ((option == "-v") || (option == "--verbose")) 
                 {
                   verbose = true;
                 }
               else if ((option == "-N") || (option == "--no-intro")) 
                 {
                   show_intro = false;
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

      if (show_intro)
	{
	  clog << endl;
	  clog << "\tprog_emfield" << endl;
	  clog << "\tVersion " << EMFIELD_LIB_VERSION << endl; 
	  clog << "\tLicence : GPL 3.0" << endl; 
	  clog << endl;
	}
  
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

// end of prog_emfield.cxx
