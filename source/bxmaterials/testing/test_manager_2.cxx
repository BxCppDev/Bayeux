// -*- mode: c++ ; -*-
// test_manager_2.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <list>

#include <datatools/utils.h>
#include <datatools/multi_properties.h>

#include <materials/manager.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'materials::manager' !" << endl;
    bool debug = false;
    std::string MatMgrSetupFile;
    int iarg = 1;
    bool parsing_options = true;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        if (! parsing_options) {
          clog << "warning: ignoring any option after the list of arguments !" << endl;
          break;
        }
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
            debug = true;
        } else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        clog << "warning: ignoring argument '" << argument << "'!" << endl;
      }
      iarg++;
    }

    if (MatMgrSetupFile.empty()) {
      MatMgrSetupFile = "${MATERIALS_DATA_DIR}/testing/config/test_manager.conf";
    }
    datatools::fetch_path_with_env(MatMgrSetupFile);

    materials::manager MatMgr;
    datatools::properties MatMgrSetup;
    datatools::properties::read_config(MatMgrSetupFile, MatMgrSetup);
    MatMgr.initialize (MatMgrSetup);
    MatMgr.tree_dump (clog, "Material manager: ");

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_manager_2.cxx
