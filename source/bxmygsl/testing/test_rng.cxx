// test_rng.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/rng.h>

using namespace std;

int main(int argc_ , char ** argv_)
{
  try 
    {
      mygsl::rng::print_dict(clog);

      cerr << endl << "A PRNG : " << endl;
      string  rng_id = mygsl::rng::DEFAULT_RNG_ID;
      int32_t seed   = 12345;
      mygsl::rng r;
      r.initialize(rng_id, seed);
      r.dump(clog);

      size_t nshoots = 3;
    
      cerr << endl;
      cerr << "name=" << r.name() << endl;
      cerr << "min=" << r.min() << endl;
      cerr << "max=" << r.max() << endl;
      cerr << endl;
    
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform();
          cout << ran << endl;
        }
    
      cerr << "store..." << endl;
      r.store("test_rng.state");

      cerr << "load..." << endl;
      r.load("test_rng.state");

      cerr << endl;
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform();
          cout << ran << endl;
        }

      clog << "Store PRNG's internal state in a file : "<< endl;
      string sname = "test_rng_full.state";
      ofstream ofstate(sname.c_str());
      r.to_stream(ofstate);
      ofstate.close();
      cerr << endl << "Play a sequence : " << endl;
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform();
          cout << ran << endl;
        }
      cerr << endl;

      clog << "Load PRNG's internal state from a file : "<< endl;
      ifstream ifstate(sname.c_str());
      r.from_stream(ifstate);
      ifstate.close();

      cerr << endl << "Replay the same sequence : " << endl;
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform();
          cout << ran << endl;
        }
      cerr << endl;

      cerr << endl << "Reset the PRNG : " << endl;
      r.reset();
      cerr << endl << "Re-initialize the PRNG : " << endl;
      r.initialize(rng_id, seed);
      cerr << endl << "Play again the first sequence from the original seed : " << endl;
      for (int i = 0; i < nshoots; i++) 
        {
          double ran = r.uniform();
          cout << ran << endl;
        }
 
      cerr << endl << "Another PRNG with seed computed from current time : " << endl;
      mygsl::rng r2;
      r2.initialize(mygsl::random_utils::SEED_TIME);
      r2.dump(clog);

      try {
        cerr << endl << "Another PRNG with invalid initialization : " << endl;
        mygsl::rng r3;
        r3.initialize();
      } 
      catch (exception & xx)
        {
          clog << "NOTICE: "
               << "As expected, the PRNG's initialization failed : " 
               << xx.what() << endl;
        }
    }
  catch (exception & x) 
    {
      cerr << "ERROR: " << x.what() << endl;
      return(EXIT_FAILURE);
    }
  return(EXIT_SUCCESS);
}

// end of test_rng.cxx
