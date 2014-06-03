// test_min_max.cxx

#include <cstdlib>
#include <iostream>
#include <cmath>

#include <mygsl/min_max.h>

using namespace std;
using namespace mygsl;

int main (/*int argc_ , char ** argv_*/)
{
  try
    {
      long seed = 314159;
      srand48 (seed);

      min_max mM;
      for (int i = 0; i < 10; i++)
        {
          mM.add (drand48 ());
        }
      cout << "Minimum value = "<< mM.get_min () << endl;
      cout << "Maximum value = "<< mM.get_max () << endl;
      cout << "Median value  = "<< mM.get_median () << endl;

    }
  catch (exception & x)
    {
      cerr << "CONSTANTS: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_min_max.cxx
