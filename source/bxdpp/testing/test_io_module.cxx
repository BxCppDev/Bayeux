/* test_io_module.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-15
 * Last modified : 2013-02-17
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrates how to use a
 *  I/O module to read/write event record.
 *
 * Usage:
 *
 *  test_io_module --debug
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

#include <dpp/io_module.h>
#include <dpp/simple_brio_data_source.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  std::string input_file;
  std::string output_file;
  bool test_brio = false;

  try
    {
      std::clog << datatools::io::notice
                << "test_io_module: testing the 'dpp::io_module' class..." << std::endl;

      bool debug = false;
      int max_events = 0;

      int iarg = 1;
      while (iarg < argc_)
        {
          std::string token = argv_[iarg];

          if (token[0] == '-')
            {
              std::string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  debug = true;
                }
              else if (option == "-10")
                {
                  max_events = 10;
                }
              else if (option == "-100")
                {
                  max_events = 100;
                }
              else if (option == "--test-brio")
                {
                  test_brio = true;
                }
              else
                {
                  std::clog << datatools::io::warning
                            << "test_io_module: ignoring option '" << option << "'!" << std::endl;
                }
            }
          else
            {
              std::string argument = token;
              std::clog << datatools::io::warning
                        << "test_io_module: ignoring argument '" << argument << "'!" << std::endl;
            }
          iarg++;
        }

      // Example of usage of the 'io_module' class (reader):
      {
        std::clog << datatools::io::notice
                  << "test_io_module: 'io_module' class example: " << std::endl;
        // Instantiate an I/O reader module :
        dpp::io_module input;

        // Set the 'input' mode (reader) :
        input.set_rw_mode (dpp::io_module::RW_MODE_INPUT);

        // Only use 1000 input files from the list of files provided below (see below) :
        input.set_max_files (1000);

        // Only load 78 event records from the full set of input files :
        input.set_max_record_total (78);

        // Only load 12 event records from each input files (then skip to another file) :
        input.set_max_record_per_file (12);

        // Configuration to build the list of filenames to be used as the source of event records :
        datatools::properties input_files_config;
        input_files_config.store ("mode", "incremental");
#if DPP_DATATOOLS_LEGACY == 1
        input_files_config.store ("incremental.path", "${DPP_DATA_DIR}/testing/legacy_data");
#else
        input_files_config.store ("incremental.path", "${DPP_DATA_DIR}/testing/data");
#endif
        input_files_config.store ("incremental.prefix", "data_");
        input_files_config.store ("incremental.extension", "txt.gz");
        input_files_config.store ("incremental.start", 0);
        input_files_config.store ("incremental.increment", 1);
        input_files_config.store ("incremental.stop", 3);

        // Build the list of input filenames :
        input.set_filenames (input_files_config);

        // General setup of the 'input' module :
        datatools::properties input_config;
        input_config.store_flag ("debug");       // Debug flag
        input_config.store ("Ctx_label", "Ctx"); // The unique ID of the 'context' service (is needed)

        // Alternative to the above 'input.set_XXX' methods:
        // we can set more properties to configure the 'input' module :
        /*
          input_config.store ("mode", "input");
          input_config.store ("input.max_files", 1000);
          input_config.store ("input.max_record_total", 157);
          input_config.store ("input.max_record_per_file", 12);
          input_config.store_flag ("input.plain_simulated", false);
          // Setup the list of input files:
          input_config.store ("input.mode", "incremental");
          input_config.store ("input.incremental.path", "${DPP_DATA_DIR}/testing/data");
          input_config.store ("input.incremental.prefix", "data_");
          input_config.store ("input.incremental.extension", "txt.gz");
          input_config.store ("input.incremental.start", 0);
          input_config.store ("input.incremental.increment", 1);
          input_config.store ("input.incremental.stop", 3);
        */

        // Initialize the 'input' module (without the help of a 'service manager'):
        input.initialize_standalone (input_config);
        input.tree_dump (std::clog, "Input module");

        // Instantiate an I/O writer module :
        dpp::io_module output;

        // General setup of the 'output' module :
        datatools::properties output_config;
        output_config.store ("mode", "output");
        //output_config.store_flag ("output.preserve_existing");
        output_config.store ("output.max_record_per_file", 30);
        //output_config.store ("output.max_files", 10);
        output_config.store ("output.max_record_total", 64);
        // Setup the list of output files:
        output_config.store ("output.mode", "incremental");
        output_config.store ("output.incremental.path", "${DPP_TMP_TEST_DIR}");
        output_config.store ("output.incremental.prefix", "test_io_module_");
        output_config.store ("output.incremental.extension", ".txt");
        output_config.store ("output.incremental.start", 0);
        output_config.store ("output.incremental.increment", 1);
        output_config.store ("output.incremental.stop", 10);
        output_config.store ("Ctx_label", "Ctx");

        // Initialize the 'output' module (without the help of a 'service manager'):
        output.initialize_standalone (output_config);
        output.tree_dump (std::clog, "Output module");

        // Loop on the event records as long as the I/O input
        // module can load data from the input files :
        int input_count  = 0;
        int output_count = 0;

        // Handle input :
        while (! input.is_terminated ())
          {
            // Declare the event record :
            datatools::things ER;

            // Loading next event record :
            int status = input.process (ER);
            if (debug)
              {
                std::clog << datatools::io::debug
                          << "test_io_module: Input module processing status = " << status << std::endl;
              }
            if (status == dpp::PROCESS_FATAL)
              {
                std::clog << datatools::io::error
                          << "test_io_module: Reader had a fatal error !" << std::endl;
                break;
              }
            else if (status == dpp::PROCESS_ERROR)
              {
                std::clog << datatools::io::error
                          << "test_io_module: Reader had an error !" << std::endl;
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
            if (! output.is_terminated())
              {
                status = output.process (ER);
                if (status == dpp::PROCESS_FATAL)
                  {
                    std::clog << datatools::io::error
                              << "test_io_module: Writer had a fatal error !" << std::endl;
                    break;
                  }
                else if (status == dpp::PROCESS_ERROR)
                  {
                    std::clog << datatools::io::error
                              << "test_io_module: Writer had an error !" << std::endl;
                    break;
                  }
                output_count++;
              }

            if ((max_events > 0) && (input_count >= max_events))
              {
                break;
              }

            error_code = status;
          }
        // Terminate the I/O modules :
        output.reset ();
        input.reset ();
        std::clog << datatools::io::notice
                  << "test_io_module: I/O modules terminated." << std::endl;

        std::clog << datatools::io::notice
                  << "test_io_module: Input event records  : " << input_count << std::endl;
        std::clog << datatools::io::notice
                  << "test_io_module: Output event records : " << output_count << std::endl;
        std::clog << datatools::io::notice
                  << "test_io_module: Error status         : " << error_code << std::endl;

      }

      // Example of usage of the 'simple_brio_data_source' class (reader):
      if (test_brio)
        {
        std::clog << datatools::io::notice
                  << "test_io_module: 'simple_brio_data_source' class example: " << std::endl;
#if DPP_DATATOOLS_LEGACY == 1
        dpp::simple_brio_data_source sbds ("${DPP_DATA_DIR}/testing/data/data_0.brio");
#else
        dpp::simple_brio_data_source sbds ("${DPP_DATA_DIR}/testing/legacy_data/data_0.brio");
#endif

        // generic data source interface:
        dpp::i_data_source & source = sbds;
        if (source.has_number_of_entries ())
          {
            std::clog << datatools::io::notice
                      << "test_io_module: "
                      << "The number of event records stored in this file is "
                      <<  source.get_number_of_entries ()
                      << std::endl;
          }
        if (source.is_sequential ())
          {
            std::clog << datatools::io::notice
                      << "test_io_module: "
                      << "This source supports sequential access."
                      << std::endl;
            int count = 0;
            while (source.has_next_record ())
              {
                datatools::things ER;
                bool ok = source.load_next_record (ER);
                if (ok)
                  {
                    source.load_next_record (ER);
                    std::ostringstream title;
                    title << "Entry #" << count;
                    std::ostringstream indent;
                    indent << datatools::io::notice << "test_io_module: " ;
                    ER.tree_dump (std::clog, title.str (), indent.str ());
                    ++count;
                  }
                else
                  {
                    std::cerr << datatools::io::error
                              << "test_io_module: "
                              << "Couldn't load another entry."
                              << std::endl;
                    error_code = EXIT_FAILURE;
                  }
              }
          }
        if (source.is_random ())
          {
            std::clog << datatools::io::notice
                      << "test_io_module: "
                      << "This source supports random access."
                      << std::endl;
          }
        int64_t entry = 8;
        if (source.can_load_record (entry))
          {
            std::clog << datatools::io::notice
                      << "test_io_module: "
                      << "This source can load entry #" << entry << "."
                      << std::endl;
            datatools::things ER;
            source.load_record (ER, entry);
            std::ostringstream title;
            title << "Entry #" << entry;
            std::ostringstream indent;
            indent << datatools::io::notice << "test_io_module: " ;
            ER.tree_dump (std::clog, title.str (), indent.str ());
          }
      }

    }
  catch (std::exception & x)
    {
      std::cerr << "ERROR: test_io_module: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "ERROR: test_io_module: " << "unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  std::cerr << "NOTICE: test_io_module: " << "THE END (" << error_code << ")" << std::endl;
  return (error_code);
}

// end of test_io_module.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
