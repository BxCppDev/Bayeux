#include <cstdlib>
#include <exception>
#include <iostream>

#include <brio/writer.h>
#include <brio/reader.h>

#include <foo.h>  // a serializable sample class
#include <foo.ipp> // Serialization code

int main(void)
{
  try {

    {
      std::clog << "Serializing objects...\n";
      long seed = 12345;
      srand48(seed);
      // Store foo objects in a BRIO file (using the default automatic store) :
      brio::writer my_writer("ex01_data.brio");
      for (int i = 0; i < 10; i++) {
        foo obj;
        obj.randomize();
        {
          std::ostringstream title;
          title << "Foo #" << i << ": ";
          obj.dump(std::clog, title.str());
        }
        my_writer.store(obj);
      }
      my_writer.close(); // not mandatory (automatic at destruction)
    }

    {
      std::clog << "\nDeserializing objects...\n";
      /* Load foo objects sequentially from a BRIO file
       * (using the default automatic store) :
       */
      brio::reader my_reader("ex01_data.brio");
      std::clog << "Default store has " << my_reader.get_number_of_entries()
                << " records." << std::endl;
      int counter =  0;
      while (my_reader.has_next()) {
        foo obj;
        my_reader.load_next(obj);
        // or : my_reader.load(obj);
        /* default 'load' behaviour
         * traverses the store sequentially
         */
        std::ostringstream title;
        title << "Foo #" << counter << ": ";
        obj.dump(std::clog, title.str());
        counter++;
      }
      std::clog << counter << " objects have been loaded.\n";
      // Rewind the current (default) store :
      my_reader.rewind_store();
      // Explicitly load the 4th foo entry from the default automatic store :
      foo obj4;
      my_reader.load(obj4, 4);
      {
        std::ostringstream title;
        title << "Foo #" << 4 << ": ";
        obj4.dump(std::clog, title.str());
      }
      my_reader.close(); // not mandatory (automatic at destruction)
    }
  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
  }
  catch(...) {
    std::cerr << "error: " << "Unexpected error !" << std::endl;
  }
  return 0;
}
