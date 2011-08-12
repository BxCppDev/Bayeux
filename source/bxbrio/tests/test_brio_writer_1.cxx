#include <cstdlib>
#include <iostream>

#include <brio_test_data.cc> /* a serializable sample class */

// Serialization code :
#include <brio_test_data.ipp> 

#include <brio/writer.h>

int main (void)
{ 
  using namespace std;

  long seed = 12345;
  srand48 (seed);

  brio::writer my_writer ("file.brio");
  
  for (int i = 0; i < 10; i++) 
    {
      brio::test::data a_data;
      a_data.randomize ();
      {
	ostringstream title;
	title << "Data #" << i << ": ";
	a_data.dump (clog, title.str ());
      }
      my_writer.store (a_data);
    }
  my_writer.close (); // not mandatory (automatic at destruction)

  return 0;
}
