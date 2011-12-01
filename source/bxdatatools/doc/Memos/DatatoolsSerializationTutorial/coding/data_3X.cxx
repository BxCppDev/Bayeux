#include <datatools/serialization/io_factory.h> // Reader/writer classes.
#include "data.h"                         // Interface for the 'data' class
#include "data.ipp"                       // ... and associated serialization code
#include <datatools/utils/properties.h>   // Interface for the 'properties' class
#include <datatools/utils/properties.ipp> // ... and associated serialization code.

int main (void) 
{
  namespace ds = datatools::serialization;
  namespace du = datatools::utils;
  { // Serialize some objects : 
    data d1, d2, d3;
    du::properties p1, p2;

    ds::data_writer the_writer ("data_3.xml", ds::using_multi_archives);
    // Serialize several objects of different types in arbitrary order :  
    the_writer.store ("data", d1);       // The choice of the 'data' and 'properties'
    the_writer.store ("properties", p1); // records is arbitrary. However, it defines
    the_writer.store ("data", d2);       // a convention that must be shared
    the_writer.store ("properties", p2); // at the store and load steps.
    the_writer.store ("data", d3);
  }

  { // Deserialize some objects :
    ds::data_reader the_reader ("data_3.xml", ds::using_multi_archives);

    // Deserialization loop :
    while (the_reader.has_record_tag ()) 
      {
	if (the_reader.record_tag_is ("data"))
	  { // Deserialization driver for class 'data" :
	    std::cout << "Load an object with record='data'." << std::endl;
	    data d; 
	    the_reader.load ("data", d);
	  } 
	else if (the_reader.record_tag_is ("properties"))
	  { // Deserialization driver for class 'datatools::utils::properties" :
	    std::cout << "Load an object with record='datatools::utils::properties'."
		      << std::endl;
	    datatools::utils::properties p; 
	    the_reader.load ("properties", p);
	  } 
	else
	  { // Error handling for unknown record tags :
	    std::cerr << "Error: Unknown record tag '" << the_reader.get_record_tag ()
		      << "' !" << std::endl;
	    break;
	  }
      }
    std::cerr << "Warning: Not more object to be deserialized !" << std::endl;
   }
  return 0;
}
