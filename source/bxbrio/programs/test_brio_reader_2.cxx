#include <iostream>

#include <datatools/utils/properties.h>
#include <brio/test/data.h>

// Serialization code :
#include <datatools/utils/properties.ipp>
#include <brio/test/data.ipp>

#include <brio/reader.h>

int main (void)
{
  using namespace std;

  brio::reader my_reader ("file2.brio");

  // check the existence of a store named 'header' :
  if (! my_reader.has_store ("header"))
    {
      cerr << "ERROR: Cannot find the 'header' store from the brio file ! Abort !" << endl;
      return 1;
    }
  // make the store named 'header' active for reading data :
  my_reader.select_store ("header");
  clog << "Header store has " << my_reader.get_number_of_entries () << " record(s)." << endl;
  datatools::utils::properties the_setup;
  my_reader.load (the_setup); // load a 'properties' object from the active store
  the_setup.dump ();

  // check the existence of a store named 'data' :
  if (! my_reader.has_store ("data"))
    {
      cerr << "ERROR: Cannot find the 'data' store from the brio file ! Abort !" << endl;
      return 1;
    }
  // make the store named 'data' active for reading data :
  my_reader.select_store ("data");
  clog << "Data store has " << my_reader.get_number_of_entries () << " record(s)." << endl;
  while (my_reader.has_next ())
    {
       brio::test::data a_data;
       my_reader.load_next (a_data); // load another 'data' object from the active store
       // or : my_reader.load (a_data);
       /* default 'load' behaviour
	* traverses the store sequentially
	*/
    }
  my_reader.rewind_store (); // rewind the store at beginning
  brio::test::data a_data;
  my_reader.load (a_data, 4); // load only entry #4 from the active store
  a_data.dump (clog);

  my_reader.close (); // not mandatory (automatic at destruction)

  return 0;
}
