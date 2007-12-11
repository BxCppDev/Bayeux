// test_histogram.cxx
/*
 * Histogram 1D sample program
 *
 *
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/histogram.h>


int main( int argc_ , char ** argv_ )
{
  try {

    mygsl::histogram::g_debug = true;

    long   seed = 12345;
    size_t bins = 100;


    mygsl::histogram h(bins,0,10.);
    
    srand48(seed);

    size_t nshoots=10000;
    for ( int i=0; i<nshoots; i++ ) {
      double x = -std::log(drand48());
      h.fill(x);
    }

    std::string sname="test_histogram.data";
    std::ofstream ofhist(sname.c_str());
    h.print(ofhist);
    ofhist.close();

    std::string sname2="test_histogram.store";
    std::ofstream ofstore(sname2.c_str());
    h.to_stream(ofstore);
    ofstore.close();

    std::ifstream ifstate(sname2.c_str());
    h.from_stream(ifstate);
    ifstate.close();

    for ( int i=0; i<nshoots; i++ ) {
      double x = -std::log(drand48());
      h.fill(x);
    }

    h.scale(1.4);
    h.shift(100.0);
    
    std::string sname3="test_histogram2.data";
    std::ofstream ofhist2(sname3.c_str());
    h.print(ofhist2);
    ofhist2.close();

    std::cout << "Histogram:" << std::endl;
    std::cout << "  Min   = " << h.min() << std::endl;
    std::cout << "  Max   = " << h.max() << std::endl;
    std::cout << "  #bins = " << h.bins() << std::endl;
    std::cout << "  Mean  = " << h.mean() << std::endl;
    std::cout << "  Sigma = " << h.sigma() << std::endl;
    std::cout << "  Sum   = " << h.sum() << std::endl;
 
    /*
    size_t bins2 = 25;
    mygsl::histogram h2(h);
    h2.rebin(bins2);
    std::string sname4="test_histogram3.data";
    std::ofstream ofhist3(sname4.c_str());
    h2.print(ofhist3);
    ofhist3.close();
    */

  }
  catch( std::exception & x ) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// end of test_histogram.cxx
