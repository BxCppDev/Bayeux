// test_rng_2.cxx

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
      mygsl::rng::g_debug = false;

      if (mygsl::rng::g_debug) mygsl::rng::print_dict (clog);

      string            rng_id = mygsl::rng::DEFAULT_RNG_TYPE;
      unsigned long int seed   = 12345;
    
      mygsl::rng r;
      r.init (rng_id, seed);
    
      mygsl::rng::state_buffer_t buffer_init;
      clog << "Storing the initial internal state in some buffer..." << endl;
      r.to_buffer (buffer_init);
      clog << endl;
      clog << "name = '" << r.name () << "'" << endl;
      clog << "min  = " << r.min () << endl;
      clog << "max  = " << r.max () << endl;
      clog << "internal state : " << buffer_init.size () << " bytes" << endl;
      clog << endl;
    
      clog << "Fire some initial random values : " << endl;
      size_t nshoots = 3;
      for (int i = 0; i < nshoots; i++) 
	{
	  double ran = r.uniform ();
	  cout << ran << endl;
	}
    
      clog << "Storing the internal state in some buffer..." << endl;
      mygsl::rng::state_buffer_t buffer;
      r.to_buffer (buffer);

      clog << "Fire some other values :" << endl;
      nshoots = 5;
      for (int i = 0; i < nshoots; i++) 
	{
	  double ran = r.uniform ();
	  cout << ran << endl;
	}

      clog << "Restore the internal state from the buffer..." << endl;
      r.from_buffer (buffer);
      clog << "Fire again some values :" << endl;
      for (int i = 0; i < nshoots; i++) 
	{
	  double ran = r.uniform ();
	  cout << ran << endl;
	}

      clog << "Restore the initial internal state..." << endl;
      r.from_buffer (buffer_init);
      clog << "Fire again some values :" << endl;
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

// end of test_rng_2.cxx
