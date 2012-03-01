// -*- mode: c++; -*- 
// test_genbb_mgr_4.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/genbb_mgr.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
	{
	  std::string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;

	  iarg++;
	}
    
      // set format to Boost archive:
      genbb::genbb_mgr mgr (genbb::genbb_mgr::FORMAT_BOOST);

      // Boost archive input data files:
      mgr.set ("${GENBB_HELP_DATA_DIR}/testing/data/bipo212_1.boost.txt.gz");
      mgr.set ("${GENBB_HELP_DATA_DIR}/testing/data/bipo212_2.boost.txt.gz");
      mgr.set ("${GENBB_HELP_DATA_DIR}/testing/data/bipo212_3.boost.txt.gz");
      if (debug) mgr.dump ();

      // initialize/lock the manager:
      mgr.init ();
      if (debug) mgr.dump ();

      // working primary event:
      genbb::primary_event pe;

      size_t count = 0;
      // main loop on primary events source:
      while (mgr.has_next ())
	{
	  mgr.load_next (pe);
	  if (debug) pe.dump ();
	  count++;
	}
      mgr.reset ();

      std::clog << "Number of loaded events: " << count << std::endl; 
      if (debug) mgr.dump ();
   
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_genbb_mgr_4.cxx
