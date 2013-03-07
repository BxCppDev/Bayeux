// test_rng.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/rng.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  try 
    {
      mygsl::rng::g_debug = true;

      if (mygsl::rng::g_debug) mygsl::rng::print_dict (clog);

      string            rng_id = mygsl::rng::DEFAULT_RNG_TYPE;
      unsigned long int seed   = 12345;
    
      mygsl::rng r;
      r.initialize (rng_id,seed);
      r.dump (clog);

      size_t nshoots = 3;
    
      cerr << endl;
      cerr << "name=" << r.name () << endl;
      cerr << "min=" << r.min () << endl;
      cerr << "max=" << r.max () << endl;
      cerr << endl;
    
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform ();
          cout << ran << endl;
        }
    
      cerr << "store..." << endl;
      r.store ("test_rng.state");

      cerr << "load..." << endl;
      r.load ("test_rng.state");

      cerr << endl;
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform ();
          cout << ran << endl;
        }

      string sname = "test_rng_full.state";
      ofstream ofstate (sname.c_str ());
      r.to_stream (ofstate);
      ofstate.close ();

      ifstream ifstate (sname.c_str ());
      r.from_stream (ifstate);
      ifstate.close ();

      cerr << endl;
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform ();
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

// end of test_rng.cxx
