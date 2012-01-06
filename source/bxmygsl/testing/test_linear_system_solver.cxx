// test_linear_system_solver.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <ctime> 

#include <mygsl/linear_system_solver.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  try 
    {
      string mode = "test";

      if (argc_ > 1)
	{
	  if (string (argv_[1]) == "--test") 
	    {
	      mode = "test";
	    }
	  if (string (argv_[1]) == "--unit") 
	    {
	      mode = "unit";
	    }
	  if (string (argv_[1]) == "--random") 
	    {
	      mode = "random";
	    }
	  if (string (argv_[1]) == "--bad") 
	    {
	      mode = "bad";
	    }
	}

      size_t dimension = 3;

      vector<double> a;
      a.assign (dimension * dimension, 0.0);

      vector<double> b;
      b.assign (dimension, 0.0);

      if (mode == "unit")
	{
	  a[0] = 1.0;
	  a[4] = 1.0;
	  a[8] = 1.0;

	  b[0] = 1.0;
	  b[1] = 1.0;
	  b[2] = 1.0;
	}
      else if (mode == "random")
	{
	  time_t now;
	  time (&now);
	  long seed = (long) now;
 	  srand48 (seed);
	  for (int i = 0; i < (dimension * dimension); i++)
	    {
	      a[i] = drand48 (); 
	    }
	  for (int i = 0; i < dimension; i++)
	    {
	      b[i] = drand48 (); 
	    }
	}
      else if (mode == "test")
	{
	  a[0] = 0.18;  a[1] = 0.60;  a[2] = 0.60;
	  a[3] = 0.96;  a[4] = 0.41;  a[5] = 0.24;
	  a[6] = 0.99;  a[7] = 0.58;  a[8] = 0.14;

	  b[0] = 1.0;
	  b[1] = 2.0;
	  b[2] = 3.0;
	}
      else if (mode == "bad")
	{
	  a[0] = 1.0;
	  a[4] = 1.0;
	  a[8] = 0.0;

	  b[0] = 1.0;
	  b[1] = 1.0;
	  b[2] = 1.0;
	}

      vector<double> x;
      x.assign (dimension, 0.0);

      mygsl::linear_system_solver LSS (dimension);
      if (LSS.solve (a, b, x))
	{
	  throw runtime_error ("No solution!");
	}
      else
	{
	  cout << "Solution: " << endl;
	  for (int i = 0; i < dimension; i++)
	    {
	      cout << "  x[" << i << "] = " << x[i] << endl;
	    }
	}

      {
	double a1, b1, c1;
	double a2, b2, c2;
	double x,y;
	a1 = 1.0;
	b1 = 2.0;
	c1 = 3.0;
	a2 = 4.0;
	b2 = 5.0;
	c2 = 0.0;
	if (mygsl::linear_system_2x2_solve (a1, b1, c1, a2, b2, c2, x, y))
	  {
	    cout << "Solution: " << endl;
	    cout << "  x = " << x << endl;
	    cout << "  y = " << y << endl;
	  }
	else
	  { 
	    cout << "No solution." << endl;
	  }
      }

    }
  catch (exception & x) 
    {
      cerr << "ERROR: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_linear_system_solver.cxx
