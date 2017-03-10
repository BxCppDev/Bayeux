// -*- mode: c++; -*-
// test_urn_to_path.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// Third party:
#include <boost/serialization/map.hpp>

// Ourselves:
#include <datatools/urn_to_path.h>
#include <datatools/io_factory.h>

// This project:
#include <datatools/version_id.h>

void test_urn_to_path_0();

int main (int /* argc_ */, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'datatools::urn_to_path' class..." << std::endl;

    test_urn_to_path_0();

  } catch (std::exception & x) {
    std::clog << "[fatal] " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "[fatal] " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_urn_to_path_0()
{
  std::clog << "[info] " << "test_urn_to_path_0..." << std::endl;

  datatools::urn_to_path u2p0;
  u2p0.set_urn("urn:config:default:datatools:rc");
  u2p0.set_path("~/.config/default/bxdatatools.rc");
  u2p0.set_category("configuration");
  u2p0.set_mime("text/configuration");
  u2p0.tree_dump(std::clog, "URN => path record : ", "[info] ");
  std::clog << std::endl;

  std::clog << "[info] " << "test_urn_to_path_0 : The end." << std::endl;
  return;
}
