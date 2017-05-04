// Standard C++ file streams :
#include <fstream>

// The text (ASCII) archives from Boost :
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// The user's class :
#include "data.h"

// Explicit include of the Boost serialization code for this class :
#include "data.ipp"

int main (void) 
{
  /** Serialize a single instance of class 'data' in a 
   *  text (ASCII) output archive associated to an output file
   */
  {
    // Declare an output file stream :
    std::ofstream output_stream ("stored_data.txt");

    // Attach a Boost text output archive to the file stream : 
    boost::archive::text_oarchive oa (output_stream);
    
    // Instantiate a 'data' object :
    data my_data;

    // Serialize it (store it in the archive) :  
    oa << my_data;

    // Flush the output file stream for safety :
    output_stream.flush ();
  }
  /** Deserialize a single instance of class 'data' in a 
   *  text (ASCII) input archive associated to an input file
   */
  {
    // Declare an input file stream :
    std::ifstream input_stream ("stored_data.txt");

    // Attach a Boost text input archive to the file stream : 
    boost::archive::text_iarchive ia (input_stream);
    
    // Instantiate a 'data' object :
    data my_data;

    // Deserialize it (load from the archive) :  
    ia >> my_data;
  }
  return 0;
}
