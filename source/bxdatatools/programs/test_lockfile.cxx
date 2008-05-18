// -*- mode: c++; -*- 
// test_lockfile.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <unistd.h>

#include <datatools/utils/simple_lockfile.h>
#include <boost/filesystem.hpp>

int 
main(int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      std::cerr << "Hello, this is a sample program\n"
		<< "for class 'datatools::utils::lockfile'!" << std::endl; 
  
      bool        verbose = false;
      bool        touch = false;
      std::string lockfile_dir = "/var/lock";
      std::string lockfile_name = lockfile_dir + "/test_lockfile.lock";

      int iarg = 1;
      while (iarg < argc_) 
	{
    
	  std::string arg = argv_[iarg];

	  if (arg == "-v" || arg == "--verbose") verbose = true;
	  if (arg == "-t" || arg == "--touch") touch = true;

	  iarg++;
	}

      datatools::utils::simple_lockfile my_lockfile(lockfile_name, 
						    0, 
						    true, 
						    false);
      my_lockfile.verbose = verbose;
      my_lockfile.create();

      int elapsed  = 0;
      int tick     = 1000000;
      int sec_tick = 1;
      while (elapsed < 10 * sec_tick ) 
	{
	  if (touch) 
	    {
	      my_lockfile.touch();
	      std::clog << "Touched lockfile '" <<  lockfile_name 
			<< "'" << std::endl;
	    }
	  if ( my_lockfile.check() ) {
	    std::clog << "Checked lockfile '" 
		      <<  lockfile_name << "'" << std::endl;
	  }
	  usleep(tick);
	  elapsed += sec_tick;
      
	}

      /*
	my_lockfile.remove();
	if ( !my_lockfile.check() ) {
	std::clog << "No more lockfile '" <<  lockfile_name << "'" << std::endl;
	}
      */

    }
  catch (std::exception & x) 
    {
      std::cerr << "error: " << x.what() << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_lockfile.cxx
