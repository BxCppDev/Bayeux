#include <datatools/serialization/io_factory.h>

#include "sdata2.h"
#include "sdata2.ipp"

int main (void) 
{
  // Shortcut namespace :
  namespace ds = datatools::serialization;

  // Serialize some 'sdata' instances : 
  {
    sdata d1, d2, d3;

    ds::data_writer the_writer ("data_4.txt", ds::using_multi_archives);

    the_writer.store (d1); // No need to pass 
    the_writer.store (d2); // a serialization tag
    the_writer.store (d3); // to the 'store' method.
  }

  // Deserialize some 'sdata' instances : 
  {
    sdata d1, d2, d3;

    ds::data_reader the_reader ("data_4.txt", ds::using_multi_archives);
    
    the_reader.load (d1); // No need to pass
    the_reader.load (d2); // a serialization tag
    the_reader.load (d3); // to the 'load' method.
  }
  return 0;
}
