// -*- mode: c++ ; -*-
// test_i_model.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools_test_model_1.cc>
using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'i_model'!" << endl;

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

      datatools::properties setup;
      setup.store ("width",      44.0); // mm
      setup.store ("height",     25.0); // mm
      setup.store ("material.ref",   "copper");
      setup.store ("visibility", true);

      geomtools::test_model_1 my_model;
      my_model.set_debug (debug);
      my_model.construct ("my_model", setup);
      my_model.tree_dump (clog, "my_model");

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

// end of test_i_model.cxx
