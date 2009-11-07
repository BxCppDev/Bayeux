// test_numerical_differentiation.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath>

#include <mygsl/numerical_differentiation.h>

using namespace std;

double p ( double x_)
{
  double x = x_;
  return x * (1 - x * (1 - x * (1 - x * (1 - x * (1 - x * (1 - x))))));
}

double dp ( double x_)
{
  double x = x_;
  return 1 
    - 2 * x 
    + 3 * pow (x, 2) 
    - 4 * pow (x, 3)
    + 5 * pow (x, 4)
    - 6 * pow (x, 5)
    + 7 * pow (x, 6);

}

int main(int argc_ , char ** argv_)
{
  try 
    {
      const size_t NP = 20;
      double x[NP];
      double f[NP];
      double dfdx[NP];
      double h = 1 / 20.;
      for (int i = 0; i < NP; i++)
	{
	  x[i] = i * h;
	  f[i] = p (x[i]);
	  dfdx[i] = dp (x[i]);
	  cout << x[i] << ' ' 
	       << f[i] << ' ' 
	       << dfdx[i] 
	       << endl;
	}
      cout << endl;
      cout << endl;

      for (int i = 0; i < (NP-1); i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_forward_regular_sampled_1 (f[i], 
							      f[i+1],
							      h)
	       << endl;
	}
      cout << endl;
      cout << endl;

      for (int i = 1; i < NP; i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_backward_regular_sampled_1 (f[i-1], 
							       f[i],
							       h)
	       << endl;
	}
      cout << endl;
      cout << endl;

      for (int i = 1; i < (NP-1); i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_central_regular_sampled_2 (f[i-1], 
							      f[i+1],
							      h)
	       << endl;
	}
      cout << endl;
      cout << endl;

      for (int i = 2; i < (NP-2); i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_central_regular_sampled_4 (f[i-2],
							      f[i-1], 
							      f[i+1], 
							      f[i+2],
							      h)
	       << endl;
	}
      cout << endl;
      cout << endl;


      for (int i = 0; i < (NP-2); i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_forward_regular_sampled_2 (f[i], 
							      f[i+1],
							      f[i+2],
							      h)
	       << endl;
	}
      cout << endl;
      cout << endl;

      for (int i = 0; i < (NP-3); i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_forward_regular_sampled_3 (f[i], 
							      f[i+1],
							      f[i+2],
							      f[i+3],
							      h)
	       << endl;
	}
      cout << endl;
      cout << endl;

      for (int i = 2; i < NP; i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_backward_regular_sampled_2 (f[i-2], 
								f[i-1], 
								f[i],
								h)
	       << endl;
	}
      cout << endl;
      cout << endl;

      for (int i = 3; i < NP; i++)
	{
	  clog << "DEVEL: i=" << i << " x[i]=" << x[i] << endl;
	  cout << x[i] << ' ' 
	       << mygsl::derivative_backward_regular_sampled_3 (f[i-3], 
								f[i-2],
								f[i-1],
								f[i],
								h)
	       << endl;
	}
      cout << endl;
      cout << endl;

    }
  catch (exception & x) 
    {
      cerr << "ERROR: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_numerical_differentiation.cxx
