// -*- mode: c++; -*-
// test_handle_macros.cxx
// Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/handle_macros.h>

int main (/*int argc_ , char ** argv_*/)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test of the 'handle_macros'..." << endl;
      // bool debug = false;

      // int iarg =  1;
      // while (iarg < argc_)
      //   {
      //     string arg = argv_[iarg];
      //     if ((arg == "-d") || (arg == "--debug")) debug = true;
      //     iarg++;
      //   }

      {
        cout << "************ DATATOOLS_HANDLE_DECLARE: " << endl;
        DATATOOLS_HANDLE_DECLARE(hi,int);
        hi.reset (new int(3));
        cout << "hi -> " << hi.get () << endl;
      }

      {
        cout << "************ DATATOOLS_HANDLE_DECLARE_NEW: " << endl;
        DATATOOLS_HANDLE_DECLARE_NEW(hi,int);
        {
          cout << "************ DATATOOLS_HANDLE_GRAB_REF: " << endl;
          DATATOOLS_HANDLE_GRAB_REF(i,hi,int);
          i = 666;
          cout << "hi -> " << hi.get () << endl;
        }
        {
          cout << "************ DATATOOLS_HANDLE_GET_REF: " << endl;
          DATATOOLS_HANDLE_GET_REF(ki, hi, int);
          cout << "ki = " << ki << endl;
        }
      }

      try
        {
          cout << "************ DATATOOLS_HANDLE_DECLARE: " << endl;
          DATATOOLS_HANDLE_DECLARE(hi,int);
          // Here no int has been allocated to the 'hi' handle
          cout << "************ DATATOOLS_HANDLE_GET_REF: " << endl;
          // The following operation will fail :
          DATATOOLS_HANDLE_GET_REF(ki, hi, int);
          cout << "ki = " << ki << endl;
        }
      catch (exception & x)
        {
          cout << "Ok ! We have missing data as expected !" << endl;
        }
    }
  catch (exception & x)
    {
      clog << "error: " << x.what () << endl;
      error_code =  EXIT_FAILURE;
    }
  catch (...)
    {
      clog << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_handle_macros.cxx
