/* dpp_processing.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-03
 * Last modified : 2012-11-13
 *
 * Copyright (C) 2011-2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A generic program that performs processing of SuperNEMO event record along a pipeline.
 *
 * Using :
 *
 *  bash$ export DPP_HEADER_RUN_NUMBER=<run number>
 *
 * before running :
 *
 *  bash$ dpp_processing                                          \
 *          -c ${SNCORE_DATA_DIR}/testing/config/test_module_manager.conf      \
 *          -i ${SNCORE_DATA_DIR}/testing/data/data_0.txt.gz \
 *          -m clear -m add_header -m chain1 -m dump_in_file                     \
 *          -o processed.xml
 *
 * will set the requested run number in the 'event header'.
 *
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>

#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/things.h>
#include <datatools/utils.h>
#include <datatools/library_loader.h>
#include <datatools/bio_guard.h>

#include <mygsl/bio_guard.h>

#include <dpp/dpp_config.h>
#include <dpp/base_module.h>
#include <dpp/io_module.h>
#include <dpp/module_manager.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  const std::string APP_NAME_PREFIX = "dpp_processing: ";

  try
    {
      // The dpp_processing event record processing program.
      bool   debug = false;
      bool   verbose = false;
      bool   break_on_error_as_fatal = false;
      int    print_modulo = 10;
      //bool   simulated_input = false;
      std::string module_manager_config_file;
      std::string module_name;
      std::vector<std::string> module_names;
      std::string LL_config;
      std::vector<std::string> LL_dlls;
      std::vector<std::string> input_files;
      std::vector<std::string> output_files;
      bool   no_max_events = false;
      int    max_events = 0;
      int    max_events_per_output_file = 0;
      bool   save_stopped_event_records = false;

      // Shortcut for Boost/program_options namespace :
      // See: http://www.boost.org/doc/libs/1_46_1/doc/html/program_options.html
      namespace po = boost::program_options;

      // Describe command line switches (-X, --XXX...) :
      po::options_description opts ("Allowed options");

      opts.add_options ()
        ("help,h", "produce help message.")
        ("debug,d",
         po::value<bool>(&debug)->zero_tokens()->default_value (false),
         "produce debug logging.")
        ("verbose,v",
         po::value<bool>(&verbose)->zero_tokens()->default_value (false),
         "produce verbose logging.")
        ("load-dll,l",
         po::value<std::vector<std::string> > (),
         "set a DLL to be loaded.")
        ("dlls-config,L",
         po::value<std::string> (),
         "set the DLL loader configuration file.")
        ("modulo,%",
         po::value<int> (&print_modulo)->default_value (10),
         "set the modulo event print period.")
        ("max-events,M",
         po::value<int> (&max_events)->default_value (0),
         "set the maximum number of events to be processed.")
        ("no-max-events,X",
         po::value<bool>(&no_max_events)->zero_tokens()->default_value (false),
         "Do not limit the maximum number of events to be processed.")
        ("module,m",
         po::value<std::vector<std::string> > (),
         "add a module in the pipeline (optionnal).")
        ("module-manager-config,c",
         po::value<std::string> (),
         "set the module manager configuration file.")
        ("input-file,i",
         po::value<std::vector<std::string> > (),
         "set an input file (optional).")
        // ("simulated-input,S",
        //  po::value<bool> (&simulated_input)->zero_tokens()->default_value (false),
        //  "use the plain simulated data format input (from sng4).")
        ("output-file,o",
         po::value<std::vector<std::string> > (),
         "set the output file (optional).")
        ("max-events-per-output-file,O",
         po::value<int> (&max_events_per_output_file)->default_value (0),
         "set the maximum number of event records per output file.")
        // ("save-stopped-events,s",
        //  po::value<bool>(&save_stopped_event_records)->zero_tokens()->default_value (false),
        //  "produce debug logging.")
        ;

      // Describe command line arguments :
      po::positional_options_description args;
      args.add ("input-file", -1);

      po::variables_map vm;
      try
        {
          po::store (po::command_line_parser (argc_, argv_)
                     .options (opts)
                     .positional (args)
                     .run (), vm);
          po::notify (vm);
        }
      catch (std::exception & x)
        {
          std::ostringstream message ;
          message << "Command line parser error : " << x.what ();
          throw std::logic_error (message.str ());
        }

      /*** begin of opts/args parsing ***/

      // Fetch the opts/args :
      if (vm.count ("help"))
        {
          std::cout << "dpp_processing -- A generic data chain processing program" << std::endl;
          std::cout << std::endl;
          std::cout << "Usage : " << std::endl;
          std::cout << std::endl;
          std::cout << "  dpp_processing [OPTIONS] [ARGUMENTS] " << std::endl;
          std::cout << std::endl;
          std::cout << opts << std::endl;
          std::cout << std::endl;
          std::cout << "Examples : " << std::endl;
          std::cout << std::endl;
          std::cout << "  Process the chain of 'my_moduleX' (X=1,2,3) data processing modules from the 'mydllY' libraries (Y=1,2): " << std::endl;
          std::cout << std::endl;
          std::cout << "  bash$ dpp_processing              \\" << std::endl;
          std::cout << "          -c my_module_manager.conf   \\" << std::endl;
          std::cout << "          -l my_dll1                  \\" << std::endl;
          std::cout << "          -l my_dll2                  \\" << std::endl;
          std::cout << "          -i my_input_events_0.txt.gz \\" << std::endl;
          std::cout << "          -i my_input_events_1.txt.gz \\" << std::endl;
          std::cout << "          -i my_input_events_2.txt.gz \\" << std::endl;
          std::cout << "          -m my_module1               \\" << std::endl;
          std::cout << "          -m my_module2               \\" << std::endl;
          std::cout << "          -m my_module3               \\" << std::endl;
          std::cout << "          -o my_output_events.txt.gz    " << std::endl;
          std::cout << std::endl;
          std::cout << "  Simple use cases : " << std::endl;
          std::cout << std::endl;
          std::cout << "  bash$ dpp_processing   \\" << std::endl;
          std::cout << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
          std::cout << "          -i ${DPP_DATA_DIR}/testing/data/data_0.txt.gz     \\" << std::endl;
          std::cout << "          -m clear         \\" << std::endl;
          std::cout << "          -m add_header    \\" << std::endl;
          std::cout << "          -m chain1        \\" << std::endl;
          std::cout << "          -o processed.xml   " << std::endl;
          std::cout << std::endl;
          std::cout << "  bash$ dpp_processing   \\" << std::endl;
          std::cout << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
          std::cout << "          -M 10            \\" << std::endl;
          std::cout << "          -m add_header    \\" << std::endl;
          std::cout << "          -m chain1        \\" << std::endl;
          std::cout << "          -m dump_in_file    " << std::endl;
          std::cout << std::endl;
          std::cout << "  bash$ dpp_processing   \\" << std::endl;
          std::cout << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
          std::cout << "          -M 10            \\" << std::endl;
          std::cout << "          -m add_header    \\" << std::endl;
          std::cout << "          -m chain1        \\" << std::endl;
          std::cout << "          -o processed.xml   " << std::endl;
          std::cout << std::endl;
          return 1;
        }

      if (vm.count ("load-dll"))
        {
          LL_dlls = vm["load-dll"].as< std::vector<std::string> > ();
        }

      if (vm.count ("dlls-config"))
        {
          LL_config = vm["dlls-config"].as<std::string> ();
        }

      if (vm.count ("module-manager-config"))
        {
          module_manager_config_file = vm["module-manager-config"].as<std::string> ();
        }
      else
        {
          std::clog << datatools::io::warning << APP_NAME_PREFIX << "No module manager configuration file !\n";
        }

      if (vm.count ("module"))
        {
          //module_name = vm["module"].as<string> ();
          module_names = vm["module"].as< std::vector<std::string> > ();
        }
      else
        {
          std::clog << datatools::io::warning << APP_NAME_PREFIX << "Missing processing module name !\n";
        }

      if (vm.count ("input-file"))
        {
          input_files = vm["input-file"].as< std::vector<std::string> > ();
        }

      if (vm.count ("output-file"))
        {
          output_files = vm["output-file"].as< std::vector<std::string> > ();
        }
      /*
        else
        {
        std::clog << io::warning << APP_NAME_PREFIX << "No output file !\n";
        }
      */

      if (input_files.size () > 0)
        {
          std::clog << datatools::io::notice << APP_NAME_PREFIX << "Input data sources : "
                    << input_files.size () << std::endl;
          for (std::vector<std::string>::const_iterator i = input_files.begin ();
               i != input_files.end ();
               i++)
            {
              std::clog << datatools::io::notice << APP_NAME_PREFIX << " - source : '"
                        << *i << "'" << std::endl;
            }
        }
      else
        {
          std::clog << datatools::io::notice << APP_NAME_PREFIX << "No input data source !" << std::endl;
          if (max_events <= 0)
            {
              // This is only an error if we decide to enforce the use of
              // a limit on the maximum number of event records :
              if (! no_max_events)
                {
                  std::ostringstream message;
                  message << "Missing maximum number of events !";
                  throw std::logic_error (message.str ());
                }
            }
        }

      if (output_files.size () > 0)
        {
          std::clog << datatools::io::notice << APP_NAME_PREFIX << "Output data sinks : "
                    << output_files.size () << std::endl;
          for (std::vector<std::string>::const_iterator i = output_files.begin ();
               i != output_files.end ();
               i++)
            {
              std::clog << datatools::io::notice << APP_NAME_PREFIX << " - sink : '"
                        << *i << "'" << std::endl;
            }
        }
      else
        {
          std::clog << datatools::io::notice << APP_NAME_PREFIX << "No output data sink !" << std::endl;
        }

      /*** end of opts/args parsing ***/

      uint32_t LL_flags = datatools::library_loader::allow_unregistered;
      datatools::library_loader LL (LL_flags, LL_config);
      BOOST_FOREACH (const std::string & dll_name, LL_dlls)
        {
          std::clog << datatools::io::notice << APP_NAME_PREFIX << "Loading DLL '"
                    << dll_name << "'." << std::endl;
          if (LL.load (dll_name) != EXIT_SUCCESS)
            {
              std::ostringstream message;
              message << "Loading DLL '" << dll_name
                      << "' fails !";
              throw std::logic_error (message.str ());
            }
        }

      /*
        if (module_names.empty ())
        {
        std::ostringstream message;
        message << "Missing module name(s) !";
        throw std::logic_error (message.str ());
        }
      */
      BOOST_FOREACH (const std::string & module_name, module_names)
        {
          std::clog << datatools::io::notice << APP_NAME_PREFIX << "Using data processing module '"
                    << module_name << "'." << std::endl;
        }

      // Module manager :
      uint32_t flags = dpp::module_manager::BLANK;
      if (debug)
        {
          flags |= dpp::module_manager::DEBUG;
          flags |= dpp::module_manager::FACTORY_DEBUG;
        }
      if (verbose)
        {
          flags |= dpp::module_manager::VERBOSE;
        }
      if (module_manager_config_file.empty ())
        {
        }

      // Load properties from the configuration file:
      if (module_names.empty () && module_manager_config_file.empty ())
        {
          std::ostringstream message;
          message << "Missing module manager configuration file !";
          throw std::logic_error (message.str ());
        }

      // XXX
      //module_manager MM (flags);
      boost::scoped_ptr<dpp::module_manager> MM;
      if ( ! module_manager_config_file.empty ())
        {
          MM.reset (new dpp::module_manager (flags));
          std::string MM_config_file = module_manager_config_file;
          
          datatools::fetch_path_with_env (MM_config_file);
          std::clog << datatools::io::notice
                    << APP_NAME_PREFIX << "Manager config. file : '" << MM_config_file << "'" << std::endl;
          
          datatools::properties MM_config;
          datatools::properties::read_config (MM_config_file,
                                       MM_config);
          MM.get ()->initialize (MM_config);
          MM.get ()->tree_dump (std::clog, "Module manager (initialized) : ", "NOTICE: ");
        }
 
      std::clog << datatools::io::notice << APP_NAME_PREFIX << "Module to be used : '" << module_name << "'" << std::endl;
      std::vector<dpp::base_module*> modules;
 
      for (int i = 0; i < module_names.size (); i++)
        {
          const std::string & module_name = module_names[i];
          if (! MM.get ()->has (module_name))
            {
              std::ostringstream message;
              message << "Manager has no module named '" << module_name << "' !";
              throw std::logic_error (message.str ());
            }
          else
            {
              std::clog << datatools::io::notice
                        << APP_NAME_PREFIX << "Found module named '" << module_name << "' !" << std::endl;
            }
          dpp::base_module & the_module = MM.get ()->grab (module_name);
          modules.push_back(&the_module);
          the_module.tree_dump (std::clog, "Added module : ", "NOTICE: " + APP_NAME_PREFIX);

        }

      // Setup the data output sink :
      boost::scoped_ptr<dpp::io_module> sink;
      if (output_files.size () > 0)
        {
          sink.reset (new dpp::io_module);
          datatools::properties sink_config;
          sink_config.store ("mode", "output");
          sink_config.store ("output.mode", "list");
          sink_config.store ("output.list.filenames", output_files);
          if (max_events_per_output_file > 0)
            {
              sink_config.store ("output.max_record_per_file", max_events_per_output_file);
            }
          sink->initialize_with_service (sink_config, MM.get ()->grab_service_manager ());
        }

      // Setup the data input source :
      boost::scoped_ptr<dpp::io_module> source;
      if (input_files.size () > 0)
        {
          source.reset (new dpp::io_module);
          datatools::properties source_config;
          source_config.store ("mode", "input");
          source_config.store ("input.mode", "list");
          source_config.store ("input.list.filenames", input_files);
          // if (simulated_input)
          //   {
          //     source_config.store_flag ("input.plain_simulated");
          //   }
          source->initialize_with_service (source_config, MM.get ()->grab_service_manager ());
        }

      // Loop on the event records from the data source file :
      if (debug) std::clog << datatools::io::debug << APP_NAME_PREFIX
                           << "Entering event record loop..." << std::endl;

      
      //std::list< datatools::handle<scm::event_record> > ER_pool;

      // Loop on the event records :
      int event_counter = 0;
      int stored_counter = 0;
      while (true)
        {
          bool do_break_event_loop = false;
          if (debug) std::clog << datatools::io::debug << APP_NAME_PREFIX
                               << "Instantiating an event record object..." << std::endl;

          // Instantiate a blank event record object :
          datatools::things ER;

          // Manage the source is any (fill the event record from it) :
          if (source)
            {
              if (source->is_terminated ())
                {
                  break;
                }
              int input_status = source->process (ER);
              if (input_status != dpp::io_module::OK)
                {
                  std::clog << datatools::io::error
                            << APP_NAME_PREFIX << "Source of event records had an error !" << std::endl;
                  break;
                }
            } // end of if (source)

          if ((print_modulo > 0) &&  (event_counter % print_modulo == 0))
            {
              std::clog << datatools::io::notice << APP_NAME_PREFIX << "Event #" << event_counter << "\n";
            }

          // Process the event using the choosen processing module :
          if (debug) std::clog << datatools::io::debug << APP_NAME_PREFIX
                               << "Processing the event record..." << std::endl;
          int processing_status = dpp::base_module::SUCCESS;
          try
            {
              BOOST_FOREACH (dpp::base_module * active_module_ptr, modules)
                {
                  dpp::base_module & the_active_module = *active_module_ptr;
                  processing_status = the_active_module.process (ER);
                  if (debug) std::clog << datatools::io::debug << APP_NAME_PREFIX
                                       << "processing_status==" << processing_status << std::endl;
                  if (processing_status & dpp::base_module::FATAL)
                    {
                      // A fatal error has been met, we break the processing loop :
                      std::clog << datatools::io::error << APP_NAME_PREFIX
                                << "Processing of event record #" << event_counter << " met a fatal error. Break !" << std::endl;
                      do_break_event_loop = true;
                      error_code = EXIT_FAILURE;
                    }
                  else if (processing_status & dpp::base_module::ERROR)
                    {
                      // A non-critical error has been met, we warn and
                      // skip to the next event record :
                      std::clog << datatools::io::error << APP_NAME_PREFIX
                                << "Processing of event record #" << event_counter << " failed." << std::endl;
                      if (break_on_error_as_fatal)
                        {
                          do_break_event_loop = true;
                          error_code = EXIT_FAILURE;
                        }
                    }
                  else if (processing_status & dpp::base_module::STOP)
                    {
                      if (verbose)
                        {
                          std::clog << datatools::io::warning << APP_NAME_PREFIX
                                    << "Processing of event record #" << event_counter << " stopped at some stage." << std::endl;
                        }
                    }
                  if (do_break_event_loop)
                    {
                      break;
                    }
                } // BOOST_FOREACH
            }
          catch (std::exception & x)
            {
              throw x;
            }

          if (debug)
            {
              ER.tree_dump (std::cout, "Event record :", "DEBUG: " + APP_NAME_PREFIX);
            }

          // Manage the sink if any :
          if (sink && ! sink->is_terminated ())
            {
              bool save_it = true;
              if (processing_status & dpp::base_module::STOP)
                {
                  save_it = save_stopped_event_records;
                }
              if (save_it)
                {
                  // Save the processed event record in the sink :
                  int output_status = sink->process (ER);
                  if (output_status != dpp::io_module::OK)
                    {
                      std::clog << datatools::io::error << APP_NAME_PREFIX << "Error while storing event #" << event_counter << " !" << std::endl;
                    }
                  stored_counter++;
                }
            }

          event_counter++;
          if ((max_events > 0) && (event_counter == max_events))
            {
              if (debug) std::clog << datatools::io::debug << APP_NAME_PREFIX
                                   << "max number of events reached !!!" << std::endl;
              do_break_event_loop = true;
            }
          // break check :
          if (do_break_event_loop)
            {
              if (debug) std::clog << datatools::io::debug << APP_NAME_PREFIX
                                   << "do_break_event_loop !!!" << std::endl;
              break;
            }
        }

      if (debug) std::clog << datatools::io::debug << APP_NAME_PREFIX
                           << "Exiting event record loop." << std::endl;

      std::clog << datatools::io::notice << APP_NAME_PREFIX << "Number of processed events : " << event_counter << "\n";
      if (sink)
        {
          std::clog << datatools::io::notice << APP_NAME_PREFIX << "Number of saved events     : " << stored_counter << "\n";
        }

      // Terminate the module manager :
      if (MM.get () != 0)
        {
          MM.get ()->reset (); // Not mandatory (automatic at destruction)
        }

    }
  catch (std::exception & x)
    {
      std::cerr << datatools::io::error << APP_NAME_PREFIX << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << datatools::io::error << APP_NAME_PREFIX << "Unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of dpp_processing.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
