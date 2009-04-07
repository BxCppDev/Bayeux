#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/list.hpp>

#include "datum.hpp"

using namespace std;

int main (int argc_, char ** argv_)
{
  string fname = "prg_mt1.txt";
  int arg_ncol = 10;

  if (argc_ > 1) arg_ncol = atoi (argv_[1]);

  {
    clog << "Storing data..." << endl;
    ofstream f (fname.c_str ());
    
    boost::archive::text_oarchive ar (f);
    
    // a collection with items that must be serialized:
    list<datum *> col;
    
    // generate the 'datum' collection:
    uint32_t ncol = arg_ncol;
    for (int i = 0; i < (int) ncol; i++)
      {
	// allocate a new 'datum' item:
	datum * p = new datum (10 * (i + 1));

	// append the new item to the list of 'datum':
	col.push_back (p);
      }
    // add a last item which is a reference to the first one:
    col.push_back (col.front ());
    
    // store the number of col:
    ncol = col.size ();
    ar & ncol;

    // traverse the data collection and store each item:
    uint32_t sum = 0;
    for (list<datum *>::const_iterator i = col.begin (); 
	 i != col.end (); 
	 i++)
      {
	clog << "-- datum value is:  " << (*i)->get_value () << endl;

	// compute the cumulative sum from all items:
	sum += (*i)->get_value ();

	// store the current item:
	ar & *i;
      }
    clog << "-- computed sum is: " << sum << endl;
    clog << "-- expected sum is: " << 10 * (ncol - 1) * (ncol) / 2 + 10 << endl;

    // free memory:
    size_t count = 0;
    for (list<datum *>::const_iterator i = col.begin (); 
	 i != col.end (); 
	 i++)
      {
	count++;
	if (count == col.size()) break;
	delete *i;
      }    
    col.clear ();

    clog << "-- output archive file is: '" << fname << "'" << endl;
  }

  {
    clog << "Loading data..." << endl;
    ifstream f (fname.c_str ());

    boost::archive::text_iarchive ar (f);
    clog << "-- input archive file is: '" << fname << "'" << endl;

    list<datum *> col;

    uint32_t ncol = 0;

    // load the number of stored col:
    ar & ncol;
    clog << "-- number of item is: '" << ncol << "'" << endl;

    // allocate the collection:
    for (int i = 0; i < (int) ncol; i++)
      {
	// allocate a new default 'datum' item in the collection:
	datum * p = new datum;

	// append the new item to the list of 'datum':
	col.push_back (p);
      }

    // traverse the data collection and store each item:
    uint32_t sum = 0;
    for (list<datum *>::iterator i = col.begin (); 
	 i != col.end (); 
	 i++)
      {
	// store the current item:
	ar & *i;
	clog << "-- datum value is:  " << (*i)->get_value () << endl;
	sum += (*i)->get_value ();
      }
    clog << "-- computed sum is: " << sum << endl;
    clog << "-- expected sum is: " << 10 * (ncol - 1) * (ncol) / 2 + 10 << endl;
    
    // free memory:
    size_t count = 0;
    for (list<datum *>::const_iterator i = col.begin (); 
	 i != col.end (); 
	 i++)
      {
	count++;
	if (count == col.size()) break;
	delete *i;
      }    
    col.clear ();
  }

  return 0;
}
