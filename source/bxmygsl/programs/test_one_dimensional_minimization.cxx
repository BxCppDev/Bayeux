// test_one_dimensional_minimization.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <mygsl/one_dimensional_minimization.h>

using namespace std;

struct func_t : public mygsl::unary_eval
{
  func_t ()
  {
  }

  double eval (double x_) const 
  {
    double f = cos (x_) + 1.0;
    return f;
  }

};

int main (int argc_, char ** argv_)
{
  try
    {
      bool debug = false;
      string method_name = mygsl::one_dimensional_minimization::GOLDENSECTION_METHOD_LABEL;
      double epsabs = 1.e-3;

      int iarg = 1;
      while (iarg < argc_)
	{
	  string arg = argv_[iarg];

	  if (arg[0] == '-') 
	    {	
	      if (arg == "--debug" || arg == "-d") 
		{
		  debug = true;
		}
	      else if (arg == "--goldensection" || arg == "-g") 
		{
		  method_name = mygsl::one_dimensional_minimization::GOLDENSECTION_METHOD_LABEL;
		}
	      else if (arg == "--brent" || arg == "-b") 
		{
		  method_name = mygsl::one_dimensional_minimization::BRENT_METHOD_LABEL;
		}
	      else if (arg == "-3") 
		{
		  epsabs = 1.e-3;
		}
	      else if (arg == "-5") 
		{
		  epsabs = 1.e-5;
		}
	      else if (arg == "-7") 
		{
		  epsabs = 1.e-7;
		}
	      else
		{
		  clog << "WARNING: Ignoring option '" << arg << "'!" << endl;
		}
	    }
	  else
	    {
	      clog << "WARNING: Ignoring argument '" << arg << "'!" << endl;
	    }
	  iarg++;
	}

      func_t func;

      mygsl::one_dimensional_minimization odm (debug);
      odm.set_default_step_action ();  
      odm.init (func, method_name);
      
      if (odm.minimize (0.0, 2 * M_PI - 0.1, 2.0, epsabs) == 0)
	{
	  clog.precision (12);
	  clog << "After " <<  odm.get_iter () << " iterations" 
	       << " minimization method converged to: " 
	       << odm.get_minimum_value () << endl;
	}
      else
	{
	  cerr << "Cannot find root after  " 
	       << odm.get_iter () << " iterations" << endl;
	}

      clog << "Alternative call: " << endl;
      clog << "  minimum = " << mygsl::one_dimensional_minimization::minimize (func, 
									       0.0, 
									       2 * M_PI - 0.1, 
									       2.0, 
									       epsabs, 
									       method_name) << endl;
    }
  catch (exception & x) 
    { 
      cerr << "ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_one_dimensional_minimization.cxx
