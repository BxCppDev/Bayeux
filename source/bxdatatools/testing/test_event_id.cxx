// test_event_id.cxx

// // The Catch API:
// #include "catch.hpp"

// Ourselves:
#include <datatools/event_id.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <list>
#include <vector>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>

// This project:
#include <datatools/io_factory.h>

void test_1();

// TEST_CASE("Test Bayeux/datatools::event_id class", "[bxdatatools][event_id]")
// {
//   test_1();
// }

int main(int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Hello, this is a test program for class 'datatools::event_id'!" << std::endl;
    test_1();
  } catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_1()
{
  using namespace std;
  datatools::event_id my_id;

  my_id.set(666, 34);

  clog << "Event id = " << my_id << endl;

  std::vector<std::string> tokens = {
    "666_7635",
    "*_*",
    "!_!",
    "!_*",
    "*_!"
  };
  for (auto tok : tokens) {
    try {
      std::istringstream ins(tok);
      clog << "Enter event id [format=XXX_YYY] ? ";
      ins >> my_id;
      if(! ins) {
        throw runtime_error("Format error!");
      }
      clog << "Token : '" << tok << "'" << std::endl;
      clog << "Event id = " << my_id << endl;
      my_id.tree_dump(clog, "datatools::event_id:");
    } catch(exception & x) {
      cerr << "Format error !" << endl;
    }
  }

  string filename = "test_event_id.xml";
  {
    clog << "Storing..." << endl;
    list<datatools::event_id> ids;
    datatools::data_writer writer(filename);
    for(int i = 0; i < 4; i++) {
      ids.push_back(datatools::event_id(666, i));
      ids.back().tree_dump(clog, "event_id","<<< ");
      writer.store(ids.back());
    }
  }

  {
    clog << "Loading..." << endl;
    list<datatools::event_id> ids;
    datatools::data_reader reader(filename);
    while(reader.has_record_tag()) {
      if(reader.record_tag_is(datatools::event_id::SERIAL_TAG)) {
        datatools::event_id id;
        ids.push_back(id);
        reader.load(ids.back());
        ids.back().tree_dump(clog, "event_id",">>> ");
      } else {
        break;
      }
    }
  }

  if (boost::filesystem::exists(filename)) {
    boost::filesystem::remove(filename);
  }
  return;
}
