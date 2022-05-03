// -*- mode: c++; -*-
// test_library_loader_0.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

// Third Party:
// - Boost:
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/datatools_config.h>
#include <datatools/detail/DynamicLoader.hxx>

int main (/*int argc_ , char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'datatools::library_loader' class." << std::endl;

    std::vector<datatools::DynamicLoader::LibraryHandle> h;

    std::clog << "************* Loading 'ssl'..." << std::endl;
    h.push_back (datatools::DynamicLoader::OpenLibrary("libssl.so"));

    std::clog << "************* Loading 'crypto'..." << std::endl;
    h.push_back (datatools::DynamicLoader::OpenLibrary("libcrypto.so"));

    std::clog << "************* Load is done." << std::endl;

    for (int i = (h.size () - 1) ; i >= 0 ; --i)
      {
        std::clog << "************* Closing library i=" << i << std::endl;
        datatools::DynamicLoader::CloseLibrary(h[i]);
      }

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
