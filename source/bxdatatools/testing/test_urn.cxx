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
  std::clog << "[info] u0 = " << u0 << std::endl;

  datatools::urn u1;
  u1.append_segment("snemo");
  u1.append_segment("demonstrator");
  u1.append_segment("sw");
  u1.append_segment("falaise");
  std::clog << "[info] u1 = " << u1 << std::endl;

  datatools::urn u2;
  u2.append_segment("snemo");
  u2.append_segment("demonstrator");
  u2.append_segment("geometry");
  u2.append_segment(datatools::version_id(4,0).to_string());
  std::clog << "[info] u2 = " << u2 << std::endl;

  std::clog << "\n[info] Parsing:" << std::endl;
  std::string su3("!");
  datatools::urn u3;
  if (u3.from_string(su3)) {
    std::clog << "[info] u3 = " << u3 << std::endl;
  } else {
    std::cerr << "[error] invalid URN format = '" << su3 << "'" << std::endl;
  }

  std::string su4("http:dqqf");
  datatools::urn u4;
  if (u4.from_string(su4)) {
    std::clog << "[info] u4 = " << u4 << std::endl;
  } else {
    std::cerr << "[error] invalid URN format = '" << su4 << "'" << std::endl;
  }

  std::string su5("urn:");
  datatools::urn u5;
  if (u5.from_string(su5)) {
    std::clog << "[info] u5 = " << u5 << std::endl;
  } else {
    std::cerr << "[error] invalid URN format = '" << su5 << "'" << std::endl;
  }

  std::string su6("urn:snemo:demonstrator:");
  datatools::urn u6;
  if (u6.from_string(su6)) {
    std::clog << "[info] u6 = " << u6 << std::endl;
  } else {
    std::cerr << "[error] invalid URN format = '" << su6 << "'" << std::endl;
  }

  std::string su7("urn:snemo:demonstrator:geometry:4.0-beta");
  datatools::urn u7;
  if (u7.from_string(su7)) {
    std::clog << "[info] u7 = " << u7 << std::endl;
  } else {
    std::cerr << "[error] invalid URN format = '" << su7 << "'" << std::endl;
  }

  std::string su8("snemo:demonstrator:geometry:4.0-beta");
  datatools::urn u8;
  if (u8.from_string(su8)) {
    std::clog << "[info] u8 = " << u8 << std::endl;
  } else {
    std::cerr << "[error] invalid URN format = '" << su8 << "'" << std::endl;
  }

  std::clog << std::endl;
  return;
}
