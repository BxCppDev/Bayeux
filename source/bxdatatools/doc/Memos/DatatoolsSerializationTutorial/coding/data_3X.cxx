#include <datatools/io_factory.h>   // Reader/writer classes.
#include "data.h"                   // Interface for the 'data' class
#include "data.ipp"                 // ... and associated serialization code
#include <datatools/properties.h>   // Interface for the 'properties' class
#include <datatools/properties.ipp> // ... and associated serialization code.

int main (void)
{
  { // Serialize some objects :
    data d1, d2, d3;
    datatools::properties p1, p2;

    datatools::data_writer the_writer ("data_3.xml", datatools::using_multi_archives);
    // Serialize several objects of different types in arbitrary order :
    the_writer.store ("data", d1);       // The choice of the 'data' and 'properties'
    the_writer.store ("properties", p1); // records is arbitrary. However, it defines
    the_writer.store ("data", d2);       // a convention that must be shared
    the_writer.store ("properties", p2); // at the store and load steps.
    the_writer.store ("data", d3);
  }

  { // Deserialize some objects :
    datatools::data_reader the_reader ("data_3.xml", datatools::using_multi_archives);

    // Deserialization loop :
    while (the_reader.has_record_tag ()) {
      if (the_reader.record_tag_is ("data")) {
        // Deserialization driver for class 'data" :
        std::clog << "Load an object with record='data'." << std::endl;
        data d;
        the_reader.load ("data", d);
      } else if (the_reader.record_tag_is ("properties")) {
        // Deserialization driver for class 'datatools::properties" :
        std::clog << "Load an object with record='datatools::properties'."
                  << std::endl;
        datatools::properties p;
        the_reader.load ("properties", p);
      } else {
        // Error handling for unknown record tags :
        std::cerr << "Error: Unknown record tag '" << the_reader.get_record_tag ()
                  << "' !" << std::endl;
        break;
      }
    }
    std::clog << "Warning: Not more object to be deserialized !" << std::endl;
  }
  return 0;
}
