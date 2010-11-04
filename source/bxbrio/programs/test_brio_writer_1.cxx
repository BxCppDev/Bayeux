#include <brio/writer.h>
#include <brio/test/data.h> /* a serializable sample class */

int main (void)
{ 
  brio::writer my_writer ("file.root");
  
  for (int i = 0; i < 10; i++) 
    {
      brio::test::data a_data;
      my_writer.store (a_data);
    }

  my_writer.close (); // not mandatory (automatic at destruction)

  return 0;
}
