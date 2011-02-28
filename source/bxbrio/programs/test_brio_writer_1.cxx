#include <cstdlib>
#include <iostream>
#include <brio/writer.h>
#include <brio/test/data.h> /* a serializable sample class */

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
      a_data.dump (clog);
      my_writer.store (a_data);
    }

  my_writer.close (); // not mandatory (automatic at destruction)

  return 0;
}
