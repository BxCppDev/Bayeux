/* test_input_output_modules.cxx
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-08-16
 * Last modified : 2013-11-01
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrates how to use
 *  input and output modules to read and write data records.
 *
 * Usage:
 *
 *  test_input_output_modules --debug
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/things.h>

// This project:
#include <dpp/input_module.h>
#include <dpp/output_module.h>
#include <dpp/simple_brio_data_source.h>

void test_io_1(bool debug, int max_events)
{
  int error_code = 0;
  // Example of usage of the 'input_module' class (reader):
  std::clog << datatools::io::notice
            << "test_io_1: 'input_module/output_module' class example: " << std::endl;

  // Instantiate a reader module :
  dpp::input_module input;

  // General setup of the 'input' module :
  datatools::properties input_config;
  input_config.store_flag ("debug");       // Debug flag
  input_config.store ("Ctx_label", "Ctx"); // The unique ID of the 'context' service (is needed)
  // Only use 1000 input files from the list of files provided below (see below) :
  input_config.store ("max_files", 1000);
  // Only load 78 event records from the full set of input files :
  input_config.store ("max_record_total", 78);
  // Only load 12 event records from each input files (then skip to another file) :
  input_config.store ("max_record_per_file", 12);
  // Configuration to build the list of filenames to be used as the
  // source of event records :
  input_config.store ("files.mode", "incremental");
#if DPP_DATATOOLS_LEGACY == 1
  input_config.store ("files.incremental.path", "${DPP_TESTING_DIR}/legacy_data");
#else
  input_config.store ("files.incremental.path", "${DPP_TESTING_DIR}/data");
#endif
  input_config.store ("files.incremental.prefix", "data_");
  input_config.store ("files.incremental.extension", "txt.gz");
  input_config.store ("files.incremental.start", 0);
  input_config.store ("files.incremental.increment", 1);
  input_config.store ("files.incremental.stop", 3);
  std::clog << datatools::io::notice << "test_io_1: TEST 1" << std::endl;

  // Initialize the 'input' module (without the help of a 'service manager'):
  input.initialize_standalone(input_config);
  std::clog << datatools::io::notice << "test_io_1: TEST 2" << std::endl;
  input.tree_dump(std::clog, "Input module");

  // Instantiate an I/O writer module :
  dpp::output_module output;

  // General setup of the 'output' module :
  datatools::properties output_config;
  //output_config.store_flag ("preserve_existing_files");
  output_config.store ("max_record_per_file", 30);
  //output_config.store ("output.max_files", 10);
  output_config.store ("max_record_total", 64);
  // Setup the list of output files:
  output_config.store ("files.mode", "incremental");
  output_config.store ("files.incremental.path", "${DPP_TMP_TEST_DIR}");
  output_config.store ("files.incremental.prefix", "test_input_output_modules_");
  output_config.store ("files.incremental.extension", ".txt");
  output_config.store ("files.incremental.start", 0);
  output_config.store ("files.incremental.increment", 1);
  output_config.store ("files.incremental.stop", 10);
  output_config.store ("Ctx_label", "Ctx");

  // Initialize the 'output' module (without the help of a 'service manager'):
  output.initialize_standalone (output_config);
  output.tree_dump (std::clog, "Output module");

  // Loop on the event records as long as the I/O input
  // module can load data from the input files :
  int input_count  = 0;
  int output_count = 0;

  // Handle input :
  while (! input.is_terminated ()) {
    // Declare the event record :
    datatools::things ER;

    // Loading next event record :
    dpp::base_module::process_status status = input.process (ER);
    if (debug) {
      std::clog << datatools::io::debug
                << "test_io_1: Input module processing status = " << status << std::endl;
    }
    if (status == dpp::base_module::PROCESS_FATAL) {
      std::clog << datatools::io::error
                << "test_io_1: Reader had a fatal error !" << std::endl;
      break;
    } else if (status == dpp::base_module::PROCESS_ERROR) {
      std::clog << datatools::io::error
                << "test_io_1: Reader had an error !" << std::endl;
      break;
    }

    // Here we add two new banks (properties) in the event record :

    datatools::properties & p = ER.add<datatools::properties> ("Info");
    p.store_flag ("test");
    p.store ("run_number", 0);
    p.store ("event_number", input_count);

    datatools::properties & p2 = ER.add<datatools::properties> ("MoreInfo");
    p2.store_flag ("simulated");
    p2.store ("author", "King Arthur");
    p2.store ("quest",  "The Search for the Holy Grail");
    p2.store ("companion", "Bevedere");
    p2.store ("favorite_colour", "Red");
    p2.store ("quote", "1, 2, 5... 3, Sire !");
    if (debug) ER.tree_dump (std::clog, "Event record :", "DEBUG: ");

    input_count++;

    // Handle output :
    if (! output.is_terminated()) {
      status = output.process (ER);
      if (status == dpp::base_module::PROCESS_FATAL) {
        std::clog << datatools::io::error
                  << "test_io_1: Writer had a fatal error !" << std::endl;
        break;
      } else if (status == dpp::base_module::PROCESS_ERROR) {
        std::clog << datatools::io::error
                  << "test_io_1: Writer had an error !" << std::endl;
        break;
      }
      output_count++;
    }

    if ((max_events > 0) && (input_count >= max_events)) {
      break;
    }

    error_code = status;
  }
  // Terminate the I/O modules :
  output.reset ();
  input.reset ();
  std::clog << datatools::io::notice
            << "test_io_1: I/O modules terminated." << std::endl;

  std::clog << datatools::io::notice
            << "test_io_1: Input event records  : " << input_count << std::endl;
  std::clog << datatools::io::notice
            << "test_io_1: Output event records : " << output_count << std::endl;
  std::clog << datatools::io::notice
            << "test_io_1: Error status         : " << error_code << std::endl;

  return;
}


void test_io_2(bool debug, int max_events)
{
  int error_code = 0;
  // Example of usage of the 'input_module' class (reader):
  std::clog << datatools::io::notice
            << "test_io_2: 'input_module/output_module' class example: " << std::endl;

  // Instantiate a reader module :
  dpp::input_module input;
  // Only load 78 event records from the full set of input files :
  // Only load 12 event records from each input files (then skip to another file) :
  // Only use 1000 input files from the list of files provided below (see below) :
  input.set_limits(78, 12, 1000);
#if DPP_DATATOOLS_LEGACY == 1
  std::string incrpath = "${DPP_TESTING_DIR}/legacy_data";
#else
  std::string incrpath = "${DPP_TESTING_DIR}/data";
#endif
  input.set_incremental_input_files(incrpath, "data_", "txt.gz", 3, 0, 1);
  input.initialize_simple();
  input.tree_dump (std::clog, "Input module");

  // Instantiate a writer module :
  dpp::output_module output;
  output.set_limits(64, 30, 10);
  output.set_incremental_output_files("${DPP_TMP_TEST_DIR}", "test_input_output_modules_", "txt", 10, 0, 1);
 // Initialize the 'output' module :
  output.initialize_simple();
  output.tree_dump (std::clog, "Output module");

  // Loop on the event records as long as the I/O input
  // module can load data from the input files :
  int input_count  = 0;
  int output_count = 0;

  // Handle input :
  while (! input.is_terminated ()) {
    // Declare the event record :
    datatools::things ER;

    // Loading next event record :
    int status = input.process (ER);
    if (debug) {
      std::clog << datatools::io::debug
                << "test_io_2: Input module processing status = " << status << std::endl;
    }
    if (status == dpp::base_module::PROCESS_FATAL) {
      std::clog << datatools::io::error
                << "test_io_2: Reader had a fatal error !" << std::endl;
      break;
    } else if (status == dpp::base_module::PROCESS_ERROR) {
      std::clog << datatools::io::error
                << "test_io_2: Reader had an error !" << std::endl;
      break;
    }

    // Here we add two new banks (properties) in the event record :

    datatools::properties & p = ER.add<datatools::properties> ("Info");
    p.store_flag ("test");
    p.store ("run_number", 0);
    p.store ("event_number", input_count);

    datatools::properties & p2 = ER.add<datatools::properties> ("MoreInfo");
    p2.store_flag ("simulated");
    p2.store ("author", "King Arthur");
    p2.store ("quest",  "The Search for the Holy Grail");
    p2.store ("companion", "Bevedere");
    p2.store ("favorite_colour", "Red");
    p2.store ("quote", "1, 2, 5... 3, Sire !");
    if (debug) ER.tree_dump (std::clog, "Event record :", "DEBUG: ");

    input_count++;

    // Handle output :
    if (! output.is_terminated()) {
      status = output.process (ER);
      if (status == dpp::base_module::PROCESS_FATAL) {
        std::clog << datatools::io::error
                  << "test_io_2: Writer had a fatal error !" << std::endl;
        break;
      } else if (status == dpp::base_module::PROCESS_ERROR) {
        std::clog << datatools::io::error
                  << "test_io_2: Writer had an error !" << std::endl;
        break;
      }
      output_count++;
    }

    if ((max_events > 0) && (input_count >= max_events)) {
      break;
    }

    error_code = status;
  }
  // Terminate the I/O modules :
  output.reset ();
  input.reset ();
  std::clog << datatools::io::notice
            << "test_io_2: I/O modules terminated." << std::endl;

  std::clog << datatools::io::notice
            << "test_io_2: Input event records  : " << input_count << std::endl;
  std::clog << datatools::io::notice
            << "test_io_2: Output event records : " << output_count << std::endl;
  std::clog << datatools::io::notice
            << "test_io_2: Error status         : " << error_code << std::endl;

  return;
}

void test_brio_1(bool /*debug*/)
{
  int error_code = 0;
  // Example of usage of the 'simple_brio_data_source' class (reader):
  std::clog << datatools::io::notice
            << "test_brio_1: 'simple_brio_data_source' class example: " << std::endl;
#if DPP_DATATOOLS_LEGACY == 1
  dpp::simple_brio_data_source sbds ("${DPP_TESTING_DIR}/data/data_0.brio");
#else
  dpp::simple_brio_data_source sbds ("${DPP_TESTING_DIR}/legacy_data/data_0.brio");
#endif

  // generic data source interface:
  dpp::i_data_source & source = sbds;
  if (source.has_number_of_entries ()) {
    std::clog << datatools::io::notice
              << "test_input_output_modules: "
              << "The number of event records stored in this file is "
              <<  source.get_number_of_entries ()
              << std::endl;
  }
  if (source.is_sequential ()) {
    std::clog << datatools::io::notice
              << "test_input_output_modules: "
              << "This source supports sequential access."
              << std::endl;
    int count = 0;
    while (source.has_next_record ()) {
      datatools::things ER;
      bool ok = source.load_next_record (ER);
      if (ok) {
        source.load_next_record (ER);
        std::ostringstream title;
        title << "Entry #" << count;
        std::ostringstream indent;
        indent << datatools::io::notice << "test_input_output_modules: " ;
        ER.tree_dump (std::clog, title.str (), indent.str ());
        ++count;
      } else {
        std::cerr << datatools::io::error
                  << "test_brio_1: "
                  << "Couldn't load another entry."
                  << std::endl;
        error_code = EXIT_FAILURE;
      }
    }
  }
  if (source.is_random ()) {
    std::clog << datatools::io::notice
              << "test_brio_1: "
              << "This source supports random access."
              << std::endl;
  }
  int64_t entry = 8;
  if (source.can_load_record (entry)) {
    std::clog << datatools::io::notice
              << "test_brio_1: "
              << "This source can load entry #" << entry << "."
              << std::endl;
    datatools::things ER;
    source.load_record (ER, entry);
    std::ostringstream title;
    title << "Entry #" << entry;
    std::ostringstream indent;
    indent << datatools::io::notice << "test_brio_1: " ;
    ER.tree_dump (std::clog, title.str (), indent.str ());
  }
  std::clog << datatools::io::notice
            << "test_brio_1: Error status         : " << error_code << std::endl;
  return;
}

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  std::string input_file;
  std::string output_file;
  bool test_brio = false;

  try {
    std::clog << datatools::io::notice
              << "test_input_output_modules: testing the 'dpp::input/ouput_module' classes." << std::endl;

    bool debug = false;
    int max_events = 0;

    int iarg = 1;
    while (iarg < argc_){
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if (option == "-10") {
          max_events = 10;
        } else if (option == "-100") {
          max_events = 100;
        } else if (option == "--test-brio") {
          test_brio = true;
        } else {
          std::clog << datatools::io::warning
                    << "test_input_output_modules: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        std::clog << datatools::io::warning
                  << "test_input_output_modules: ignoring argument '" << argument << "'!" << std::endl;
      }
      iarg++;
    }

    test_io_1(debug, max_events);

    test_io_2(debug, max_events);

    if (test_brio) {
      //test_brio_1(debug);
    }

    {
      std::string fout = "${DPP_TMP_TEST_DIR}/test_input_output_modules_0.txt";
      datatools::fetch_path_with_env(fout);
      unlink(fout.c_str());
    }

    {
      std::string fout = "${DPP_TMP_TEST_DIR}/test_input_output_modules_1.txt";
      datatools::fetch_path_with_env(fout);
      unlink(fout.c_str());
    }

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: test_input_output_modules: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "ERROR: test_input_output_modules: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  std::cerr << "NOTICE: test_input_output_modules: " << "THE END (" << error_code << ")" << std::endl;
  return (error_code);
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
