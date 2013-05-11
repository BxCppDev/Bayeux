#include <cstdlib>
#include <exception>
#include <iostream>

#include <brio/writer.h>
#include <brio/reader.h>

// Serializable classes :
#include <datatools/properties.h>
#include <foo.h>

// Serialization code :
#include <datatools/properties.ipp>
#include <foo.ipp>

int main(void)
{
  try {
    {
      std::clog << "Serializing objects...\n";
      long seed = 12345;
      srand48(seed);
      // Store foo objects in a BRIO file (using the default store) :
      brio::writer my_writer("ex02_data.brio");
      my_writer.add_store ("header"); // create a store named 'header'
      my_writer.add_store ("data");   // create a store named 'data'
      my_writer.lock (); /* no more stores can be added,
                          * even the *automatic* store
                          */
      my_writer.select_store ("header");
      datatools::properties my_setup;
      my_setup.store_flag ("example");
      my_setup.store ("author", "King Arthur");
      my_setup.store ("year", 543);
      my_setup.store ("coconut.version", "1.4(afrika)");
      my_setup.store ("favorite.color", "red");
      my_setup.tree_dump (std::clog, "Setup: ");
      my_writer.store (my_setup); /* store a 'datatools::properties' object in
                                   * the active store
                                   */
      my_writer.select_store ("data"); // make the store named 'data' active
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
       * (using the default store) :
       */
      brio::reader my_reader("ex02_data.brio");
      if (! my_reader.has_store ("header")) {
        std::ostringstream message;
        message << "Cannot find the 'header' store from the brio file !";
        throw std::logic_error(message.str());
      }
      // Make the store named 'header' active for reading data :
      my_reader.select_store ("header");
      datatools::properties my_setup;
      my_reader.load (my_setup); /* load a 'datatools::properties' object
                                  * from the active store
                                  */
      my_setup.tree_dump (std::clog, "Setup: ");
      // Check the existence of a store named 'data' :
      if (! my_reader.has_store ("data")) {
        std::ostringstream message;
        message << "Cannot find the 'data' store from the brio file !";
        throw std::logic_error(message.str());
      }
      // Make the store named 'data' active for reading data :
      my_reader.select_store ("data");
      int counter =  0;
      while (my_reader.has_next()) {
        foo obj;
        my_reader.load_next(obj);
        std::ostringstream title;
        title << "Foo #" << counter << ": ";
        obj.dump(std::clog, title.str());
        counter++;
      }
      std::clog << counter << " objects have been loaded.\n";
      // Rewind the current (default) store :
      my_reader.rewind_store();
      // Explicitly load the 4th foo entry from the default store :
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
