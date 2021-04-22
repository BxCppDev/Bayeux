// -*- mode: c++; -*-
// test_genbb_mgr_5.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/properties.h>

#include <genbb_help/genbb_mgr.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    string config_file;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];
      if (arg[0] == '-') {
        if (arg == "-d" || arg == "--debug") debug = true;
      } else {
        if (config_file.empty ()) {
          config_file = arg;
        }
      }
      iarg++;
    }

    if (config_file.empty ()) {
      config_file = "${GENBB_HELP_TESTING_DIR}/config/genbb_mgr_0.conf";
    }
    datatools::fetch_path_with_env (config_file);
    datatools::properties config;
    datatools::read_config (config_file, config);

    genbb::genbb_mgr mgr;
    // initialize the manager:
    mgr.initialize_standalone (config);
    mgr.dump (clog);

    // working primary event:
    genbb::primary_event pe;

    size_t count = 0;
    // main loop on primary events source:
    while (mgr.has_next ()) {
      mgr.load_next (pe);
      if (debug) pe.dump ();
      count++;
    }
    mgr.reset ();

    clog << "Number of loaded events: " << count << endl;
    if (debug) mgr.dump ();

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_genbb_mgr_5.cxx
