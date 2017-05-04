#include <cstdlib>
#include <iostream>

#include <datatools/properties.h>
#include <brio_test_data.cc>

// Serialization code :
#include <datatools/properties.ipp>
#include <brio_test_data.ipp>

#include <brio/writer.h>

int main (void)
{ 
  using namespace std;

  long seed = 12345;
  srand48 (seed);

  brio::writer my_writer ("file2.brio");
  my_writer.add_store ("header"); // create a store named 'header'
  my_writer.add_store ("data");   // create a store named 'data'
  my_writer.lock (); // no more stores can be added (even the *automatic* store)

  my_writer.select_store ("header"); // make the store named 'header' active
  datatools::properties the_setup;
  the_setup.store_flag ("test");
  the_setup.store ("author", "King Arthur");
  the_setup.store ("year", 543);
  the_setup.store ("coconut_version", "1.4(afrika)");
  the_setup.dump ();
  my_writer.store (the_setup); // store a 'properties' object in the active store

  my_writer.select_store ("data"); // make the store named 'data' active
  for (int i = 0; i < 10; i++) 
    {
      brio::test::data_t a_data;
      a_data.randomize ();
      a_data.dump (clog);
      my_writer.store (a_data); // store a 'data' object in the active store
    }

  my_writer.close (); // not mandatory (automatic at destruction)

  return 0;
}
