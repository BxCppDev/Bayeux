#include <brio/writer.h>
#include <brio/test/data.h> /* a serializable sample class */
#include <datatools/utils/properties.h> /* the (serializable) properties class from the datatools package */

int main (void)
{ 
  brio::writer my_writer ("file.root");
  my_writer.add_store ("header");
  my_writer.add_store ("data");
  my_writer.lock (); // no more stores can be added (even the *automatic* store)

  my_writer.select_store ("header");
  datatools::utils::properties the_setup;
  the_setup.store_flag ("test");
  the_setup.store ("author", "King Arthur");
  the_setup.store ("year", 543);
  the_setup.store ("coconut_version", "1.4(afrika)");
  my_writer.store (the_setup);

  my_writer.select_store ("data");
  for (int i = 0; i < 10; i++) 
    {
      brio::test::data a_data;
      my_writer.store (a_data);
    }

  my_writer.close (); // not mandatory (automatic at destruction)

  return 0;
}
