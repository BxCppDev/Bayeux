// -*- mode: c++ ; -*-
// test_simulated_data_reader_1.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include <boost/scoped_ptr.hpp>

// Utilities :
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>


// Simulated data model :
#include <mctools/simulated_data.h>
#include <mctools/simulated_data_reader.h>

// Serialization :
#include <datatools/io_factory.h>
#include <brio/writer.h>
#include <mctools/utils.h>
#include <mctools/simulated_data.ipp>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>
#include <mctools/bio_guard.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'simulated_data_reader'!" << endl;

    bool debug = false;
    // bool hf = false; // store header/footer

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      }
      else {
        string argument = token;
        {
          clog << "warning: ignoring argument '"
               << argument << "'!" << endl;
        }
      }
      iarg++;
    }


    clog << "*******************************************************************************" << endl;
    clog << "Step 1: Deserialization of simulated data objects through the dedicated reader" << endl;
    clog << "*******************************************************************************" << endl;
    {
      mctools::simulated_data_reader reader;
      if (debug) reader.set_logging_priority(datatools::logger::PRIO_TRACE);
      std::vector<std::string> files;
      files.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_0.brio");
      files.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_0.xml");
      files.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_1.xml");
      files.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_1.brio");
      files.push_back("${MCTOOLS_TESTING_DIR}/samples/test_simulated_data_2.xml");
      reader.initialize(files);
      if (debug) reader.tree_dump(std::cerr, "Reader status: ");
      if (reader.has_run_header()) {
        reader.get_run_header().tree_dump(std::clog, "Run header (loaded from the dedicated reader) :");
      }
      int count = 0;
      while (reader.has_next()) {
        mctools::simulated_data SD;
        reader.load_next(SD);
        SD.tree_dump(std::clog, "Simulated data (loaded from the dedicated reader) :");
        if (debug) reader.tree_dump(std::cerr, "Reader status: ");
        count++;
      }
      if (reader.has_run_footer()) {
        reader.get_run_footer().tree_dump(std::clog, "Run footer (loaded from the dedicated reader) :");
      }
      std::clog << "Number of loaded simulated data records: " << count << std::endl;
      if (debug) reader.tree_dump(std::cerr, "Reader status: ");
    }


  } catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_simulated_data_reader_1.cxx
