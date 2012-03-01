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
  string fname = "mte0b.txt";
  int arg_nbags = 10;

  if (argc_ > 1) arg_nbags = atoi (argv_[1]);

  {
    clog << "Storing bags..." << endl;
    ofstream f (fname.c_str ());
    
    boost::archive::text_oarchive ar (f);
    
    list<bag> bags;
    
    // generate the 'bags' collection:
    uint32_t nbags = arg_nbags;
    uint32_t sum_ok = 0;
    for (int i = 0; i < (int) nbags; i++)
      {
	// append a new bag to the 'bags' collection:
	bags.push_back (bag ());

	// initialize the new item:
	bags.back ().set (i);

	// check the 'bag sum' for the new item:
	if (bags.back ().check_sum ())
	  {
	    sum_ok++;
	  }
	else 
	  {
	    cerr << "Ooops! Sum is bad!" << endl;	    
	  }	
      }    
    if (sum_ok == nbags) clog << "All sums are ok!" << endl;
    
    // store the number of bags:
    ar & boost::serialization::make_nvp ("nbags", nbags);

    // traverse the bags collection and store each item:
    for (list<bag>::const_iterator i = bags.begin (); 
	 i != bags.end (); 
	 i++)
      {
	// store the current bag item:
	ar & boost::serialization::make_nvp ("Bag", *i);
      }
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
    for (int i = 0; i < (int) nbags; i++)
      {
	// alloacte a new bag item in the collection:
	bags.push_back (bag ());

	// load it:
	ar & boost::serialization::make_nvp ("Bag", bags.back ());
      }

    uint32_t sum_ok = 0;
    for (list<bag>::const_iterator i = bags.begin (); 
	 i != bags.end (); 
	 i++)
      {
	if (i->check_sum ())
	  {
	    sum_ok++;
	  }
	else 
	  {
	    cerr << "Ooops! Sum is bad!" << endl;	    
	  }
	
      }
    if (sum_ok == nbags) clog << "All sums are ok!" << endl;
    else cerr << "Some sums are wrong!" << endl;
    
  }

  return 0;
}
