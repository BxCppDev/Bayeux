// -*- mode: c++; -*-
// test_urn.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// Ourselves:
#include <datatools/urn.h>

// Thios project:
#include <datatools/version_id.h>

void test_urn_0();

int main (int /* argc_ */, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'datatools::urn' class..." << std::endl;

    test_urn_0();

  } catch (std::exception & x) {
    std::clog << "[fatal] " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "[fatal] " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_urn_0()
{
  std::clog << "[info] " << "test_urn_0..." << std::endl;

  datatools::urn u0;
  std::clog << "u0 = " << u0 << std::endl;

  datatools::urn u1;
  u1.append_segment("snemo");
  u1.append_segment("demonstrator");
  u1.append_segment("sw");
  u1.append_segment("falaise");
  std::clog << "u1 = " << u1 << std::endl;

  datatools::urn u2;
  u2.append_segment("snemo");
  u2.append_segment("demonstrator");
  u2.append_segment("geometry");
  u2.append_segment(datatools::version_id(4,0).to_string());
  std::clog << "u2 = " << u2 << std::endl;


  std::clog << std::endl;
  return;
}
