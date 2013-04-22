// The datatools reader/writer classes :
#include <datatools/io_factory.h>

// The user's class :
#include "data.h"
#include "data.ipp"

int main (void)
{
  // Serialize some 'data' instances :
  {
    // Instantiate some 'data' objects :
    data d1, d2, d3;

    // Declare a writer :
    datatools::data_writer the_writer ("data_3.txt", datatools::using_multi_archives);

    // Serialize it (store them through the writer) :
    the_writer.store ("data", d1);
    the_writer.store ("data", d2);
    the_writer.store ("data", d3);
  }

  // Deserialize some 'data' instances :
  {
    // Instantiate some 'data' objects :
    data d1, d2, d3;

    // Declare a reader :
    datatools::data_reader the_reader ("data_3.txt", datatools::using_multi_archives);

    // Deserialize them (load them through the reader) :
    the_reader.load ("data", d1);
    the_reader.load ("data", d2);
    the_reader.load ("data", d3);
  }
  return 0;
}
