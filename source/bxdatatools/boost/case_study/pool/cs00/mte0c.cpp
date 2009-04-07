#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "data.hpp"
#include "bag.hpp"

using namespace std;

int main (int argc_, char ** argv_)
{
  string fname = "mte0c.txt";
  int arg_nbags = 10;

  if (argc_ > 1) arg_nbags = atoi (argv_[1]);

  {
    clog << "Storing bags..." << endl;
    ofstream f (fname.c_str ());
    
    boost::archive::text_oarchive ar (f);
    
    uint32_t nbags = arg_nbags;

    // store the number of bags:
    ar & boost::serialization::make_nvp ("nbags", nbags);
    uint32_t sum_ok = 0;
    for (int i = 0; i < (int) nbags; i++)
      {
	// setup a bag instance:
	bag b;

	// initialize it:
	b.set (i);

	// check the 'bag sum' for it:
	if (b.check_sum ())
	  {
	    sum_ok++;
	  }
	else 
	  {
	    cerr << "Ooops! Sum is wrong!" << endl;	    
	  }	

	// store it:
	ar & boost::serialization::make_nvp ("Bag", b);
      }    
    if (sum_ok == nbags) clog << "All sums are ok!" << endl;
  }

  {
    clog << "Loading bags..." << endl;
    ifstream f (fname.c_str ());

    boost::archive::text_iarchive ar (f);

    list<bag> bags;

    uint32_t nbags = 10;
    // load the number of stored bags:
    ar & boost::serialization::make_nvp ("nbags", nbags);

    // iterate and load bag items from the archive:
    uint32_t sum_ok = 0;
    for (int i = 0; i < (int) nbags; i++)
      {
	// alloacte a new bag item in the collection:
	bags.push_back (bag ());

	// load it:
	ar & boost::serialization::make_nvp ("Bag", bags.back ());

	// check the 'bag sum' for the new item:
	if (bags.back ().check_sum ())
	  {
	    sum_ok++;
	  }
	else 
	  {
	    cerr << "Ooops! Sum is wrong!" << endl;	    
	  }	
      }
    if (sum_ok == nbags) clog << "All sums are ok!" << endl;
    else cerr << "Some sums are wrong!" << endl;
  }

  return 0;
}
