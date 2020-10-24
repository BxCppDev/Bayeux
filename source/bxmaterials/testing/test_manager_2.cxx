// test_manager_2.cxx

// Ourselves:
#include <materials/manager.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <list>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/multi_properties.h>

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'materials::manager' !" << std::endl;
    // bool debug = false;
    std::string MatMgrSetupFile;
    // int iarg = 1;
    // bool parsing_options = true;
    // while (iarg < argc_) {
    //   std::string token = argv_[iarg];
    //   if (token[0] == '-') {
    //     if (! parsing_options) {
    //       std::clog << "warning: ignoring any option after the list of arguments !" << std::endl;
    //       break;
    //     }
    //     std::string option = token;
    //     if ((option == "-d") || (option == "--debug")) {
    //         debug = true;
    //     } else {
    //       std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
    //     }
    //   } else {
    //     std::string argument = token;
    //     std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
    //   }
    //   iarg++;
    // }

    if (MatMgrSetupFile.empty()) {
      MatMgrSetupFile = "${MATERIALS_RESOURCE_DIR}/../testing/config/test_manager.conf";
    }
    datatools::fetch_path_with_env(MatMgrSetupFile);

    materials::manager MatMgr;
    datatools::properties MatMgrSetup;
    datatools::read_config(MatMgrSetupFile, MatMgrSetup);
    MatMgr.initialize (MatMgrSetup);
    MatMgr.tree_dump (std::clog, "Material manager: ");

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
