#include <iostream>

#include <brio/test/data.h> /* a serializable sample class */
/* high-level serialization code from the 'brio' library 
 * - test classes (brio::test::data)
 */
#include <brio/test/the_serializable.h>
#include <brio/reader.h>

int main (void)
{
  using namespace std; 

  brio::reader my_reader ("file.brio");  
  clog << "Default store has " << my_reader.get_number_of_entries () << " records." << endl;

  while (my_reader.has_next ())  
    {
       brio::test::data a_data;
       my_reader.load_next (a_data);
       // or : my_reader.load (a_data); 
       /* default 'load' behaviour 
	* traverses the store sequentially
	*/
    }
  my_reader.tree_dump (clog, "Reader : ");
  my_reader.rewind_store ();
  brio::test::data a_data;
  my_reader.load (a_data, 4);
  a_data.dump (clog);
  my_reader.close (); // not mandatory (automatic at destruction)
 
  return 0; 
}
