// test_mean.cxx

#include <cstdlib>
#include <iostream>
#include <cmath>

#include <mygsl/mean.h>

using namespace std;
using namespace mygsl;

int main (/*int argc_ , char ** argv_*/)
{
  try
    {
      long seed = 314159;
      srand48 (seed);

      arithmetic_mean AM;
      for (int i = 0; i < 10; i++)
        {
          AM.add (drand48 ());
        }
      cout << "Number of values = "<< AM.get_n () << endl;
      cout << "Arithmetic mean  = "<< AM.get_mean () << endl;
      cout << "Variance         = "<< AM.get_variance () << endl << endl;

      AM.remove (drand48 ());
      AM.remove (drand48 ());
      cout << "Number of values = "<< AM.get_n () << endl;
      cout << "Arithmetic mean  = "<< AM.get_mean () << endl;
      cout << "Variance         = "<< AM.get_variance () << endl << endl;

      weighted_mean WM;
      for (int i = 0; i < 10; i++)
        {
          WM.add (drand48 (), drand48 ());
        }
      cout << "Weighted mean    = "<< WM.get_mean () << endl << endl;

    }
  catch (exception & x)
    {
      cerr << "CONSTANTS: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_mean.cxx
