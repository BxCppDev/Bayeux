// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/properties.h>
// - Bayeux/brio:
#include <brio/writer.h>
#include <brio/reader.h>

// This example:
#include <foo.h>  // a serializable sample class
#include <foo.ipp> // Serialization code

int main(int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try {

    if (argc_>1) {
      std::string opt = argv_[1];
      if (opt == "--debug") logging = datatools::logger::PRIO_DEBUG;
      else if (opt == "--warning") logging = datatools::logger::PRIO_WARNING;
    }

    {
      DT_LOG_NOTICE(logging,"Serializing objects...");
      long seed = 12345;
      srand48(seed);
      // Store foo objects in a BRIO file (using the default store) :
      brio::writer my_writer("ex02_data.brio", logging);
      my_writer.add_store("header"); // create a store named 'header'
      my_writer.add_store("data");   // create a store named 'data'
      my_writer.lock(); /* no more stores can be added,
                         * even the *automatic* store
                         */
      my_writer.select_store("header");
      datatools::properties my_setup;
      my_setup.store_flag("example");
      my_setup.store("author", "King Arthur");
      my_setup.store("year", 543);
      my_setup.store("coconut.version", "1.4(afrika)");
      my_setup.store("favorite.color", "red");
      my_setup.tree_dump(std::clog, "Setup: ");
      my_writer.store(my_setup); /* store a 'datatools::properties' object in
                                  * the active store
                                  */
      my_writer.select_store("data"); // Select the store named 'data'
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
      DT_LOG_NOTICE(logging,"Deserializing objects...");
      /* Load foo objects sequentially from a BRIO file
       * (using the default store) :
       */
      brio::reader my_reader("ex02_data.brio", logging);
      DT_THROW_IF(! my_reader.has_store("header"),
                  std::logic_error,
                  "Cannot find the 'header' store from the brio file !");
      // Make the store named 'header' active for reading data :
      my_reader.select_store("header");
      datatools::properties my_setup;
      my_reader.load(my_setup); /* load a 'datatools::properties' object
                                 * from the active store
                                 */
      my_setup.tree_dump(std::clog, "Setup: ");
      // Check the existence of a store named 'data' :
      DT_THROW_IF(! my_reader.has_store("data"),
                  std::logic_error,
                  "Cannot find the 'data' store from the brio file !");
      // Make the store named 'data' active for reading data :
      my_reader.select_store("data");
      int counter =  0;
      while (my_reader.has_next()) {
        foo obj;
        my_reader.load_next(obj);
        std::ostringstream title;
        title << "Foo #" << counter << ": ";
        obj.dump(std::clog, title.str());
        counter++;
      }
      DT_LOG_NOTICE(logging, counter << " objects have been loaded.");
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
  } catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,x.what());
  } catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,"Unexpected error !");
  }
  return 0;
}
