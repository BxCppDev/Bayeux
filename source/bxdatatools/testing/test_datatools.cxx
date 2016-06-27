// test_datatools.cxx

// The Catch API:
#include "catch.hpp"

// Ourselves:
#include <datatools/datatools_config.h>
#include <datatools/version.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

void datatools_version()
{
  std::cerr << "Datatools version is: " << datatools::version::get_version() << std::endl;
  return;
}

TEST_CASE("Test Bayeux/datatools version", "[bxdatatools][datatools]")
{
  datatools_version();
  // try {
  //   throw std::logic_error("A dummy error!");
  // } catch (std::exception & x) {
  //   std::cerr << "ERROR: " << x.what() << std::endl;
  //   throw;
  // }
}
