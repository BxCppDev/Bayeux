/* test_dummy_service.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-20
 * Last modified : 2011-06-20
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrate how to use a dummy service.
 *
 * Usage:
 *
 *  ${SNCORE_BIN_DIR}/test_dummy_service --debug
 *
 */

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

// This project:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>

// This test:
#include "dummy_service.h"
#include "dummy_service.cc"

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  try
    {
      std::clog << "Test program for class 'datatools::dummy_service' !" << std::endl;

      bool debug = false;

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
              else
                {
                  std::clog << datatools::io::warning << "ignoring option '" << option << "'!" << std::endl;
                }
            }
          else
            {
              std::string argument = token;
              {
                std::clog << datatools::io::warning << "ignoring argument '" << argument << "'!" << std::endl;
              }
            }
          iarg++;
        }

      {
        // Setup the configuration parameters of the service:
        datatools::properties DS_config;
        if (debug) DS_config.store_flag ("debug");
        DS_config.store ("label", "test_service::label");

        datatools::dummy_service DS;

        // Initialize the event record processing service :
        if (debug) std::clog << datatools::io::debug << "Initializing service '" << DS.get_name () << "'..." << std::endl;
        DS.initialize_standalone (DS_config);
        if (debug) std::clog << datatools::io::debug << "Done." << std::endl;

        std::clog << "Dummy service label is '" << DS.get_label () << "'" << std::endl;

        // Terminate the test service :
        if (debug) std::clog << datatools::io::debug << "Terminating service '" << DS.get_name () << "'..." << std::endl;
        DS.reset ();
        if (debug) std::clog << datatools::io::debug << "Done." << std::endl;
      }

}
  catch (std::exception & x)
    {
      std::cerr << datatools::io::error << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << datatools::io::error << "Unexpected error!" << std::endl;
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
