// -*- mode: c++; -*- 
// test_library_loader_1.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include <datatools/utils/library_loader.h>

using namespace std;

int main (int argc_ , char ** argv_)
{

  int error_code = EXIT_SUCCESS;
  try 
    { 
      clog << "Test of the 'datatools::utils::library_loader' class." << endl; 
  
      bool   debug = false;
      bool   test = false;
      string libdir = "";
      string libname = "";

      int iarg = 1;
      while (iarg <  argc_) 
        {
          string arg = argv_[iarg];
          if (boost::algorithm::starts_with (arg, "-"))
            {
              if (arg == "-d" || arg == "--debug") 
                {
                  debug =  true;
                }
             if (arg == "-t" || arg == "--test") 
                {
                  test =  true;
                }
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

      using namespace datatools::utils;
 
      library_loader::g_devel = debug;
      library_loader::g_test  = test;
 
      if (libname.empty ()) 
	{
	  libname = "crypto"; // or "curses" ...
	}

      string LL_config = "${DATATOOLS_ROOT}/testing/config/test_library_loader.conf";
      uint32_t LL_flags = datatools::utils::library_loader::allow_unregistered;
      datatools::utils::library_loader LL (LL_flags, LL_config);
      datatools::utils::library_loader LL2 (LL_flags, LL_config);
 
      clog << " Loading 'ssl'..." << endl;
      LL.load ("ssl");
 
      // clog << "Loading 'mygsl'..." << endl;
      // LL.load ("mygsl");

      // clog << "Loading 'geomtools'..." << endl;
      // LL.load ("geomtools");

      clog << "Loading '" << libname << "'..." << endl;
      LL.load (libname);

      clog << "Done." << endl;
      LL.print (clog);

      clog << "Closing '" << libname << "'..." << endl;
      LL.close (libname);
 
      clog << "Loading 'geomtools' (2)..." << endl;
      LL2.load ("geomtools");

      LL2.print (clog);

      clog << "Closing all..." << endl;
      LL.close_all ();

      //clog << "Closing 'geomtools'..." << endl;
      //library_loader::get_library_loader ().close ("geomtools");

      //clog << "Closing 'mygsl'..." << endl;
      //library_loader::get_library_loader ().close ("mygsl");

      //clog << "Closing 'ssl'..." << endl;
      //library_loader::get_library_loader ().close ("ssl");
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

// end of test_library_loader_1.cxx
