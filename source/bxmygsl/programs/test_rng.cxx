// test_rng.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/rng.h>

int main( int argc_ , char ** argv_ )
{
  try {

    mygsl::rng::g_debug = true;

    if ( mygsl::rng::g_debug ) mygsl::rng::print_dict(std::cerr);

    std::string       rng_id = "mt19937";
    unsigned long int seed   = 12345;
    
    mygsl::rng r;
    r.init(rng_id,seed);

    size_t nshoots=3;
    
    std::cerr << std::endl;
    std::cerr << "name=" << r.name() << std::endl;
    std::cerr << "min=" << r.min() << std::endl;
    std::cerr << "max=" << r.max() << std::endl;
    std::cerr << std::endl;
    
    for (int i=0; i<nshoots; i++) {
      double ran=r.uniform();
      std::cout << ran << std::endl;
    }
    
    std::cerr << "store..." << std::endl;
    r.store("test_rng.state");

    std::cerr << "load..." << std::endl;
    r.load("test_rng.state");

    std::cerr << std::endl;
    for (int i=0; i<nshoots; i++) {
      double ran=r.uniform();
      std::cout << ran << std::endl;
    }

    std::string sname="test_rng_full.state";
    std::ofstream ofstate(sname.c_str());
    r.to_stream(ofstate);
    ofstate.close();

    std::ifstream ifstate(sname.c_str());
    r.from_stream(ifstate);
    ifstate.close();

    std::cerr << std::endl;
    for (int i=0; i<nshoots; i++) {
      double ran=r.uniform();
      std::cout << ran << std::endl;
    }

  }
  catch( std::exception & x ) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// end of test_rng.cxx
