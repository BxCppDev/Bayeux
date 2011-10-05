/** demo_pba_0.cpp
 *
 * (C) Copyright 2011 François Mauger, Christian Pfligersdorffer
 *
 * Use, modification and distribution is subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 */

/**
 * The intent of this program is to serve as a tutorial for
 * users of the portable binary archive in the framework of 
 * the Boost/Serialization package. 
 *
 * A quick start example that shows how to store and load some 
 * basic types (bool, integer and floating numbers) in a portable 
 * binary archive format associated to a file stream.
 * Not many explanations are given.
 *
 */

#include <string>
#include <fstream>

#include <boost/cstdint.hpp>
#include <boost/archive/portable_binary_oarchive.hpp>
#include <boost/archive/portable_binary_iarchive.hpp>

int main (void)
{
  using namespace std;

  
  // The name for the example data file :  
  string filename = "demo_pba_0.data"; 

  // Store:
  {
    // Some variables of various types :
    bool     b       = true;
    char     c       = 'B';
    uint32_t answer  = 42;
    float    computing_time    = 7.5e6;
    double   e       = 2.71828182845905;
    string   slogan  = "DON'T PANIC";

    // Open an output binary file stream :
    ofstream fout (filename.c_str (), ios_base::binary);
    
    // An output portable binary archive attached to the output file :
    boost::archive::portable_binary_oarchive opba (fout);
    
    // Storing (serializing) variables...
    opba & b & c & answer & computing_time & e & slogan;

    cout << "\tData have been stored in '" << filename  << "'." << endl;
  }

  // Load:
  {
    // Some variables of various types :
    bool     b;
    char     c;
    uint32_t answer;
    float    computing_time;
    double   e;
    string   slogan;
   
    // Open an input binary file stream :
    ifstream fin (filename.c_str (), ios_base::binary);
    
    // An input portable binary archive attached to the input file :
    boost::archive::portable_binary_iarchive ipba (fin);
    
    // Storing (serializing) values...
    ipba & b & c & answer & computing_time & e & slogan;

    cout << "\tData have been loaded from '" << filename  << "'." << endl;
  }

  return 0;   
}

// end of demo_pba_0.cpp
