// -*- mode: c++; -*- 
// prg_genbb_mgr.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <genbb_help/genbb_mgr.h>

int
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  
      bool debug = false;
      std::string genbb_file = "";

      int iarg = 1;
      while (iarg < argc_)
	{
	  std::string arg = argv_[iarg];
	  if (arg[0] == '-')
	    {
	      if (arg == "-d" || arg == "--debug") debug = true;
	    }
	  else
	    {
	      if (genbb_file.empty ())
		{
		  genbb_file = arg;
		}
	    }
	  iarg++;
	}
    
      if (genbb_file.empty ())
	{
	  throw std::runtime_error ("Missing GENBB generated events file!");
	}

      genbb::genbb_mgr mgr;

      // genbb input data files:
      mgr.set (genbb_file);
      if (debug) mgr.dump ();

      // initialize the manager:
      mgr.init ();
      if (debug) mgr.dump ();

      // working primary event:
      genbb::primary_event pe;

      size_t count = 0;
      // main loop on primary events source:
      while (mgr.has_next ())
	{
	  mgr.load_next (pe);
	  pe.dump (std::cout);
	  std::cout << std::endl;
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

// end of prg_genbb_mgr.cxx
