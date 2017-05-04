// -*- mode: c++ ; -*-
// test_and_cut.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <cuts/and_cut.h>
#include <cuts/accept_cut.h>
#include <cuts/reject_cut.h>
#include <cuts/cut_tools.h>

using namespace std;

int main (int /*argc_*/, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'cuts::and_cut'!" << endl;

      /*
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
      */

      cuts::cut_handle_type ha1 (new cuts::accept_cut);
      cuts::cut_handle_type ha2 (new cuts::accept_cut);
      cuts::cut_handle_type hr1 (new cuts::reject_cut);
      cuts::cut_handle_type hr2 (new cuts::reject_cut);

      cuts::and_cut my_and_cut;
      my_and_cut.set_cuts (ha1, ha2);
      //my_and_cut.set_cuts (ha1, hr1);
      //my_and_cut.set_cuts (hr1, hr2);

      int i;
      my_and_cut.set_user_data (&i);
      int status = my_and_cut.process ();
      if (status == cuts::SELECTION_ACCEPTED)
        {
          clog << "Accepted !" << endl;
        }
      else if (status == cuts::SELECTION_REJECTED)
        {
          clog << "Rejected !" << endl;
        }
      else
        {
          cerr << "Unapplicable !" << endl;
        }

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

// end of test_and_cut.cxx
