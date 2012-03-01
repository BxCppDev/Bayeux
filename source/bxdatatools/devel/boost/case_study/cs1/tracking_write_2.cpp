// tracking_write_2.cpp 
// (C) Copyright 2009 François Mauger <mauger@lpccaen.in2p3.fr>
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <string>
#include <list>

#include <boost/cstdint.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "foo.hpp"

using namespace std;

int main (void)
{
  string fname = "data_2.xml";
  // Store data in XML archive output file:
  ofstream fout (fname.c_str ());

  // number of 'foo' data to be stored:
  uint32_t ndata = 5;
  { 
    // setup the output XML archive: 
    boost::archive::xml_oarchive xoa (fout);

    // declare a container for 'foo' data:
    list <foo> foos;

    // store the number of stored objects:
    xoa & boost::serialization::make_nvp ("ndata", ndata); 
    for (size_t i = 0; i < ndata; i++)
      {
	// add a new 'foo' in the 'foos' list:
	foos.push_back (foo ()); // invoking 'foo' default constructor

	// get a reference to the new 'foo':
	foo & the_foo = foos.back ();

	// initialize this 'foo' data with some 'fuus' in it:
	the_foo.init_fuus (i + 1);

	// print current 'foo':
	clog << endl << "foo is: " << the_foo << endl << endl;
	
	// store the 'foo' data:
	xoa & boost::serialization::make_nvp ("foo", the_foo); 

	// do not clear the 'fuu' instances within 'foo'
	// in order to freeze the memory: no memory reallocation is possible
	// for the next instances.

 	clog << endl;
     }

    // Archive is done, remove all 'foo' instances from the 'foos' container:
    foos.clear ();

    clog << endl;
  }

  // print memory statistics:
  foo::dynamic_memory_stat ();

  clog << "XML archive file is: '" << fname << "'" << endl;
  
  return 0;
}

// end of tracking_write_2.cpp
