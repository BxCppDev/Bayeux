/* prg1.cxx */

// Standard C++ file streams:
#include <fstream>

// The text (ASCII) archives from Boost:
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// The user's class:
#include "data.h"

using namespace std;

int main (int argc_, char ** argv_) 
{
  /** Serialize a single instance of class 'data' in a 
   *  text (ASCII) archive associated to an output file
   */
  {
    ofstream output_stream ("stored_data.txt");
    boost::archive::text_oarchive oa (output_stream);
    
    data my_data;
    oa << my_data;
    output_stream.flush ();
  }

  /** Deserialize a single instance of class 'data' in a 
   *  text (ASCII) archive associated to an output file
   */
  {
    ifstream input_stream ("stored_data.txt");
    boost::archive::text_iarchive ia (input_stream);
    
    data my_data;
    ia >> my_data;
  }

  return 0;
}

/* end of prg1.cxx */
