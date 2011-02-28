#include <iostream>

#include <brio/reader.h>
#include <brio/test/data.h> /* a serializable sample class */
#include <datatools/utils/properties.h> /* the (serializable) properties class from the datatools package */

int main (void)
{
  using namespace std; 

  brio::reader my_reader ("file2.brio");  
  if (! my_reader.has_store ("header"))
    {
      cerr << "ERROR: Cannot find the 'header' store from the brio file ! Abort !" << endl;
      return 1;
    }
  my_reader.select_store ("header");
  clog << "Header store has " << my_reader.get_number_of_entries () << " record(s)." << endl;
  datatools::utils::properties the_setup;
  my_reader.load (the_setup);
  the_setup.dump ();
  
  if (! my_reader.has_store ("data"))
    {
      cerr << "ERROR: Cannot find the 'data' store from the brio file ! Abort !" << endl;
      return 1;
    }
  my_reader.select_store ("data");
  clog << "Data store has " << my_reader.get_number_of_entries () << " record(s)." << endl;
  while (my_reader.has_next ())  
    {
       brio::test::data a_data;
       my_reader.load_next (a_data);
       // or : my_reader.load (a_data); 
       /* default 'load' behaviour 
	* traverses the store sequentially
	*/
    }
  my_reader.rewind_store ();
  brio::test::data a_data;
  my_reader.load (a_data, 4);
  a_data.dump (clog);
  my_reader.close (); // not mandatory (automatic at destruction)
 
  return 0; 
}
