/** tutorial_pba_2.cpp
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
 * the Boost/Serialization library. 
 *
 * This sample program shows how to use a portable binary archive 
 * without a header.
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
  std::string filename = "pba_1.data"; 

  {
    // Some variable to be stored :
    uint32_t answer = 666;
    
    // Open an output file stream in binary mode :
    std::ofstream fout (filename.c_str (), ios_base::binary);
    
    // Create an output portable binary archive attached to the output file :
    boost::archive::portable_binary_oarchive opba (fout, boost::archive::no_header);
    
    // Store (serializing) variables :
    opba & answer;
  }

  { 
    // Some variable to be loaded :
    uint32_t answer;

    // Open an input file stream in binary mode :
    ifstream fin (filename.c_str (), ios_base::binary);
  
    // Create an input portable binary archive attached to the input file :
    boost::archive::portable_binary_iarchive ipba (fin, boost::archive::no_header);
  
    // Loading (de-serializing) variables using the same 
    // order than for serialization :
    ipba & answer;
  }

  return 0;   
}

// end of tutorial_pba_2.cpp
