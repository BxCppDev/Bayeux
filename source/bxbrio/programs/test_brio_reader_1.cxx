#include <brio/reader.h>
#include <brio/test/data.h> /* a serializable sample class */

int main (void)
{ 
  brio::reader my_reader ("file.root");
  
  while (my_reader.has_next ())  
    {
       brio::test::data a_data;
       my_reader.load_next (a_data);
    }

  my_reader.close (); // not mandatory (automatic at destruction)

  return 0;
}
