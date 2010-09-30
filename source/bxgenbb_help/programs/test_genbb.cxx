// -*- mode: c++; -*- 
// test_genbb.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/genbb.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  
      bool debug = false;
      bool test = false;

      int iarg = 1;
      while (iarg < argc_)
	{
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;
	  if (arg == "-t" || arg == "--test") test = true;

	  iarg++;
	}
    
      genbb::genbb GBB;

      datatools::utils::properties config;
      if (debug) config.store_flag ("debug");
      if (test) config.store_flag ("test");
      config.store ("seed", 314159);
      config.store ("buffer_size", 10000);
      config.store ("decay_type", "DBD");
      config.store ("decay_isotope", "Se82");
      config.store ("decay_dbd_level", 0);
      config.store ("decay_dbd_mode", 1);
      if (debug) config.tree_dump (clog, "Configuration: ", "debug: ");
 
      // genbb input data files:
      GBB.initialize (config);

      size_t max_count = 27000;

      // working primary event:
      genbb::primary_event pe;

      for (int i = 0; i < max_count; i++)
	{
	  clog << "Count : " << i << endl;
	  GBB.load_next (pe);
	  if (debug) pe.dump ();
	}

       if (debug) clog << "debug: " << "The end." << endl;
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

// end of test_genbb.cxx
