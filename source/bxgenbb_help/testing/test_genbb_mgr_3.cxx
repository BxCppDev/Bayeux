// -*- mode: c++; -*-
// test_genbb_mgr_3.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/genbb_mgr.h>
#include <genbb_help/primary_event.h>

#include <datatools/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
//#include <datatools/bio_guard.h>
//#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      iarg++;
    }

    genbb::genbb_mgr mgr;

    // genbb input data files:
    mgr.set ("${GENBB_HELP_TESTING_DIR}/data/bipo212_1.genbb");
    mgr.set ("${GENBB_HELP_TESTING_DIR}/data/bipo212_2.genbb");
    mgr.set ("${GENBB_HELP_TESTING_DIR}/data/bipo212_3.genbb");
    if (debug) mgr.dump ();

    // initialize the manager:
    mgr.initialize_simple ();
    if (debug) mgr.dump ();

    // working primary event:
    genbb::primary_event pe;

    datatools::data_writer writer;

    writer.init ("bipo212_123.txt.gz",
                 datatools::using_multi_archives);

    size_t count = 0;
    // main loop on primary events source:
    while (mgr.has_next ()) {
      mgr.load_next (pe);
      writer.store (pe);
      count++;
    }
    mgr.reset ();

    std::clog << "Number of loaded events: " << count << std::endl;
    if (debug) mgr.dump ();

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...)  {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_genbb_mgr_3.cxx
