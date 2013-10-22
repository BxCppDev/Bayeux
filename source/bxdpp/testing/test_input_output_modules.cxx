/* test_input_output_modules.cxx
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-08-16
 * Last modified : 2013-08-16
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

#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/things.h>
#include <datatools/bio_guard.h>

#include <dpp/input_module.h>
#include <dpp/output_module.h>
#include <dpp/simple_brio_data_source.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  std::string input_file;
  std::string output_file;
  bool test_brio = false;

  try {
      std::clog << datatools::io::notice
                << "test_input_output_modules: testing the 'dpp::input_module' class..." << std::endl;

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

      // Example of usage of the 'input_module' class (reader):
      {
        std::clog << datatools::io::notice
                  << "test_input_output_modules: 'input_module' class example: " << std::endl;

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

        // Initialize the 'input' module (without the help of a 'service manager'):
        input.initialize_standalone (input_config);
        input.tree_dump (std::clog, "Input module");

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
        output_config.store ("files.incremental.prefix", "test_input_module_");
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
            int status = input.process (ER);
            if (debug) {
              std::clog << datatools::io::debug
                        << "test_input_module: Input module processing status = " << status << std::endl;
            }
            if (status == dpp::PROCESS_FATAL) {
              std::clog << datatools::io::error
                        << "test_input_module: Reader had a fatal error !" << std::endl;
              break;
            } else if (status == dpp::PROCESS_ERROR) {
              std::clog << datatools::io::error
                        << "test_input_module: Reader had an error !" << std::endl;
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
              if (status == dpp::PROCESS_FATAL) {
                std::clog << datatools::io::error
                              << "test_input_module: Writer had a fatal error !" << std::endl;
                break;
              } else if (status == dpp::PROCESS_ERROR) {
                std::clog << datatools::io::error
                          << "test_input_module: Writer had an error !" << std::endl;
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
                  << "test_input_module: I/O modules terminated." << std::endl;

        std::clog << datatools::io::notice
                  << "test_input_module: Input event records  : " << input_count << std::endl;
        std::clog << datatools::io::notice
                  << "test_input_module: Output event records : " << output_count << std::endl;
        std::clog << datatools::io::notice
                  << "test_input_module: Error status         : " << error_code << std::endl;

      }

      // Example of usage of the 'simple_brio_data_source' class (reader):
      if (test_brio) {
        std::clog << datatools::io::notice
                  << "test_input_module: 'simple_brio_data_source' class example: " << std::endl;
#if DPP_DATATOOLS_LEGACY == 1
        dpp::simple_brio_data_source sbds ("${DPP_TESTING_DIR}/data/data_0.brio");
#else
        dpp::simple_brio_data_source sbds ("${DPP_TESTING_DIR}/legacy_data/data_0.brio");
#endif

        // generic data source interface:
        dpp::i_data_source & source = sbds;
        if (source.has_number_of_entries ()) {
          std::clog << datatools::io::notice
                    << "test_input_module: "
                    << "The number of event records stored in this file is "
                    <<  source.get_number_of_entries ()
                    << std::endl;
        }
        if (source.is_sequential ()) {
          std::clog << datatools::io::notice
                    << "test_input_module: "
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
              indent << datatools::io::notice << "test_input_module: " ;
              ER.tree_dump (std::clog, title.str (), indent.str ());
              ++count;
            } else {
              std::cerr << datatools::io::error
                        << "test_input_module: "
                        << "Couldn't load another entry."
                        << std::endl;
              error_code = EXIT_FAILURE;
            }
          }
        }
        if (source.is_random ()) {
          std::clog << datatools::io::notice
                    << "test_input_module: "
                    << "This source supports random access."
                    << std::endl;
        }
        int64_t entry = 8;
        if (source.can_load_record (entry)) {
          std::clog << datatools::io::notice
                    << "test_input_module: "
                    << "This source can load entry #" << entry << "."
                    << std::endl;
          datatools::things ER;
          source.load_record (ER, entry);
          std::ostringstream title;
          title << "Entry #" << entry;
          std::ostringstream indent;
          indent << datatools::io::notice << "test_input_module: " ;
          ER.tree_dump (std::clog, title.str (), indent.str ());
        }
      }

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: test_input_module: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "ERROR: test_input_module: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  std::cerr << "NOTICE: test_input_module: " << "THE END (" << error_code << ")" << std::endl;
  return (error_code);
}

// end of test_input_module.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
