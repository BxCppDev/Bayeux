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

void test_0();
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
    test_0();
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

void test_0()
{
  datatools::event_id a;

  DT_THROW_IF(a.is_valid(), std::logic_error, "Default constructed event_id must be invalid");

  datatools::event_id b{1,2};
  DT_THROW_IF(!b.is_valid(), std::logic_error, "User constructed event_id must be valid");

  datatools::event_id c{b};
  DT_THROW_IF(!(c == b), std::logic_error, "Copy construction must yield identical objects");

  datatools::event_id d = b;
  DT_THROW_IF(!(c == b), std::logic_error, "Copy assignment must yield identical objects");

  datatools::event_id e{std::move(d)};
  DT_THROW_IF(!(e == b), std::logic_error, "Move construction does not yield correct object");

  datatools::event_id f = std::move(e);
  DT_THROW_IF(!(f == b), std::logic_error, "Move assignment does not yield correct object");
}

void test_1()
{
  datatools::event_id my_id;

  my_id.set(666, 34);

  std::clog << "Event id = " << my_id << std::endl;

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
      std::clog << "Enter event id [format=XXX_YYY] ? ";
      ins >> my_id;
      if(! ins) {
        throw std::runtime_error("Format error!");
      }
      std::clog << "Token : '" << tok << "'" << std::endl;
      std::clog << "Event id = " << my_id << std::endl;
      my_id.tree_dump(std::clog, "datatools::event_id:");
    } catch(std::exception & x) {
      std::cerr << "Format error !" << std::endl;
    }
  }

  std::string filename = "test_event_id.xml";
  {
    std::clog << "Storing..." << std::endl;
    std::list<datatools::event_id> ids;
    datatools::data_writer writer(filename);
    for(int i = 0; i < 4; i++) {
      ids.push_back(datatools::event_id(666, i));
      ids.back().tree_dump(std::clog, "event_id","<<< ");
      writer.store(ids.back());
    }
  }

  {
    std::clog << "Loading..." << std::endl;
    std::list<datatools::event_id> ids;
    datatools::data_reader reader(filename);
    while(reader.has_record_tag()) {
      if(reader.record_tag_is(datatools::event_id::SERIAL_TAG)) {
        datatools::event_id id;
        ids.push_back(id);
        reader.load(ids.back());
        ids.back().tree_dump(std::clog, "std::event_id",">>> ");
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
