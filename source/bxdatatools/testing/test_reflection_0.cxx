/* test_reflection_0.cxx */

// Standard Library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third Party:
// - Camp:
#include <camp/userobject.hpp>
#include <camp/value.hpp>
#include <camp/args.hpp>
#include <camp/class.hpp>
#include <camp/errors.hpp>
#include <camp/enum.hpp>

// This Project:
#include <datatools/logger.h>
// Introspectable classes :
#include <datatools/logger.h>
#include <datatools/event_id.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/things.h>

void test_things(bool /*debug_*/);
void test_multi_properties(bool /*debug_*/);
void test_properties(bool /*debug_*/);
void test_event_id(bool /*debug_*/);
void test_logger(bool /*debug_*/);

int main (int /*argc_*/, char ** /*argv_*/)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try {
    bool debug = false;
    long seed  = 12345;

    {
      // 2016-08-12 FM: Trick to force linkage to libBayeux:
      datatools::properties force_bx_linkage;
    }

    srand48(seed);

    DT_LOG_NOTICE(logging, "Number of metaclasses = " << camp::classCount());
    for (int i = 0; i < (int) camp::classCount(); i++) {
      const camp::Class & c = camp::classByIndex(i);
      DT_LOG_NOTICE(logging, "Metaclass #" << i << " : " << c.name());
    }

    DT_LOG_NOTICE(logging, "Number of metaenums = " << camp::enumCount());
    for (int i = 0; i < (int) camp::enumCount(); i++) {
      const camp::Enum & e = camp::enumByIndex(i);
      DT_LOG_NOTICE(logging, "Metaenum #" << i << " : " << e.name());
    }

    test_logger(debug);

    test_event_id(debug);

    test_properties(debug);

    test_multi_properties(debug);

    test_things(debug);

    std::clog << "The end" << std::endl;
  } catch (camp::BadType & ic) {
    std::cerr << "ERROR: test_reflection_0: " << ic.what () << std::endl;
    exit (EXIT_FAILURE);
  }
  catch (std::exception & x) {
    std::cerr << "ERROR: test_reflection_0: " << x.what () << std::endl;
    exit (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}

void test_things (bool /*debug_*/)
{
  std::clog << "*** test_things : " << std::endl;

  // Access meta class for class 'things' :
  const camp::Class & tMetaClass = camp::classByName("datatools::things");
  {

    // Allocate a new 'things' object from the metaclass factory
    // and store it in a scoped_ptr for memory sanity :
    camp::UserObject erObj = tMetaClass.construct(camp::Args ("<unnamed>", "A generic event record"));

    // Build a meta object from this new allocated things :
    //camp::UserObject erObj = er.get ();

    // Invoke a print method :
    erObj.call("tree_print", camp::Args(1, "Event record (things) : "));

    // Change its 'name' property :
    erObj.set("name", camp::Value ("event_record"));

    // Change its 'description' property :
    erObj.set("description", camp::Value ("A smart generic event record"));

    // Print it again :
    erObj.call("tree_print", camp::Args(1, "Event record (things) : "));

    // Now automagically allocate and store a new bank named 'EH'
    // of the serialization type 'properties' :
    camp::Value ehVal = erObj.call("add_entry", camp::Args ("EH",
                                                            "datatools::properties",
                                                            "The event header", false));

    std::clog << "EH was added. " << std::endl;

    // More with a new bank named 'GC'
    // of the serialization type 'multi_properties' :
    camp::Value gcVal = erObj.call ("add_entry", camp::Args ("GC",
                                                             "datatools::multi_properties",
                                                             "The global configuration",
                                                             false));
    std::clog << "GC was added. " << std::endl;

    // Manipulate the 'EH' bank :
    std::clog << "Build an object from the value : " << std::endl;
    camp::UserObject ehObj = ehVal.to<camp::UserObject>();
    std::clog << "Done." << std::endl;

    if (erObj.call ("entry_is_a", camp::Args ("EH", "datatools::properties")))
      {
        std::clog << "Manipulate the 'EH' object : " << std::endl;
        ehObj.call ("store_flag", camp::Args("debug", "Debug flag", false));
        ehObj.call ("store_integer", camp::Args("run_number", 3, "Run number", true));
        ehObj.call ("store_integer", camp::Args("event_number", 0, "Event number", true));
        ehObj.call ("store_real", camp::Args("pi", 3.14, "Pi value", true));
        ehObj.call ("store_string", camp::Args("Author", "Monty", "Author's name", false));
        ehObj.call ("store_integer", camp::Args("Epoch", 123456789, "Date", false));
        ehObj.call ("tree_print", camp::Args(1, "Event header: "));
      }

    // Final print :
    erObj.call("tree_print", camp::Args(1, "Event record (things) : "));
  }
}

void test_multi_properties (bool /*debug_*/)
{
  std::clog << "*** test_multi_properties : " << std::endl;
  const camp::Class & mpropsMetaClass = camp::classByName("datatools::multi_properties");
  {
    camp::UserObject mpropsObj = mpropsMetaClass.construct(camp::Args ("id", "class"));

    mpropsObj.set ("description", camp::Value ("A test multi-properties"));
    mpropsObj.call("tree_print", camp::Args(1, "Multi-properties: "));

    {
      mpropsObj.call ("set_description", camp::Args("The bar description"));
      camp::Value propsVal = mpropsObj.call ("get_description");
      std::clog << "NOTICE: Bar description is '"
                << propsVal.to<std::string>() << "'." << std::endl;
    }

    {
      camp::Value propsVal = mpropsObj.call ("has_section", camp::Args("foo"));
      if (! propsVal.to<bool>())
        {
          std::clog << "NOTICE: No 'foo' section." << std::endl;
        }
    }

    {
      //camp::Value propsVal = mpropsObj.call ("write", camp::Args("datatools_test_reflection_0a.txt", false, false));
    }

    {
      camp::Value propsVal = mpropsObj.call ("add_section", camp::Args("foo", "foo_type"));
      camp::UserObject propsObj = propsVal.to<camp::UserObject>();
      propsObj.call ("store_flag",    camp::Args("debug",  "Debug flag", false));
      propsObj.call ("store_real",    camp::Args("pi",     3.14, "Pi value", true));
      propsObj.call ("store_string",  camp::Args("Author", "Monty", "Author's name", false));
      propsObj.call ("store_integer", camp::Args("Epoch",  123456789, "Date", false));
      propsObj.call ("tree_print", camp::Args(1, "foo: "));
    }

    {
      camp::Value propsVal = mpropsObj.call ("add_section", camp::Args("bar", "bar_type"));
      camp::UserObject propsObj = propsVal.to<camp::UserObject>();
      propsObj.call ("store_string",  camp::Args("Version",  "1.0", "The version ID", false));
      propsObj.call ("store_integer", camp::Args("Major",    1, "The version major number", false));
      propsObj.call ("store_integer", camp::Args("Minor",    0, "The version minor number", false));
      propsObj.call ("store_integer", camp::Args("Revision", 0, "The version revision number", false));
      propsObj.call ("store_flag",    camp::Args("Devel",    "Development mode", false));
      propsObj.call ("tree_print", camp::Args(1, "bar: "));
    }

    mpropsObj.call("tree_print", camp::Args(1, "Multi-properties: "));
    //mpropsObj.call ("write", camp::Args("datatools_test_reflection_0b.txt", true, false));

  }
}


void test_properties (bool /*debug_*/)
{
  std::clog << "*** test_properties : " << std::endl;
  const camp::Class & propsMetaClass = camp::classByName("datatools::properties");

  {
    //boost::scoped_ptr<datatools::properties> props (propsMetaClass.construct<datatools::properties> (camp::Args ("A test properties")));
    //props.get ()->tree_dump(std::clog, "Properties : ");

    camp::UserObject propsObj = propsMetaClass.construct(camp::Args ("A test properties"));
    propsMetaClass.function("set_description").call(propsObj, camp::Args("A dummy properties"));
    propsObj.call ("store_flag", camp::Args("foo", "A dummy (boolean) flag", false));
    propsObj.call ("store_integer", camp::Args("number", 3, "Number of stars", false));
    propsObj.call ("store_real", camp::Args("pi", 3.14, "Value of Pi", true));
    propsObj.call ("store_string", camp::Args("author", "frc", "The author's name", true));
    propsObj.call ("store_boolean", camp::Args("debug", false, "Debug flag", false));

    propsObj.call("tree_print", camp::Args(1, "Properties: "));

    camp::Value piVal = propsObj.call ("fetch_real", camp::Args("pi"));
    std::clog << "piVal = " << piVal << std::endl;

    std::clog << "Change 'number' from 3 to 8 ..." << std::endl;
    propsObj.call ("change_integer", camp::Args("number", 8));

    std::clog << "Change 'number' from 3 to 17 (by string)..." << std::endl;
    propsObj.call ("change_integer", camp::Args("number", "17"));

    if (propsObj.call ("is_locked", camp::Args("pi")))
      {
        std::clog << "Cannot change Pi value because it is locked !" << std::endl;
        std::clog << "Unlock it first !" << std::endl;
        propsObj.call ("unlock", camp::Args("pi"));
        std::clog << "Changing Pi value..." << std::endl;
        propsObj.call ("change_real", camp::Args("pi", "3.14159"));
        std::clog << "Finally re-lock it !" << std::endl;
        propsObj.call ("lock", camp::Args("pi"));
      }
    propsObj.call("tree_print", camp::Args(1, "Properties: "));
  }
  return;
}


void test_event_id (bool /*debug_*/)
{
  std::clog << "*** test_event_id : " << std::endl;
  const camp::Class & evIdMetaClass = camp::classByName("datatools::event_id");

  {
    //boost::scoped_ptr<datatools::event_id> evId (evIdMetaClass.construct<datatools::event_id> (camp::Args (3, 55)));
    //std::clog << "Event ID = " << *evId.get () << std::endl;

    camp::UserObject evIdObj = evIdMetaClass.construct(camp::Args (3, 55));

    evIdMetaClass.function("set").call(evIdObj, camp::Args(7, 567));
    std::clog << "Event ID = "
              << evIdMetaClass.function("to_string").call(evIdObj)
              << std::endl;

    evIdObj.set ("run_number", camp::Value(12));
    evIdObj.set ("event_number", camp::Value(3));
    std::clog << "Event ID = "
              << evIdObj.call ("to_string") << std::endl;

    evIdObj.set ("run_number", 13);
    evIdObj.set ("event_number", 4);
    std::clog << "Event ID = "
              << evIdObj.call ("to_string") << std::endl;

    evIdObj.call("tree_print", camp::Args(1, "Event ID: "));
  }

  return;
}


void test_logger (bool /*debug_*/)
{
  std::clog << "*** test_logger : " << std::endl;
  {
    const camp::Class & tMetaClass = camp::classByName("datatools::logger");
    std::clog << "tMetaClass = " << tMetaClass.name() <<  std::endl;

    try {
      camp::detail::EnumManager & emgr = camp::detail::EnumManager::instance();
      std::clog << "Count: " << emgr.count() << std::endl;

      if (emgr.enumExists("datatools::logger::priority")) {
        std::clog << "Exists: " << "datatools::logger::property" << std::endl;
      } else {
        std::clog << "Does not exist: " << "datatools::logger::property" << std::endl;
      }

      const camp::Enum & tMetaEnum = camp::enumByName("datatools::logger::priority");
      for (int i = 0; i < (int) tMetaEnum.size(); i++) {
        std::clog << " - Key '" << tMetaEnum.pair(i).name
                  << "' has value = " << tMetaEnum.pair(i).value << std::endl;
      }
    } catch (camp::EnumNotFound & ie) {
      std::cerr << "Error: test_logger: " << ie.what() << " for enum named '" << "'" << std::endl;
      throw;
    }
  }

  return;
}
