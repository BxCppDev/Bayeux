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

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'datatools::utils::lockfile' class..." << endl; 
  
      bool        verbose = false;
      bool        touch = false;
      string lockfile_dir = "/var/lock";
      string lockfile_name = lockfile_dir + "/test_lockfile.lock";

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];

	  if (arg == "-v" || arg == "--verbose") verbose = true;
	  if (arg == "-t" || arg == "--touch") touch = true;

	  iarg++;
	}

      /*
	datatools::utils::simple_lockfile my_lockfile (lockfile_name, 
	0, 
	true, 
	false);
	my_lockfile.verbose = verbose;
	my_lockfile.create ();

	int elapsed  = 0;
	int tick     = 1000000;
	int sec_tick = 1;
	while (elapsed < 10 * sec_tick ) 
	{
	if (touch) 
	{
	my_lockfile.touch ();
	clog << "Touched lockfile '" <<  lockfile_name 
	<< "'" << endl;
	}
	if ( my_lockfile.check () ) {
	clog << "Checked lockfile '" 
	<<  lockfile_name << "'" << endl;
	}
	usleep (tick);
	elapsed += sec_tick;
      
	}
      */

      /*
	my_lockfile.remove ();
	if ( !my_lockfile.check () ) {
	clog << "No more lockfile '" <<  lockfile_name << "'" << endl;
	}
      */

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

// end of test_lockfile.cxx
