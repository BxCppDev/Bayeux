// tracking_read_2.cpp 
// (C) Copyright 2009 François Mauger <mauger@lpccaen.in2p3.fr>
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
 
#include <fstream>
#include <string>
#include <list>

#include <boost/cstdint.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "foo.hpp"

using namespace std;

int main (int argc_, char ** argv_)
{
  string fname = "data_2.xml";
  clog << "XML archive file is: '" << fname << "'" << endl;
  // Load data from XML archive output file:
  ifstream fin (fname.c_str ());
    
  {
    // setup the input XML archive:
    boost::archive::xml_iarchive xia (fin);
 
    // declare a container for 'foo' data:
    list <foo> foos;

    // load the number of stored objects:
    uint32_t ndata = 0;
    xia & boost::serialization::make_nvp ("ndata", ndata);
    clog << "Number of loaded data: " << ndata << endl;

     for (int i = 0; i < (int) ndata; i++)
      {
	// add a new 'foo' in the 'foos' list:
	foos.push_back (foo ()); // invoking 'foo' default constructor

	// get a reference to the new 'foo':
	foo & the_foo = foos.back ();

	// load the 'foo' data:
	xia & boost::serialization::make_nvp ("foo", the_foo);
	
	// print the last deserialized 'foo':
	clog << endl << "foo is: " << the_foo << endl << endl;
      }
    clog << endl;
  }
  
  return 0;
}

// end of tracking_read_2.cpp
