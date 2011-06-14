// -*- mode: c++; -*- 
// test_time_tools.cxx

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/time_tools.h>
#include <datatools/utils/clhep_units.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'time_tools' classes..." << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg <  argc_) 
	{
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug =  true;

	  iarg++;
	}
    
      long seed = 314159;
      srand48 (seed);

      datatools::utils::computing_time CT;

      size_t max_run = 50;
      for (int run = 0; run < max_run; run++)
	{
	  size_t run_size = (size_t) (10000 + 10000 * drand48 ());
	  if (debug) cerr << "DEBUG: Run size = " << run_size << endl;
	  CT.start (); // start the clock
	  for (int i = 0; i < run_size; i++)
	    {
	      double t = drand48 () * 2 * M_PI;
	      double x = cos (t);
	      CT.pause ();
	      double y = sin (t);
	      CT.resume ();
	    }
	  CT.stop (); // stop the clock
	  cout << CT.get_last_elapsed_time () / CLHEP::second << endl;
	}

      CT.tree_dump (clog, "Computing time: ", "NOTICE: ");
      CT.reset ();
      clog << "Reset the 'computing time' tools !" << endl;
      CT.tree_dump (clog, "Computing time: ", "NOTICE: ");

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what() << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_time_tools.cxx
