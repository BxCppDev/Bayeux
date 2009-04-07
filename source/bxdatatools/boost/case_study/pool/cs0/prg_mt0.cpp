#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "datum.hpp"

using namespace std;

int main (int argc_, char ** argv_)
{
  string fname = "prg_mt0.txt";
  int arg_ncol = 10;

  if (argc_ > 1) arg_ncol = atoi (argv_[1]);

  {
    clog << "Storing data..." << endl;
    ofstream f (fname.c_str ());
    
    boost::archive::text_oarchive ar (f);
    
    // generate the 'datum' collection:
    uint32_t ncol = arg_ncol;

    // store the number of col:
    ar & ncol;

    // create many 'datum' items and store each of them:
    uint32_t sum = 0;
    for (int i = 0; i < (int) ncol; i++)
      {
	// allocate a new 'datum' item:
	datum * p = new datum (10 * (i + 1));

	clog << "-- datum value is:  " << p->get_value () << endl;

	// compute the cumulative sum from all items:
	sum += p->get_value ();

	// store the current item:
	ar & *p;

	// free memory:
	delete p;	
      }
    clog << "-- computed sum is: " << sum << endl;
    clog << "-- expected sum is: " << 10 * (ncol) * (ncol + 1) / 2 << endl;
    clog << "-- output archive file is: '" << fname << "'" << endl;
  }

  {
    clog << "Loading data..." << endl;
    ifstream f (fname.c_str ());

    boost::archive::text_iarchive ar (f);
    clog << "-- input archive file is: '" << fname << "'" << endl;
    uint32_t ncol = 0;

    // load the number of stored col:
    ar & ncol;
    clog << "-- number of item is: '" << ncol << "'" << endl;

    // create many 'datum' items and load each of them:
    uint32_t sum = 0;
    for (int i = 0; i < (int) ncol; i++)
      {
	// allocate a new default 'datum' item in the collection:
	datum * p = new datum;

	// load the current item:
	ar & *p;

	clog << "-- datum value is:  " << p->get_value () << endl;

	// compute the cumulative sum from all items:
	sum += p->get_value ();
	
	// free memory:
	delete p;	
      }
    clog << "-- computed sum is: " << sum << endl;
    clog << "-- expected sum is: " << 10 * (ncol) * (ncol + 1) / 2 << endl;
  }

  return 0;
}
