/* test_service_manager.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-09
 * Last modified : 2013-12-14
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrates how to create a service manager.
 *
 * Usage:
 *
 *  ${SNCORE_BIN_DIR}/test_service_manager --debug
 *
 */

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

// This Project:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/base_service.h>
#include <datatools/service_manager.h>

// explicitly include this dummy service (with registration) :
#include "dummy_service.h"
#include "dummy_service.cc"

using namespace std;

/** An event record test processing service:
 *  it adds a simple text property (namely the 'label')
 *  in the event header of any processed event.
 *
 */
class test_service : public ::datatools::base_service
{
public:

  test_service();

  virtual ~test_service();

  static const string DEFAULT_LABEL;

  void set_label (const string & a_label);

  const string & get_label () const;

  virtual bool is_initialized() const;

  virtual int initialize(const datatools::properties& /* config_ */,
                         datatools::service_dict_type& /* service_map_ */);

  virtual int reset();

private:

  string label_;

  /* Macro to declare the registration interface of this service
   * in the database with a default creator :
   */
  DATATOOLS_SERVICE_REGISTRATION_INTERFACE (test_service);
};

/** Auto-registration of this service class in a central service Db */
DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(test_service, "test_service")

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
test_service::test_service()
  : datatools::base_service("Test",
                            "An event record processor test service",
                            "0.1")
{
  label_ = "";
  return;
}

// Destructor :
test_service::~test_service()
{
  if (this->is_initialized()) this->test_service::reset();
}

bool test_service::is_initialized() const
{
  return ! label_.empty ();
}

// Initialization hook :
int test_service::initialize(const datatools::properties& a_config,
                             datatools::service_dict_type& /*a_dictionnary*/)
{

  if (!is_initialized ()) {
    // If the label is not setup yet, pickup from the configuration list:
    if (a_config.has_key ("label")) {
      string label = a_config.fetch_string ("label");
      set_label (label);
    }
  }

  // Default label is none is setup :
  if (label_.empty ()){
    label_ = DEFAULT_LABEL;
  }

  return datatools::SUCCESS;
}

// Reset hook :
int test_service::reset()
{
  label_ = "";
  return datatools::SUCCESS;
}

/************************************************************/

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  try
    {
      clog << "Test program for class 'datatools::service_manager' !" << endl;

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
                  clog << datatools::io::warning << "ignoring option '" << option << "'!" << endl;
                }
            }
          else
            {
              string argument = token;
              {
                clog << datatools::io::warning << "ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
        }

      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (datatools::base_service).print (clog);

      {
        // Setup the configuration parameters of the service:
        datatools::properties TS_config;
        if (debug) TS_config.store ("logging.priority","debug");
        TS_config.store ("label", "test_service::label");

        test_service TS;

        // Initialize the event record processing service :
        if (debug) clog << datatools::io::debug
                        << "Initializing service '"
                        << TS.get_name () << "'..." << endl;
        TS.initialize_standalone (TS_config);
        if (debug) clog << datatools::io::debug << "Done." << endl;

        clog << "Test service label is '" << TS.get_label () << "'" << endl;

        // Terminate the test service :
        if (debug) clog << datatools::io::debug
                        << "Terminating service '"
                        << TS.get_name () << "'..." << endl;
        TS.reset ();
        if (debug) clog << datatools::io::debug << "Done." << endl;
      }

      {
        uint32_t SM_flags = datatools::service_manager::BLANK;
        if (debug)
          {
            SM_flags |= datatools::service_manager::FACTORY_VERBOSE;
          }
        datatools::service_manager SM ("SM", "A test service manager", SM_flags);
        if (debug) SM.set_logging_priority(datatools::logger::PRIO_DEBUG);
        SM.tree_dump(clog, "Service manager : ");

        // Create a multi_property container:
        datatools::multi_properties SM_services_config("name", "type");
        SM_services_config.add ("test_1", "test_service");
        SM_services_config.grab_section ("test_1").store ("label", "test_service_1::label");

        SM_services_config.add ("test_2", "test_service");
        SM_services_config.grab_section ("test_2").store ("label", "test_service_2::label");
        vector<string> strict_dependencies;
        strict_dependencies.push_back ("test_1");
        SM_services_config.grab_section ("test_2").store ("dependencies.strict", strict_dependencies);

        SM_services_config.add ("foo", "datatools::dummy_service");
        SM_services_config.grab_section ("foo").store ("label", "King Arthur");

        SM_services_config.tree_dump(std::cerr, "SM services config: ", "DEVEL: ");
        if (debug) clog << datatools::io::debug << "Load embedded services' configuration..." << endl;
        // Load it !
        SM.load (SM_services_config);
        SM.tree_dump (clog, "Service manager (with loaded services) : ");

        // Load another multi_property container stored in a file :
        datatools::multi_properties SM_services_config_2("name", "type");
        string services_conf = "${DATATOOLS_TESTING_DIR}/config/test_service_manager.conf";
        datatools::fetch_path_with_env (services_conf);
        SM_services_config_2.read (services_conf);
        SM.load (SM_services_config_2);
        SM.tree_dump (clog, "Service manager (with loaded services 2) : ");

        if (debug) clog << datatools::io::debug << "Initializing the service manager..." << endl;
        SM.initialize ();
        if (debug) clog << datatools::io::debug << "Done." << endl;

        if (SM.has ("test_2") && SM.is_a<test_service> ("test_2"))
          {
            // Access to a service by (const) reference through its name and class :
            const test_service & TS = SM.get<test_service> ("test_2");
            clog << "Test service 'test_2' has label '" << TS.get_label () << "'" << endl;
          }
        if (SM.has ("test_2") && SM.is_a<test_service> ("test_2"))
          {
            // Access to a service by mutable reference through its name and class :
            test_service & TS =  SM.grab<test_service> ("test_2");
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

        clog << datatools::io::debug << "Terminating service manager..." << endl;
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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
