// -*- mode: c++ ; -*-
// test_simulated_data_input_module_2.cxx

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

using namespace std;

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'simulated_data_input_module'!" << endl;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          logging = datatools::logger::PRIO_DEBUG;
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '"
               << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    // The input data processing module :
    mctools::simulated_data_input_module PSDIM;
    PSDIM.set_logging_priority(logging);
    PSDIM.set_sd_bank_label("SD");
    PSDIM.set_overwrite_existing_sd_bank_label(true);
    std::vector<std::string> filenames;
    filenames.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_1.brio");
    filenames.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_1.xml");
    PSDIM.set_list_of_input_files(filenames, false);
    PSDIM.set_limits(100000, 10000, 100);
    PSDIM.initialize_simple();

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

// end of test_simulated_data_input_module_2.cxx
