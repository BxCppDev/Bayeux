// test_unary_eval.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <mygsl/unary_eval.h>

#include <mygsl/unary_eval_with_derivative.h>

using namespace std;

struct cos_t : public mygsl::unary_eval
{
  double eval (double x_) const 
  {
    return std::cos (x_);
  }
};

struct cos2_t : public mygsl::unary_eval_with_derivative
{
  double eval_f (double x_) const 
  {
    return std::cos (x_);
  }
  double eval_df (double x_) const 
  {
    return -std::sin (x_);
  }
};


int main (int argc_, char ** argv_)
{
  try
    {
      bool debug = false;

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

      cos_t  c1;
      cos2_t c2;
      mygsl::unary_eval_promoted_with_numeric_derivative c3 (c1);

      /*
       * mygsl::unary_eval_from_native::native_eval e = std::cos;
       * void * ptr = static_cast<void *> (&e);
       * clog << "DEVEL: addr(cos) = " << ptr << endl;
       * clog << "DEVEL: addr(cos) = " << (void *) (*e) << endl;
       */
      mygsl::unary_eval_from_native c4 (std::cos);
      mygsl::unary_eval_from_native c5 (std::sin);

      for (double x = -2 * M_PI / 3; x <= 2 * M_PI / 3; x += 0.001)
	{
	  cout.precision (12);
	  cout << x 
	       << ' ' << c1 (x) 
	       << ' ' << c2 (x) << ' ' << c2.eval_df (x) 
	       << ' ' << c3 (x) << ' ' << c3.eval_df (x) 
	       << ' ' << c4 (x) << ' ' << -c5 (x) 
	       << endl;
	}
    }
  catch (exception & x) 
    { 
      cerr << "ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_unary_eval.cxx
