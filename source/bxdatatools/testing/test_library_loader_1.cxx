// -*- mode: c++; -*-
// test_library_loader_1.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

// Third Party:
// - Boost:
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/library_loader.h>

int main (int argc_ , char ** argv_)
{
  using namespace std;

  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test of the 'datatools::library_loader' class." << endl;

      // bool   debug = false;
      // bool   test = false;
      string libdir = "";
      string libname = "";

      int iarg = 1;
      while (iarg <  argc_)
        {
          string arg = argv_[iarg];
          if (boost::algorithm::starts_with (arg, "-"))
            {
             //  if (arg == "-d" || arg == "--debug")
             //    {
             //      debug =  true;
             //    }
             //  if (arg == "-t" || arg == "--test")
             //    {
             //      test =  true;
             //    }
            }
          else
            {
              if (libname.empty ())
                {
                  libname = arg;
                }
              else if (libdir.empty ())
                {
                  libdir = arg;
                }
            }
          iarg++;
        }

      if (libname.empty ())
        {
          libname = "crypto"; // or "curses" ...
        }

      string LL_config = "${DATATOOLS_TESTING_DIR}/config/test_library_loader.conf";
      datatools::library_loader LL (LL_config);
      datatools::library_loader LL2 (LL_config);

      clog << " Loading 'ssl'..." << endl;
      LL.load ("ssl");

      clog << "Loading '" << libname << "'..." << endl;
      LL.load (libname);

      clog << "Done." << endl;
      LL.print (clog);

      clog << "Closing '" << libname << "'..." << endl;
      LL.close (libname);

      LL2.print (clog);

      clog << "Closing all..." << endl;
      LL.close_all ();

      clog << "Closing all (2)..." << endl;
      LL2.close_all ();

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
  return error_code;
}
