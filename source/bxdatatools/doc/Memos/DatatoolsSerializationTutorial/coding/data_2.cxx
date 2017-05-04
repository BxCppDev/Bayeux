// Standard C++ file streams :
#include <fstream>

// The xml (ASCII) archives from Boost :
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/nvp.hpp>

// The user's class :
#include "data.h"

// Explicit include of the Boost serialization code for this class :
#include "data.ipp"

int main (void) 
{
  /** Serialize a single instance of class 'data' in a 
   *  XML (ASCII) output archive associated to an output file
   */
  {
    // Declare an output file stream :
    std::ofstream output_stream ("stored_data.xml");

    // Attach a Boost xml output archive to the file stream : 
    boost::archive::xml_oarchive oa (output_stream);
    
    // Instantiate a 'data' object :
    data my_data;

    // Serialize it (store it in the archive) :  
    oa << boost::serialization::make_nvp ("my_data", my_data);

    // Flush the output file stream for safety :
    output_stream.flush ();
  }
  /** Deserialize a single instance of class 'data' in a 
   *  XML (ASCII) input archive associated to an input file
   */
  {
    // Declare an input file stream :
    std::ifstream input_stream ("stored_data.xml");

    // Attach a Boost xml input archive to the file stream : 
    boost::archive::xml_iarchive ia (input_stream);
    
    // Instantiate a 'data' object :
    data my_data;

    // Deserialize it (load from the archive) :  
    ia >> boost::serialization::make_nvp ("my_data", my_data);
  }
  return 0;
}
