/* test_service_manager.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-09
 * Last modified : 2011-06-22
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrate how to create a service manager.
 *
 * Usage:
 *
 *  ${SNCORE_BIN_DIR}/test_service_manager --debug
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

#include <datatools/services/base_service.h>
#include <datatools/services/service_macros.h>
#include <datatools/services/service_manager.h>
#include <datatools/utils/utils.h>

using namespace std;
using namespace datatools::service;
using namespace datatools::utils;

/** An event record test processing service:
 *  it adds a simple text property (namely the 'label')
 *  in the event header of any processed event.
 *
 */
DATATOOLS_SERVICE_CLASS_DECLARE(test_service)
{
	/** Macro to automate the generation of the service interface :
	 * - default constructor
	 * - destructor
	 * - virtual interface methods
	 */
	DATATOOLS_SERVICE_INTERFACE (test_service);

 public:

	static const string DEFAULT_LABEL;

 public:

	void set_label (const string & a_label);

	const string & get_label () const;

 private:

	string label_;

	/* Macro to declare the registration interface of this service
	 * in the database with a default creator :
	 */
	DATATOOLS_SERVICE_REGISTRATION_INTERFACE (test_service);
};

const string test_service::DEFAULT_LABEL = "test_service::label";

void test_service::set_label (const string & a_label)
{
	label_ = a_label;
	return;
}

const string & test_service::get_label () const
{
	return label_;
}

// Constructor :
DATATOOLS_SERVICE_CONSTRUCTOR_IMPLEMENT_HEAD(test_service,							\
																						 "Test",										\
																						 "An event record processor test service", \
																						 "0.1")
{
	label_ = "";
	return;
}

// Destructor :
DATATOOLS_SERVICE_DESTRUCTOR_IMPLEMENT(test_service)

DATATOOLS_SERVICE_IS_INITIALIZED_IMPLEMENT_HEAD(test_service)
{
	return ! label_.empty ();
}

// Initialization hook :
DATATOOLS_SERVICE_INITIALIZE_IMPLEMENT_HEAD(test_service,				\
																						a_config,						\
																						a_dictionnary)
{
	/* const datatools::utils::properties & a_config
	 * service_dict_type *                  a_dictionary
	 * void *                               a_user_resource
	 */

	if (!is_initialized ())
		{
			// If the label is not setup yet, pickup from the configuration list:
			if (a_config.has_key ("label"))
				{
					string label = a_config.fetch_string ("label");
					set_label (label);
				}
		}

	// Default label is none is setup :
	if (label_.empty ())
		{
			label_ = DEFAULT_LABEL;
		}

	return datatools::utils::SUCCESS;
}

// Reset hook :
DATATOOLS_SERVICE_RESET_IMPLEMENT_HEAD(test_service)
{
	label_ = "";
	return datatools::utils::SUCCESS;
}

// DATATOOLS_SERVICE_FETCH_DEPENDENCIES_IMPLEMENT_HEAD(test_service,a_dependency_list)
// {
// 	/* Parameter:
// 	 * service_dependency_dict_type & a_dependency_list;
// 	 * this is a list of dependency_info_type objects
// 	 * to be updated:
// 	 */
// 	a_dependency_list.clear ();
// 	{
// 		// Here we add a dependency with the 'dummy' external service :
// 		dependency_info_type dit;
// 		dit.id      = "dummy_service";  // name of the service
// 		dit.version = "";      // requested version
// 		dit.meta    = "debug"; // meta information
// 		dit.level   = OPTIONAL_DEPENDENCY; //
// 		a_dependency_list["foo"] = dit;
// 	}
// 	return;
// }

/** Auto-registration of this service class in a central service Db */
DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(test_service, "test_service")

/************************************************************/

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  try
    {
      clog << "Test program for class 'datatools::service::service_manager' !" << endl;

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
			base_service::get_service_creator_db ().dump_service_creators (clog);

			{
				// Setup the configuration parameters of the service:
				properties TS_config;
				if (debug) TS_config.store_flag ("debug");
				TS_config.store ("label", "test_service::label");

				test_service TS;

				// Initialize the event record processing service :
				if (debug) clog << io::debug
                        << "Initializing service '"
                        << TS.get_name () << "'..." << endl;
				TS.initialize_standalone (TS_config);
				if (debug) clog << io::debug << "Done." << endl;

				clog << "Test service label is '" << TS.get_label () << "'" << endl;

				// Terminate the test service :
				if (debug) clog << io::debug
                        << "Terminating service '"
                        << TS.get_name () << "'..." << endl;
				TS.reset ();
				if (debug) clog << io::debug << "Done." << endl;
			}

			{
				uint32_t SM_flags = service_manager::BLANK;
				if (debug)
					{
						SM_flags |= service_manager::DEBUG;
					}
				service_manager SM ("SM", "A test service manager", SM_flags);
				SM.tree_dump (clog, "Service manager : ");

				// Create a multi_property container:
				multi_properties SM_services_config;
				SM_services_config.add ("test_1", "test_service");
				SM_services_config.get_section ("test_1").store ("label", "test_service_1::label");

				SM_services_config.add ("test_2", "test_service");
				SM_services_config.get_section ("test_2").store ("label", "test_service_2::label");
				vector<string> strict_dependencies;
				strict_dependencies.push_back ("test_1");
				SM_services_config.get_section ("test_2").store ("dependencies.strict", strict_dependencies);

				SM_services_config.add ("foo", "datatools::service::dummy_service");
				SM_services_config.get_section ("foo").store ("label", "King Arthur");

				if (debug) clog << io::debug << "Load embedded services' configuration..." << endl;
				// Load it !
				SM.load (SM_services_config);

				// Load another multi_property container stored in a file :
				datatools::utils::multi_properties SM_services_config_2;
				string services_conf = "${DATATOOLS_ROOT}/resources/test/test_service_manager.conf";
				datatools::utils::fetch_path_with_env (services_conf);
				SM_services_config_2.read (services_conf);
				SM.load (SM_services_config_2);

				if (debug) clog << io::debug << "Initializing the service manager..." << endl;
				SM.initialize ();

				if (SM.has ("test_2") && SM.is_a<test_service> ("test_2"))
					{
						// Access to a service by (const) reference through its name and class :
						const test_service & TS = SM.get<test_service> ("test_2");
						clog << "Test service 'test_2' has label '" << TS.get_label () << "'" << endl;
					}
				if (SM.has ("test_2") && SM.is_a<test_service> ("test_2"))
					{
						// Access to a service by mutable reference through its name and class :
						test_service & TS =  SM.get<test_service> ("test_2");
						TS.set_label ("new");
						clog << "Test service 'test_2' has new label '" << TS.get_label () << "'" << endl;
					}

				SM.tree_dump (clog, "Service manager : ", "");
				if (SM.can_drop ("test_2"))
					{
						// Remove an existing service :
						clog << "Dropping test service 'test_2'..." << endl;
						SM.drop ("test_2");
					}
				SM.tree_dump (clog, "Service manager (dropped test_2) : ", "");

				clog << io::debug << "Terminating service manager..." << endl;
				SM.reset ();
				SM.tree_dump (clog, "Service manager (terminated) : ", "");
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
