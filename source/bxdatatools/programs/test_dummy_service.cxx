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

#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

#include <datatools/utils/ioutils.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/multi_properties.h>

#include <datatools/services/dummy_service.h>
#include <datatools/utils/utils.h>

using namespace std;
using namespace datatools::service;
using namespace datatools::utils;

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  try
    {
      clog << "Test program for class 'datatools::service::dummy_service' !" << endl;

      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
							string option = token;
							if ((option == "-d") || (option == "--debug"))
								{
									debug = true;
								}
							else
								{
									clog << io::warning << "ignoring option '" << option << "'!" << endl;
								}
            }
          else
            {
              string argument = token;
              {
								clog << io::warning << "ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
				}

			base_service::g_debug = debug;
			{
				// Setup the configuration parameters of the service:
				properties DS_config;
				if (debug) DS_config.store_flag ("debug");
				DS_config.store ("label", "test_service::label");

				dummy_service DS;

				// Initialize the event record processing service :
				if (debug) clog << io::debug << "Initializing service '" << DS.get_name () << "'..." << endl;
				DS.initialize_standalone (DS_config);
				if (debug) clog << io::debug << "Done." << endl;

				clog << "Dummy service label is '" << DS.get_label () << "'" << endl;

				// Terminate the test service :
				if (debug) clog << io::debug << "Terminating service '" << DS.get_name () << "'..." << endl;
				DS.reset ();
				if (debug) clog << io::debug << "Done." << endl;
			}

}
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_service_manager.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
