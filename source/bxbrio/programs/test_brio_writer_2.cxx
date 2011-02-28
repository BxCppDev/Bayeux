#include <cstdlib>
#include <iostream>
#include <brio/writer.h>
#include <brio/test/data.h> /* a serializable sample class */
#include <datatools/utils/properties.h> /* the (serializable) properties class from the datatools package */

int main (void)
{ 
  using namespace std;

  long seed = 12345;
  srand48 (seed);

  brio::writer my_writer ("file2.brio");
  my_writer.add_store ("header");
  my_writer.add_store ("data");
  my_writer.lock (); // no more stores can be added (even the *automatic* store)

  my_writer.select_store ("header");
  datatools::utils::properties the_setup;
  the_setup.store_flag ("test");
  the_setup.store ("author", "King Arthur");
  the_setup.store ("year", 543);
  the_setup.store ("coconut_version", "1.4(afrika)");
  the_setup.dump ();
  my_writer.store (the_setup);

  my_writer.select_store ("data");
  for (int i = 0; i < 10; i++) 
    {
      brio::test::data a_data;
      a_data.randomize ();
      a_data.dump (clog);
      my_writer.store (a_data);
    }

  my_writer.close (); // not mandatory (automatic at destruction)

  return 0;
}
