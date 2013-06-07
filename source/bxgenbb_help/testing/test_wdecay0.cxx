// -*- mode: c++; -*-
// test_wdecay0.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/wdecay0.h>
#include <genbb_help/primary_event.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    bool test  = false;
    bool dump  = false;
    size_t max_count = 10;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-t" || arg == "--test") test = true;
      if (arg == "-D" || arg == "--dump") dump = true;
      if (arg == "-1000") max_count = 1000;
      iarg++;
    }

    datatools::properties config;
    if (debug) config.store_flag ("logging.debug");
    config.store ("seed", 314159);
    config.store ("decay_type", "DBD");
    config.store ("decay_isotope", "Se82");
    config.store ("decay_dbd_level", 0);
    config.store ("decay_dbd_mode", 4);
    config.store ("energy_min", 2.0);
    config.store ("energy_max", 3.2);
    config.store ("energy_unit", "MeV");
    if (debug) config.tree_dump (clog, "Configuration: ", "debug: ");

    genbb::wdecay0 WD0;
    if (debug)  WD0.set_logging_priority(datatools::logger::PRIO_DEBUG);
    WD0.initialize_standalone (config);

    // working primary event:
    genbb::primary_event pe;

    for (int i = 0; i < max_count; i++) {
      if (((i % 100) == 0) || (i + 1) == max_count) {
        clog << "Count : " << i << endl;
      }
      WD0.load_next (pe);
      if (dump) pe.dump ();
    }

    if (debug) clog << "debug: " << "The end." << endl;
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

// end of test_wgenbb.cxx
