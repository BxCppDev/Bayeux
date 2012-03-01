// test_one_dimensional_root_finding.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <mygsl/error.h>
#include <mygsl/one_dimensional_root_finding.h>

using namespace std;

struct quadratic_t : public mygsl::unary_eval
{
  double a, b, c;

  quadratic_t ()
  {
    a =  1.0;
    b =  0.0;
    c = -5.0;
  }

  double eval (double x_) const 
  {
    double f = (a * x_ + b) * x_ + c;
    return f;
  }

};

int main (int argc_, char ** argv_)
{
  try
    {
      bool debug = false;
      string method_name = mygsl::one_dimensional_root_solver::BISECTION_METHOD_LABEL;
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
	      else if (arg == "--bisection") 
		{
		  method_name = mygsl::one_dimensional_root_solver::BISECTION_METHOD_LABEL;
		}
	      else if (arg == "--falsepos") 
		{
		  method_name = mygsl::one_dimensional_root_solver::FALSEPOS_METHOD_LABEL;
		}
	      else if (arg == "--brent") 
		{
		  method_name = mygsl::one_dimensional_root_solver::BRENT_METHOD_LABEL;
		}
	      else if (arg == "--newton") 
		{
		  method_name = mygsl::one_dimensional_root_solver::NEWTON_METHOD_LABEL;
		}
	      else if (arg == "--secant") 
		{
		  method_name = mygsl::one_dimensional_root_solver::SECANT_METHOD_LABEL;
		}
	      else if (arg == "--steffenson") 
		{
		  method_name = mygsl::one_dimensional_root_solver::STEFFENSON_METHOD_LABEL;
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

      mygsl::error::off ();

      quadratic_t func;

      mygsl::one_dimensional_root_solver odrs (debug);
      odrs.set_default_step_action ();
      odrs.init (func, method_name);
      
      if (odrs.solve (epsabs, 0.0, 5.0) == 0)
	{
	  clog.precision (12);
	  clog << "After " <<  odrs.get_iter () << " iterations" 
	       << " root finding method converged to: " << odrs.get_root () << endl;
	}
      else
	{
	  cerr << "Cannot find root after  " 
	       << odrs.get_iter () << " iterations" << endl;
	}

      clog << "Alternative call: " << endl;
      clog << "  root = " << mygsl::one_dimensional_root_solver::solve (func, epsabs, 0.0, 5.0, method_name) << endl;

    }
  catch (exception & x) 
    { 
      cerr << "ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_one_dimensional_root_solver.cxx
