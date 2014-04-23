// -*- mode: c++ ; -*-
// test_simulated_data_input_module_1.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include <boost/scoped_ptr.hpp>

// Generic data model :
#include <datatools/things.h>

// Simulated data model :
#include <mctools/simulated_data.h>
#include <mctools/simulated_data_input_module.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>
#include <mctools/bio_guard.h>

using namespace std;

int main (int /* argc_ */, char ** /* argv_ */)
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'simulated_data_input_module'!" << endl;

    // bool debug = false;
    // int iarg = 1;
    // while (iarg < argc_) {
    //   string token = argv_[iarg];
    //   if (token[0] == '-') {
    //     string option = token;
    //     if ((option == "-d") || (option == "--debug")) {
    //       logging = datatools::logger::PRIO_DEBUG;
    //       clog << "warning: ignoring option '" << option << "'!" << endl;
    //     }
    //   }
    //   else {
    //     string argument = token;
    //     {
    //       clog << "warning: ignoring argument '"
    //            << argument << "'!" << endl;
    //     }
    //   }
    //   iarg++;
    // }

    // The input data processing module :
    mctools::simulated_data_input_module PSDIM(logging);
    datatools::properties PSDIMconf;
    PSDIMconf.store("logging.priority", datatools::logger::get_priority_label(logging));
    PSDIMconf.store("SD_bank.label", "SD");
    PSDIMconf.store_flag("SD_bank.overwrite_existing");
    PSDIMconf.store("input.files.mode", "list");
    std::vector<string> filenames;
    filenames.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_1.brio");
    filenames.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_1.xml");
    PSDIMconf.store("input.files.list.filenames", filenames);
    PSDIMconf.store("input.max_files", 100);
    PSDIMconf.store("input.max_record_total", 100000);
    PSDIMconf.store("input.max_record_per_file", 10000);
    PSDIMconf.tree_dump(std::clog, "Input module configuration parameters: ");
    PSDIM.initialize_standalone(PSDIMconf);

    // A event record object :
    datatools::things ER;

    // Load a simulated data bank from the input module:
    int count = 0;
    while (! PSDIM.is_terminated ()) {
      PSDIM.process(ER);
      clog << "The 'event_record' object has been filled with a 'simulated data' bank." << endl;
      ER.tree_dump(std::clog, "Event record: ");
      count++;
    }
    std::clog << "Number of processed event records: " << count << std::endl;

  } catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_simulated_data_input_module_1.cxx
