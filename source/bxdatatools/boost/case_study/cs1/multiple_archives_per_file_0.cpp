// multiple_archives_per_file_0.cpp

#include <fstream>
#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "foo.hpp"

using namespace std;

int main (void)
{
  clog << "Hello!" << endl << endl;
    
  string fname = "data_0.txt";
  size_t ndata = 5;
  {
    clog << "Store data in ASCII file '" << fname << "': " << endl;
    clog << "-- open the output file '" << fname << "'..." << endl;
    ofstream fout (fname.c_str ());
    
    clog << "-- number of data to be stored: " << ndata << endl;
    for (size_t i = 0; i < ndata; i++)
      {
	// setup the data:
	clog << "-- initialize a 'foo' data..." << endl;
	foo the_foo;
	the_foo.set_i (i);
	the_foo.set_x (i * 3.14);
	ostringstream name_ss;
	name_ss << "name_" << i;
	the_foo.set_name (name_ss.str ());
	clog << "-- data is: " << the_foo << endl;

	{
	  // setup the ASCII output archive:
	  boost::archive::text_oarchive toa (fout);

	  // store the 'foo' data:
	  clog << "-- store the 'foo' data..." << endl;
	  toa & the_foo;
	}
      }
    clog << endl;
  }
  
  {
    clog << "Load data from ASCII file '" << fname << "': " << endl;
    clog << "-- open the input file '" << fname << "'..." << endl;
    ifstream fin (fname.c_str ());
    size_t ndata = 0;
    while (fin && ! fin.eof ())
      {
	clog << "-- default 'foo' data..." << endl;
	foo the_foo;

	{
	  // setup the ASCII input archive:
	  boost::archive::text_iarchive tia (fin);
	  
	  // load the 'foo' data:
	  clog << "-- load the 'foo' data..." << endl;
	  tia & the_foo;

	  ndata++;
	}

	clog << "-- data is: " << the_foo << endl;

	clog << "-- eat trailing 'space' chars from the input stream..." << endl;
	fin >> ws;
      }
    clog << "-- number of loaded data: " << ndata << endl;
    clog << endl;
  }
  clog << "Bye!" << endl;
  
  return 0;
}

// end of multiple_archives_per_file_0.cpp
