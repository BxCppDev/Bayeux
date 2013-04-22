#include <datatools/io_factory.h>

#include "sdata.h"
#include "sdata.ipp"

int main (void)
{
  // Serialize some 'sdata' instances :
  {
    sdata d1, d2, d3;

    datatools::data_writer the_writer ("data_4.txt", datatools::using_multi_archives);

    the_writer.store (d1); // No need to pass
    the_writer.store (d2); // a serialization tag
    the_writer.store (d3); // to the 'store' method.
  }

  // Deserialize some 'sdata' instances :
  {
    sdata d1, d2, d3;

    datatools::data_reader the_reader ("data_4.txt", datatools::using_multi_archives);

    the_reader.load (d1); // No need to pass
    the_reader.load (d2); // a serialization tag
    the_reader.load (d3); // to the 'load' method.
  }
  return 0;
}
