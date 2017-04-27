// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
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
      // Store foo objects in a BRIO file (using the default automatic store) :
      brio::writer my_writer("ex01_data.brio", logging);
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
       * (using the default automatic store) :
       */
      brio::reader my_reader("ex01_data.brio", logging);
      DT_LOG_NOTICE(logging, "Default store has " << my_reader.get_number_of_entries() << " records.");
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
      DT_LOG_NOTICE(logging, counter << " objects have been loaded.");
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
  } catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,x.what());
  } catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,"Unexpected error !");
  }
  return 0;
}
