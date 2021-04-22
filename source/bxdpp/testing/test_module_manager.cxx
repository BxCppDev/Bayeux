/* test_module_manager.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-19
 * Last modified : 2013-02-16
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrate how to create a module
 *  for event record processing.
 *
 * Usage:
 *
 *  test_module_manager --debug \
 *    --module "chain1" \
 *    ${DPP_TESTING_DIR}/data/data_0.txt.gz \
 *    out.txt.gz
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdexcept>

#include <boost/foreach.hpp>

#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/library_loader.h>
#include <datatools/factory_macros.h>

#include <dpp/base_module.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_data_sink.h>
#include <dpp/dummy_module.h>
#include <dpp/module_manager.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  std::list<std::string> io_files;
  std::string output_file;

  try
    {
      std::clog << "Test program for class 'dpp::module_manager' !" << std::endl;

      bool   debug = false;
      bool   verbose = false;
      int    max_events = 0;
      std::string module_name;
      std::string mgr_config;
      std::string LL_config;
      std::vector<std::string> LL_dlls;

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
              else if ((option == "-v") || (option == "--verbose"))
                {
                  verbose = true;
                }
              else if (option == "-10")
                {
                  max_events = 10;
                }
              else if ((option == "-m") || (option == "--module"))
                {
                  module_name = argv_[++iarg];
                }
              else if ((option == "-c") || (option == "--config"))
                {
                  mgr_config = argv_[++iarg];
                }
              else if ((option == "-l") || (option == "--load-dll"))
                {
                  LL_dlls.push_back(argv_[++iarg]);
                }
              else
                {
                  std::clog << datatools::io::warning << "ignoring option '"
                            << option << "'!" << std::endl;
                }
            }
          else
            {
              std::string argument = token;
              {
                io_files.push_back (argument);
              }
            }
          iarg++;
        }

      datatools::library_loader LL(LL_config);
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

      if (module_name.empty ())
        {
          module_name = "chain1";
        }
      std::clog << datatools::io::notice << "Using processor module '"
                << module_name << "'." << std::endl;

      if (io_files.size () < 2)
        {
          //throw std::logic_error ("Missing input source(s) and output sink !");
          io_files.push_back("${DPP_TESTING_DIR}/data/data_0.txt.gz");
          io_files.push_back("${DPP_TESTING_DIR}/data/data_1.txt.gz");
          io_files.push_back("${DPP_TESTING_DIR}/data/data_2.txt.gz");
          io_files.push_back("${DPP_TESTING_DIR}/data/data_3.txt.gz");
          io_files.push_back("${DPP_TMP_TEST_DIR}/test_module_manager_out.txt");

        }
      output_file = io_files.back ();
      io_files.pop_back ();

      std::clog << datatools::io::notice << "Number of input data sources : "
                << io_files.size () << std::endl;
      std::clog << datatools::io::notice << "Output data source : "
                << "'" << output_file << "'" << std::endl;

      uint32_t flags = dpp::module_manager::BLANK;
      dpp::module_manager MM (flags);
      if (verbose) {
        MM.set_logging_priority(datatools::logger::PRIO_NOTICE);
      }
      if (debug) {
        MM.set_logging_priority(datatools::logger::PRIO_DEBUG);
      }

      // load properties from the configuration file:
      std::string MM_config_file = "${DPP_TESTING_DIR}/config/test_module_manager.conf";
      if (! mgr_config.empty ())
        {
          MM_config_file = mgr_config;
        }

      datatools::fetch_path_with_env (MM_config_file);
      std::clog << "Manager config. file : '" << MM_config_file << "'" << std::endl;

      datatools::properties MM_config;
      datatools::read_config (MM_config_file, MM_config);
      MM.initialize (MM_config);
      MM.tree_dump (std::clog, "Module manager (initialized) : ");

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
      the_module.tree_dump (std::clog, "The active module : ");
      MM.tree_dump (std::clog, "Module manager (after grabing the active module) : ");

      // Setup the data output sink :
      dpp::simple_data_sink sink (output_file);

      // Loop on the data source files :
      int counts = 0;
      for (std::list<std::string>::const_iterator i = io_files.begin ();
           i != io_files.end ();
           i++)
        {
          const std::string & source_label = *i;
          if (debug) std::clog << datatools::io::notice
                               << "Using data source '" << source_label << "'..." << std::endl;
          dpp::simple_data_source source (source_label);
          if (debug) source.set_logging_priority (datatools::logger::PRIO_DEBUG);

          // Loop on the event records from the data source file :
          while (source.has_next_record ())
            {
              datatools::things ER;

              if (debug) std::clog << datatools::io::debug
                                   << "Loading next event record..." << std::endl;
              source.load_next_record (ER);

              if (debug) std::clog << datatools::io::debug
                                   << "Processing the event record..." << std::endl;
              dpp::base_module::process_status status = the_module.process (ER);
              if (status & dpp::base_module::PROCESS_ERROR)
                {
                  std::cerr << datatools::io::error
                            << "Error at processing event record #"
                            <<  counts << std::endl;
                }

              if (debug) ER.tree_dump (std::cout, "Event record :", "DEBUG: ");
              sink.store_next_record (ER);

              counts++;
              if ((max_events > 0) && (counts == max_events))
                {
                  break;
                }
            }
        }

      std::vector<std::pair<std::string,std::string> > modules_infos;
      MM.build_modules_infos(modules_infos);
      std::clog << "\nManaged modules are: " << std::endl;
      for (int i = 0; i < (int) modules_infos.size(); i++) {
        std::clog << "'" << modules_infos[i].first << "' as a '" <<   modules_infos[i].second << "'"
                  << std::endl;
      }

      std::clog << "\nManaged modules are: " << std::endl;
      const dpp::module_handle_dict_type & MD = MM.get_modules();
      for (dpp::module_handle_dict_type::const_iterator i = MD.begin();
           i != MD.end();
           i++) {
        std::clog << "'" << i->first << "' as '";
        const dpp::module_entry_type & me = i->second;
        std::clog << me.get_module_id();
        std::clog << "'" << std::endl;
        // Also we can inspect the configuration:
        //   me.get_module_config(); (properties...)
        // the status within the manager
        //   me.is_created() ,  me.is_initialized()
      }

      std::clog << "\nList of manager registered modules IDs: " << std::endl;
      std::vector<std::string> mmods;
      MM.build_registered_module_types(mmods);
      for (int i = 0; i < (int) mmods.size(); i++) {
        std::clog << mmods[i] << std::endl;
      }

      // Terminate the module manager :
      MM.reset ();

      {
       datatools::object_configuration_description OCD;
       datatools::load_ocd<dpp::module_manager>(OCD);
       std::ofstream fscf ("test_OCD_module_manager.sample.conf");
       OCD.generate_sample_configuration(fscf, "the configuration of a 'dpp::module_manager' test object");
      }

      /*
       * const dpp::base_module::factory_register_type & frt
       *   = dpp::base_module::get_system_factory_register();
       * frt.list_of_factories(mods);
       */
      std::vector<std::string> mods;
      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(dpp::base_module).list_of_factories(mods);
      std::clog << "\nList of system registered modules: " << std::endl;
      for (int i = 0; i < (int) mods.size(); i++) {
        std::clog << mods[i] << std::endl;
      }

      datatools::fetch_path_with_env(output_file);
      unlink(output_file.c_str());
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl;
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
