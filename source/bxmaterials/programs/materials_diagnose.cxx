// -*- mode: c++ ; -*-
// materials_diagnose.cxx

// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// - Bayeux:
#include <bayeux/bayeux.h>
#include <bayeux/version.h>
#include <bayeux/reloc.h>
#include <bayeux/BayeuxBinReloc.h>

// This project:
#include <materials/version.h>
#include <materials/resource.h>

/****************
 * Main program *
 ****************/
int main (int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);

  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  try {

    std::cout << "Bayeux:" << std::endl;
    std::cout << "  Shared lib path   : " << br_find_exe(0) << std::endl;
    std::cout << "  Shared lib dir    : " << br_find_exe_dir(0) << std::endl;
    std::cout << "  Version           : " << bayeux::version::get_version() << std::endl;
    std::cout << "  Prefix dir        : " << bayeux::get_prefix_dir() << std::endl;
    std::cout << "  Binary dir        : " << bayeux::get_binary_dir() << std::endl;
    std::cout << "  Library dir       : " << bayeux::get_library_dir() << std::endl;
    std::cout << "  Data dir          : " << bayeux::get_data_dir() << std::endl;
    std::cout << "  Resource dir      : " << bayeux::get_resource_dir() << std::endl;
    std::cout << "Bayeux/materials:" << std::endl;
    std::cout << "  Version           : " << materials::version::get_version() << std::endl;
    std::cout << "  Resource dir      : " << materials::get_resource_dir() << std::endl;
    std::cout << "  Standard isotopes : " << materials::get_resource("data/std_isotopes.def") << std::endl;

  }
  catch (std::exception & x) {
    DT_LOG_ERROR(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_ERROR(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}
