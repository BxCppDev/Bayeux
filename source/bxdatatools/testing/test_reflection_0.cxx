/* test_reflection_0.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <camp/invalidenum.hpp>

#include <datatools/logger.h>

// Introspectable classes :
#include <datatools/logger.h>
#include <datatools/event_id.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/things.h>

// Some pre-processor guard about CAMP reflection usage and linkage :
#include <datatools/reflection_guard.h>

void test_things (bool debug_)
{
  std::clog << "*** test_things : " << std::endl;

  // Access meta class for class 'things' :
  const DR_CLASS & tMetaClass = DR_CLASS_BY_NAME("datatools::things");
  {
    boost::scoped_ptr<datatools::things> er;

    // Allocate a new 'things' object from the metaclass factory
    // and store it in a scoped_ptr for memory sanity :
    er.reset (tMetaClass.construct<datatools::things> (DR_ARGS ("<unnamed>", "A generic event record")));

    // Build a meta object from this new allocated things :
    DR_OBJECT erObj = er.get ();

    // Invoke a print method :
    erObj.call("tree_print", DR_ARGS(1, "Event record (things) : "));

    // Change its 'name' property :
    erObj.set ("name", DR_VALUE ("event_record"));

    // Change its 'description' property :
    erObj.set ("description", DR_VALUE ("A smart generic event record"));

    // Print it again :
    erObj.call("tree_print", DR_ARGS(1, "Event record (things) : "));

    // Now automagically allocate and store a new bank named 'EH'
    // of the serialization type 'properties' :
    DR_VALUE ehVal = erObj.DR_CALL ("add_entry", DR_ARGS ("EH",
                                                         "datatools::properties",
                                                         "The event header", false));

    std::clog << "EH was added. " << std::endl;

    // More with a new bank named 'GC'
    // of the serialization type 'multi_properties' :
    DR_VALUE gcVal = erObj.DR_CALL ("add_entry", DR_ARGS ("GC",
                                                          "datatools::multi_properties",
                                                          "The global configuration",
                                                          false));
    std::clog << "GC was added. " << std::endl;

    // Manipulate the 'EH' bank :
    std::clog << "Build an object from the value : " << std::endl;
    DR_OBJECT ehObj = ehVal.to<DR_OBJECT>();
    std::clog << "Done." << std::endl;

    if (erObj.DR_CALL ("entry_is_a", DR_ARGS ("EH", "datatools::properties")))
      {
        std::clog << "Manipulate the 'EH' object : " << std::endl;
        ehObj.call ("store_flag", DR_ARGS("debug", "Debug flag", false));
        ehObj.call ("store_integer", DR_ARGS("run_number", 3, "Run number", true));
        ehObj.call ("store_integer", DR_ARGS("event_number", 0, "Event number", true));
        ehObj.call ("store_real", DR_ARGS("pi", 3.14, "Pi value", true));
        ehObj.call ("store_string", DR_ARGS("Author", "Monty", "Author's name", false));
        ehObj.call ("store_integer", DR_ARGS("Epoch", 123456789, "Date", false));
        ehObj.call ("tree_print", DR_ARGS(1, "Event header: "));
      }

    // Final print :
    erObj.call("tree_print", DR_ARGS(1, "Event record (things) : "));
  }
}

void test_multi_properties (bool debug_)
{
  std::clog << "*** test_multi_properties : " << std::endl;
  const DR_CLASS & mpropsMetaClass = DR_CLASS_BY_NAME("datatools::multi_properties");
  {
    boost::scoped_ptr<datatools::multi_properties> mprops;
    mprops.reset (mpropsMetaClass.construct<datatools::multi_properties> (DR_ARGS ("id", "class")));
    DR_OBJECT mpropsObj = mprops.get ();

    mpropsObj.set ("description", DR_VALUE ("A test multi-properties"));
    mpropsObj.call("tree_print", DR_ARGS(1, "Multi-properties: "));

    {
       mpropsObj.call ("set_description", DR_ARGS("The bar description"));
       DR_VALUE propsVal = mpropsObj.call ("get_description");
       std::clog << "NOTICE: Bar description is '"
                 << propsVal.to<std::string>() << "'." << std::endl;
    }

    {
      DR_VALUE propsVal = mpropsObj.call ("has_section", DR_ARGS("foo"));
      if (! propsVal.to<bool>())
        {
          std::clog << "NOTICE: No 'foo' section." << std::endl;
        }
    }

    {
      DR_VALUE propsVal = mpropsObj.call ("write", DR_ARGS("datatools_test_reflection_0a.txt", false, false));
    }

     {
      DR_VALUE propsVal = mpropsObj.call ("add_section", DR_ARGS("foo", "foo_type"));
      DR_OBJECT propsObj = propsVal.to<DR_OBJECT>();
      propsObj.call ("store_flag",    DR_ARGS("debug",  "Debug flag", false));
      propsObj.call ("store_real",    DR_ARGS("pi",     3.14, "Pi value", true));
      propsObj.call ("store_string",  DR_ARGS("Author", "Monty", "Author's name", false));
      propsObj.call ("store_integer", DR_ARGS("Epoch",  123456789, "Date", false));
      propsObj.call ("tree_print", DR_ARGS(1, "foo: "));
    }

    {
      DR_VALUE propsVal = mpropsObj.call ("add_section", DR_ARGS("bar", "bar_type"));
      DR_OBJECT propsObj = propsVal.to<DR_OBJECT>();
      propsObj.call ("store_string",  DR_ARGS("Version",  "1.0", "The version ID", false));
      propsObj.call ("store_integer", DR_ARGS("Major",    1, "The version major number", false));
      propsObj.call ("store_integer", DR_ARGS("Minor",    0, "The version minor number", false));
      propsObj.call ("store_integer", DR_ARGS("Revision", 0, "The version revision number", false));
      propsObj.call ("store_flag",    DR_ARGS("Devel",    "Development mode", false));
      propsObj.call ("tree_print", DR_ARGS(1, "bar: "));
    }

    mpropsObj.call("tree_print", DR_ARGS(1, "Multi-properties: "));
    mpropsObj.call ("write", DR_ARGS("datatools_test_reflection_0b.txt", true, false));

  }
}


void test_properties (bool debug_)
{
  std::clog << "*** test_properties : " << std::endl;
  const DR_CLASS & propsMetaClass = DR_CLASS_BY_NAME("datatools::properties");

  {
    boost::scoped_ptr<datatools::properties> props (propsMetaClass.construct<datatools::properties> (DR_ARGS ("A test properties")));

    props.get ()->tree_dump(std::clog, "Properties : ");

    DR_OBJECT propsObj(*props.get ());
    propsMetaClass.function("set_description").call(propsObj, DR_ARGS("A dummy properties"));
    propsObj.call ("store_flag", DR_ARGS("foo", "A dummy (boolean) flag", false));
    propsObj.call ("store_integer", DR_ARGS("number", 3, "Number of stars", false));
    propsObj.call ("store_real", DR_ARGS("pi", 3.14, "Value of Pi", true));
    propsObj.call ("store_string", DR_ARGS("author", "frc", "The author's name", true));
    propsObj.call ("store_boolean", DR_ARGS("debug", false, "Debug flag", false));

    propsObj.call("tree_print", DR_ARGS(1, "Properties: "));

    DR_VALUE piVal = propsObj.call ("fetch_real", DR_ARGS("pi"));
    std::clog << "piVal = " << piVal << std::endl;

    std::clog << "Change 'number' from 3 to 8 ..." << std::endl;
    propsObj.call ("change_integer", DR_ARGS("number", 8));

    std::clog << "Change 'number' from 3 to 17 (by string)..." << std::endl;
    propsObj.call ("change_integer", DR_ARGS("number", "17"));

    if (propsObj.call ("is_locked", DR_ARGS("pi")))
      {
        std::clog << "Cannot change Pi value because it is locked !" << std::endl;
        std::clog << "Unlock it first !" << std::endl;
        propsObj.call ("key_unlock", DR_ARGS("pi"));
        std::clog << "Changing Pi value..." << std::endl;
        propsObj.call ("change_real", DR_ARGS("pi", "3.14159"));
        std::clog << "Finally re-lock it !" << std::endl;
        propsObj.call ("key_lock", DR_ARGS("pi"));
      }
    propsObj.call("tree_print", DR_ARGS(1, "Properties: "));
  }
  return;
}


void test_event_id (bool debug_)
{
  std::clog << "*** test_event_id : " << std::endl;
  const DR_CLASS & evIdMetaClass = DR_CLASS_BY_NAME("datatools::event_id");

  {
    boost::scoped_ptr<datatools::event_id> evId (evIdMetaClass.construct<datatools::event_id> (DR_ARGS (3, 55)));

    std::clog << "Event ID = " << *evId.get () << std::endl;

    DR_OBJECT evIdObj(*evId.get ());
    evIdMetaClass.function("set").call(evIdObj, DR_ARGS(7, 567));
    std::clog << "Event ID = "
              << evIdMetaClass.function("to_string").call(evIdObj)
              << std::endl;

    evIdObj.set ("run_number", DR_VALUE(12));
    evIdObj.set ("event_number", DR_VALUE(3));
    std::clog << "Event ID = "
              << evIdObj.call ("to_string") << std::endl;

    evIdObj.set ("run_number", 13);
    evIdObj.set ("event_number", 4);
    std::clog << "Event ID = "
              << evIdObj.call ("to_string") << std::endl;

    evIdObj.call("tree_print", DR_ARGS(1, "Event ID: "));
  }

  return;
}


//#include<camp/detail/enummanager.hpp>

void test_logger (bool debug_)
{
  std::clog << "*** test_logger : " << std::endl;
  {
    const DR_CLASS & tMetaClass = camp::classByName("datatools::logger");
    std::clog << "tMetaClass = " << tMetaClass.name() <<  std::endl;

    try {
      camp::detail::EnumManager & emgr = camp::detail::EnumManager::instance();
      std::clog << "Count: " << emgr.count() << std::endl;

      if (emgr.enumExists("datatools::logger::priority")) {
        std::clog << "Exists: " << "datatools::logger::property" << std::endl;
      } else {
        std::clog << "Does not exist: " << "datatools::logger::property" << std::endl;
       }

      const DR_ENUM & tMetaEnum = camp::enumByName("datatools::logger::priority");
      for (int i = 0; i < tMetaEnum.size(); i++) {
        std::clog << " - Key '" << tMetaEnum.pair(i).name
                  << "' has value = " << tMetaEnum.pair(i).value << std::endl;
      }
    } catch (camp::InvalidEnum & ie) {
      std::cerr << "Error: test_logger: " << ie.what() << " for enum named '" << ie.enumName() << "'" << std::endl;
      throw;
    }
 }

  return;
}

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try
    {
      bool debug = false;
      long seed  = 12345;
      bool test = false;
      int nrecords = 3;

      int iarg = 1;
      while (iarg < argc_)
        {
          std::string arg = argv_[iarg];
          if (arg[0] == '-')
            {
              if (arg == "-d") debug = true;
              if (arg == "-10") nrecords = 10;
              if (arg == "-100") nrecords = 100;
              if (arg == "-1000") nrecords = 1000;
            }
          else
            {
            }
          iarg++;
        }

      srand48 (seed);

      DT_LOG_NOTICE(logging, "Number of metaclasses = " << camp::classCount());
      for (int i = 0; i < camp::classCount(); i++) {
        const camp::Class & c = camp::classByIndex(i);
        DT_LOG_NOTICE(logging, "Metaclass #" << i << " : " << c.name());
      }

      DT_LOG_NOTICE(logging, "Number of metaenums = " << camp::enumCount());
      for (int i = 0; i < camp::enumCount(); i++) {
        const camp::Enum & e = camp::enumByIndex(i);
        DT_LOG_NOTICE(logging, "Metaenum #" << i << " : " << e.name());
      }

      test_logger (debug);

      test_event_id (debug);

      test_properties (debug);

      test_multi_properties (debug);

      test_things (debug);

      std::clog << "The end" << std::endl;

    }
  catch (std::exception & x)
    {
      std::cerr << "ERROR: test_reflection_0: " << x.what () << std::endl;
      exit (EXIT_FAILURE);
    }

  return (EXIT_SUCCESS);
}

/* end of test_reflection_0.cxx */
