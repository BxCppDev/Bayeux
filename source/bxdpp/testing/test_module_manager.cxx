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
 *    ${DPP_DATA_DIR}/testing/data/data_0.txt.gz \
 *    out.txt.gz
 *
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

#include <dpp/base_module.h>
#include <dpp/module_macros.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_data_sink.h>
#include <dpp/dummy_module.h>
#include <dpp/module_manager.h>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/utils/ioutils.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/ioutils.h>
#include <datatools/serialization/bio_guard.h>
#else
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/bio_guard.h>
#endif

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
              else
                {
                  std::clog << DPP_DU::io::warning << "ignoring option '"
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

      if (module_name.empty ())
        {
          module_name = "chain1";
        }
      std::clog << DPP_DU::io::notice << "Using processor module '"
                << module_name << "'." << std::endl;

      if (io_files.size () < 2)
        {
          throw std::logic_error ("Missing input source(s) and output sink !");
        }
      output_file = io_files.back ();
      io_files.pop_back ();

      std::clog << DPP_DU::io::notice << "Number of input data sources : "
                << io_files.size () << std::endl;
      std::clog << DPP_DU::io::notice << "Output data source : "
                << "'" << output_file << "'" << std::endl;

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
      dpp::module_manager MM (flags);

      // load properties from the configuration file:
      std::string MM_config_file = "${DPP_DATA_DIR}/testing/config/test_module_manager.conf";
      if (! mgr_config.empty ())
        {
          MM_config_file = mgr_config;
        }

      DPP_DU::fetch_path_with_env (MM_config_file);
      std::clog << "Manager config. file : '" << MM_config_file << "'" << std::endl;

      DPP_DU::properties MM_config;
      DPP_DU::properties::read_config (MM_config_file,
                                       MM_config);
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
          if (debug) std::clog << DPP_DU::io::notice
                               << "Using data source '" << source_label << "'..." << std::endl;
          dpp::simple_data_source source (source_label);
          source.set_debug_level (debug ? 1: 0);

          // Loop on the event records from the data source file :
          while (source.has_next_record ())
            {
              DPP_DU::things ER;

              if (debug) std::clog << DPP_DU::io::debug
                                   << "Loading next event record..." << std::endl;
              source.load_next_record (ER);

              if (debug) std::clog << DPP_DU::io::debug
                                   << "Processing the event record..." << std::endl;
              int status = the_module.process (ER);
              if (status & dpp::base_module::ERROR)
                { 
                  std::cerr << DPP_DU::io::error
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

      // Terminate the module manager :
      MM.reset ();

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

// end of test_module_manager.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
