/* test_module_chain.cxx
 * Author (s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-25
 * Last modified : 2013-02-15
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A program that performs event record processing.
 *
 * Usage:
 *
 * test_module_chain \
 *   --debug 1 \
 *   --module-name chain1 \
 *   -c ${DPP_TESTING_DIR}/config/test_module_manager.conf   \
 *   -i ${DPP_TESTING_DIR}/data/data_0.txt.gz \
 *   -o out.txt.gz
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/things.h>
#include <datatools/utils.h>
#include <datatools/base_service.h>
#include <datatools/library_loader.h>

// This project:
#include <dpp/simple_data_source.h>
#include <dpp/simple_data_sink.h>
#include <dpp/base_module.h>
#include <dpp/dummy_module.h>
#include <dpp/module_manager.h>

namespace po = boost::program_options;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
      std::clog << "The event record processing test program." << std::endl;

      bool   debug;
      bool   verbose;
      int    max_number_of_events;
      std::string module_name;
      std::string mgr_config;
      std::vector<std::string> input_files;
      std::string output_file;
      int    print_modulo;
      std::string LL_config;
      std::vector<std::string> LL_dlls;

      po::options_description opts ("Allowed options");
      opts.add_options ()
        ("help,h", "produce help message")
        ("debug,d",
         po::value<bool> (&debug)->default_value (false),
         "produce debug message")
        ("verbose,v",
         po::value<bool> (&verbose)->default_value (false),
         "produce verbose message")
        ("modulo,m",
         po::value<int> (&print_modulo)->default_value (10),
         "set the modulo event print period")
        ("max-number-of-events,n",
         po::value<int> (&max_number_of_events)->default_value (-1),
         "maximum number of processed events")
        ("module-name,m",
         po::value<std::string> (),
         "set the name of the data processing module")
        ("manager-config,c",
         po::value<std::string> (),
         "set the module manager configuration file")
        ("input-file,i",
         po::value< std::vector<std::string> > (),
         "set an input file")
        ("output-file,o",
         po::value<std::string> (),
         "set the output file")
        ("load-dll,l",
         po::value<std::vector<std::string> > (),
         "set a DLL to be loaded.")
        ;

      po::positional_options_description args;
      args.add ("input-file", -1);

      po::variables_map vm;
      po::store (po::command_line_parser (argc_, argv_)
                 .options (opts)
                 .positional (args)
                 .run (),
                 vm);
      po::notify (vm);

      if (vm.count ("load-dll"))
        {
          LL_dlls = vm["load-dll"].as< std::vector<std::string> > ();
        }

      datatools::library_loader LL (LL_config);
      BOOST_FOREACH (const std::string & dll_name, LL_dlls)
        {
          std::clog << datatools::io::notice << "Loading DLL '"
                    << dll_name << "'." << std::endl;
          if (LL.load (dll_name) != EXIT_SUCCESS)
            {
              std::ostringstream message;
              message << "Loading DLL '" << dll_name
                      << "' fails !";
              throw std::logic_error (message.str ());
            }
        }

      if (vm.count ("help"))
        {
          std::cout << opts << "\n";
          return 1;
        }

      if (vm.count ("max-events"))
        {
          max_number_of_events = vm["max-events"].as<int> ();
          std::clog << "Maximum number of processed events set to "
                    << max_number_of_events << ".\n";
        }

      if (vm.count("input-file")) {
        input_files = vm["input-file"].as< std::vector<std::string> > ();
        std::clog << "Input files are : \n";
        BOOST_FOREACH (const std::string & file_name, input_files)
          {
            std::clog << " Input file : '" << file_name << "'\n.";
          }
      } else {
        //throw std::logic_error ("Missing input file(s) !");
        input_files.push_back("${DPP_TESTING_DIR}/data/data_0.txt.gz");
        input_files.push_back("${DPP_TESTING_DIR}/data/data_1.txt.gz");
        input_files.push_back("${DPP_TESTING_DIR}/data/data_2.txt.gz");
        input_files.push_back("${DPP_TESTING_DIR}/data/data_3.txt.gz");
      }


      if (vm.count("output-file")) {
        output_file = vm["output-file"].as<std::string> ();
        std::clog << "Output file is : " << output_file << ".\n";
      } else {
        output_file = "${DPP_TMP_TEST_DIR}/test_module_manager_out.txt";
      }

      if (vm.count ("module-name"))
        {
          module_name = vm["module-name"].as<std::string> ();
          std::clog << datatools::io::notice << "Using processor module '"
                    << module_name << "'." << std::endl;
        }
      else
        {
          // std::ostringstream message;
          // message << "Missing module name !";
          // throw std::logic_error (message.str ());
          module_name = "chain0";
        }

      if (vm.count ("manager-config"))
        {
          mgr_config = vm["manager-config"].as<std::string> ();
          std::clog << datatools::io::notice << "Using module manager configuration file '"
                    << mgr_config << "'." << std::endl;
        }
      else
        {
          // std::ostringstream message;
          // message << "Missing module manager configuration file !";
          // throw std::logic_error (message.str ());
          mgr_config = "${DPP_TESTING_DIR}/config/test_module_manager.conf";
        }

      if (print_modulo < 0)
        {
          print_modulo = 0;
        }

      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (datatools::base_service).print (std::clog);

      // Module manager :
      uint32_t flags = dpp::module_manager::BLANK;
      dpp::module_manager MM (flags);

      std::cerr << "****** DEVEL ***** Go* " << std::endl;

      if (verbose) {
        MM.set_logging_priority(datatools::logger::PRIO_NOTICE);
      }
      if (debug) {
        MM.set_logging_priority(datatools::logger::PRIO_DEBUG);
      }

      // Load properties from the configuration file:
      if (mgr_config.empty ())
        {
          std::ostringstream message;
          message << "Missing module manager configuration file !";
          throw std::logic_error (message.str ());
        }
      std::string MM_config_file = mgr_config;
      datatools::fetch_path_with_env (MM_config_file);
      std::clog << "Manager config. file : '" << MM_config_file << "'" << std::endl;

      datatools::properties MM_config;
      datatools::read_config (MM_config_file, MM_config);
      MM.initialize (MM_config);
      if (debug) MM.tree_dump (std::clog, "Module manager (initialized) : ", "DEBUG: ");

      std::clog << "Module to be used : '" << module_name << "'" << std::endl;
      if (! MM.has (module_name))
        {
          std::ostringstream message;
          message << "Manager has no module named '" << module_name << "' !";
          throw std::logic_error (message.str ());
        }
      else
        {
          std::clog << "Found module named '" << module_name << "' !" << std::endl;
        }
      dpp::base_module & the_module = MM.grab (module_name);
      if (debug) the_module.tree_dump (std::clog, "The active module : ", "DEBUG: ");

      // Setup the data output sink :
      boost::scoped_ptr<dpp::simple_data_sink> sink;
      if (! output_file.empty ())
        {
          sink.reset (new dpp::simple_data_sink (output_file));
        }

      // Loop on the data source files :
      int data_counter = 0;
      BOOST_FOREACH (const std::string & source_label, input_files)
        {
          if (debug) std::clog << datatools::io::notice
                               << "Using data source '" << source_label << "'..." << std::endl;
          datatools::logger::priority priority = datatools::logger::PRIO_NOTICE;
          if (debug)
            {
              priority = datatools::logger::PRIO_DEBUG;
            }
          dpp::simple_data_source source (source_label, priority);

          // Loop on the event records from the data source file :
          if (debug) std::clog << datatools::io::debug
                               << "Entering event record loop..." << std::endl;
          while (source.has_next_record ())
            {
              datatools::things DR;
              if (debug) std::clog << datatools::io::debug
                                   << "Loading next event record..." << std::endl;
              source.load_next_record (DR);
              if (debug) std::clog << datatools::io::debug
                                   << "Processing the event record..." << std::endl;
              dpp::base_module::process_status status = the_module.process (DR);
              if (status & dpp::base_module::PROCESS_ERROR)
                {
                  std::cerr << datatools::io::error
                            << "Error at processing event record #"
                            <<  data_counter << std::endl;
                }
              if (debug) DR.tree_dump (std::clog, "Event record :", "DEBUG: ");
              if (sink)
                {
                  sink->store_next_record (DR);
                }

              if ((print_modulo > 0) &&  (data_counter % print_modulo == 0))
                {
                  std::clog << datatools::io::notice << "Event #" << data_counter << "\n";
                }
              data_counter++;
              if ( (max_number_of_events > 0) && (data_counter == max_number_of_events))
                {
                  break;
                }
            }
          if (debug) std::clog << datatools::io::debug
                               << "Exiting event record loop." << std::endl;
        } // BOOST_FOREACH

      // Terminate the module manager :
      MM.reset ();

   }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
