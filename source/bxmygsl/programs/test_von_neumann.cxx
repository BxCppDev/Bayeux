// test_von_neumann_method.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/von_neumann_method.h>
#include <mygsl/unary_eval.h>

using namespace std;

struct my_func : public mygsl::unary_eval
{
  double eval (double x_) const
  {
    if (x_ < 0.0) return 0.0;
    if (x_ > M_PI) return 0.0;
    return abs (sin (x_));
  }
  
};

int main (int argc_, char ** argv_)
{
  try 
    {
      string       rng_id = "mt19937";
      unsigned long int seed   = 12345;
    
      mygsl::rng rand (rng_id, seed);

      my_func func;

      mygsl::von_neumann_method::g_debug = true;
      mygsl::von_neumann_method VNM (0.0, M_PI, func);
      VNM.dump ();

      size_t nshoots = 100000;
      for (int i = 0; i < nshoots; i++) 
	{
	  double ran = VNM.shoot (rand);
	  cout << ran << endl;
	}

    }
  catch (exception & x) 
    {
      cerr << "ERROR: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_von_neumann_method.cxx
