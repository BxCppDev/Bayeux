/* test_service_manager.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-09
 * Last modified : 2016-09-08
 *
 * Copyright (C) 2011-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

/** An event record test processing service:
 *  it adds a simple text property (namely the 'label')
 *  in the event header of any processed event.
 *
 */
class test_service : public ::datatools::base_service
{
public:

  static const std::string DEFAULT_LABEL; ///< Default label

  /// Default constructor
  test_service();

  /// Destructor
  ~test_service() override;

  /// Set the label
  void set_label(const std::string & a_label);

  /// Get the label
  const std::string & get_label() const;

  /// Check initialization status
  bool is_initialized() const override;

  /// Initialize the service
  int initialize(const datatools::properties& /* config_ */,
                 datatools::service_dict_type& /* service_map_ */) override;

  /// Reset the service
  int reset() override;

  /// Use the service
  void use_me_after_initialization();

  /// Smart print
  void tree_dump(std::ostream& out = std::clog,
                 const std::string & title = "",
                 const std::string & indent = "",
                 bool inherit = false) const override;

private:

  bool initialized_ = false; //!< Initialization flag
  std::string label_;        //!< Configuration parameter

  /* Macro to declare the registration interface of this service
   * in the database with a default creator :
   */
  DATATOOLS_SERVICE_REGISTRATION_INTERFACE(test_service)
};

/** Auto-registration of this service class in a central service Db */
DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(test_service, "test_service")

const std::string test_service::DEFAULT_LABEL = "test_service::label";

void test_service::set_label(const std::string & a_label)
{
  DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the label after initialization!");
  label_ = a_label;
  return;
}

const std::string & test_service::get_label() const
{
  return label_;
}

// Constructor :
test_service::test_service()
  : datatools::base_service("Test",
                            "A test service",
                            "0.1")
{
  initialized_ = false;
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
  return initialized_;
}

// Initialization hook :
int test_service::initialize(const datatools::properties& a_config,
                             datatools::service_dict_type& /*a_dictionnary*/)
{
  DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
  _common_initialize(a_config);

  DT_LOG_DEBUG(get_logging_priority(), "Initializing service '" << get_name() << "'...");

  if (label_.empty()) {
    // If the label is not setup yet, pickup from the configuration list:
    if (a_config.has_key("label")) {
      std::string label = a_config.fetch_string("label");
      set_label(label);
    }
  }

  // Default label is none is setup :
  if (label_.empty()) {
    label_ = DEFAULT_LABEL;
  }

  DT_LOG_DEBUG(get_logging_priority(), "Service '" << get_name() << "' label is '" << label_ << "'");

  initialized_ = true;
  DT_LOG_DEBUG(get_logging_priority(), "Initialization of service '" << get_name() << "' is done.");
  return datatools::SUCCESS;
}

// Reset hook :
int test_service::reset()
{
  DT_THROW_IF(!is_initialized(), std::logic_error, "Service is not initialized!");
  initialized_ = false;
  label_ = "";
  return datatools::SUCCESS;
}

void test_service::use_me_after_initialization()
{
  DT_THROW_IF(!is_initialized(), std::logic_error, "Service is not initialized!");
  DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Using '" << get_name() << "'...");
  return;
}

void test_service::tree_dump(std::ostream& a_out,
                             const std::string& a_title,
                             const std::string& a_indent,
                             bool a_inherit) const
{
  this->base_service::tree_dump(a_out, a_title, a_indent, true);

  a_out << a_indent << i_tree_dumpable::inherit_tag(a_inherit)
        << "Label  : '"
        << this->get_label() << "'" << std::endl;
}

/************************************************************/


int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;

  try {
    std::clog << "Test program for class 'datatools::service_manager' !" << std::endl;

    bool debug = false;
    bool dynamic = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-D") || (option == "--dynamic")) {
          dynamic = true;
        } else {
          std::clog << datatools::io::warning << "ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        string argument = token;
        {
          std::clog << datatools::io::warning << "ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(datatools::base_service).print(std::clog);

    {
      // Setup the configuration parameters of the service:
      datatools::properties TS_config;
      if (debug) TS_config.store("logging.priority","debug");
      TS_config.store("label", "test_service::label");

      test_service TS;

      // Initialize the event record processing service :
      if (debug) std::clog << datatools::io::debug
                           << "Initializing service '"
                           << TS.get_name() << "'..." << std::endl;
      TS.initialize_standalone(TS_config);
      if (debug) std::clog << datatools::io::debug << "Done." << std::endl;

      std::clog << "Test service label is '" << TS.get_label() << "'" << std::endl;

      // Terminate the test service :
      if (debug) std::clog << datatools::io::debug
                           << "Terminating service '"
                           << TS.get_name() << "'..." << std::endl;
      TS.reset();
      if (debug) std::clog << datatools::io::debug << "Done." << std::endl;
    }

    {
      uint32_t SM_flags = datatools::service_manager::BLANK;
      if (debug) {
        SM_flags |= datatools::service_manager::FACTORY_VERBOSE;
      }
      if (dynamic) {
        SM_flags |= datatools::service_manager::ALLOW_DYNAMIC_SERVICES;
      }
      datatools::service_manager SM("SM", "A test service manager", SM_flags);
      if (debug) SM.set_logging_priority(datatools::logger::PRIO_DEBUG);
      SM.tree_dump(std::clog, "Service manager : ");

      // Create a multi_property container:
      datatools::multi_properties SM_services_config("name", "type");
      SM_services_config.add("test_1", "test_service");
      SM_services_config.grab_section("test_1").store("label", "test_service_1::label");

      SM_services_config.add("test_2", "test_service");
      SM_services_config.grab_section("test_2").store("label", "test_service_2::label");
      vector<string> strict_dependencies;
      strict_dependencies.push_back("test_1");
      SM_services_config.grab_section("test_2").store("dependencies.strict", strict_dependencies);

      SM_services_config.add("foo", "datatools::dummy_service");
      SM_services_config.grab_section("foo").store("label", "King Arthur");

      SM_services_config.tree_dump(std::cerr, "SM services config: ", "DEVEL: ");
      if (debug) std::clog << datatools::io::debug << "Load embedded services' configuration..." << std::endl;
      // Load it !
      SM.load(SM_services_config);
      SM.tree_dump(std::clog, "Service manager(with loaded services) : ");

      // Load another multi_property container stored in a file :
      datatools::multi_properties SM_services_config_2("name", "type");
      std::string services_conf = "${DATATOOLS_TESTING_DIR}/config/test_service_manager.conf";
      datatools::fetch_path_with_env(services_conf);
      datatools::read_config(services_conf, SM_services_config_2);
      SM.load(SM_services_config_2);
      SM.tree_dump(std::clog, "Service manager (with loaded services 2) : ");
      if (debug) std::clog << datatools::io::debug << "Initializing the service manager..." << std::endl;
      SM.initialize();
      if (debug) std::clog << datatools::io::debug << "Done." << std::endl;
      if (SM.has("test_2") && SM.is_a<test_service>("test_2")) {
        // Access to a service by (const) reference through its name and class :
        const test_service & TS = SM.get<test_service>("test_2");
        std::clog << "Test service 'test_2' has label '" << TS.get_label() << "'" << std::endl;
      }
      if (SM.has("test_2") && SM.is_a<test_service>("test_2")) {
        // Access to a service by mutable reference through its name and class :
        test_service & TS = SM.grab<test_service>("test_2");
        TS.use_me_after_initialization();
        std::clog << "Test service 'test_2' has new label '" << TS.get_label() << "'" << std::endl;
      }

      try {
        std::clog << "Loading test service 'foo1'..." << std::endl;
        datatools::properties foo1_config;
        foo1_config.store("label", "foo1::label");
        SM.load("foo1", "test_service", foo1_config);
        SM.tree_dump(std::clog, "Service manager (loaded foo1) : ", "");
      } catch (std::exception & dyn_serv_error) {
        std::cerr << "error: " << "as expected: " << dyn_serv_error.what() << std::endl;
      }

      try {
        std::clog << "Loading test service 'foo3' without initialization..." << std::endl;
        {
          test_service & foo3
            = dynamic_cast<test_service&>(SM.load_no_init("foo3", "test_service"));
          std::clog << "Manually configure service 'foo3'..." << std::endl;
          foo3.set_label("foo3::label");
          std::clog << "Pass configuration to service 'foo3'..." << std::endl;
          datatools::properties foo3_config;
          foo3_config.store("logging.priority", "debug");
          // foo3_config.store("label", "bar");
          SM.configure_no_init("foo3", foo3_config);
        }
        SM.tree_dump(std::clog, "Service manager (loaded/configured foo3) : ", "");
        // {
        //  std::clog << "Manually initialize service 'foo3'..." << std::endl;
        //  foo3.initialize_with_services(SM.grab_services());
        // }
        {
          std::clog << "Automatically grab and initialize service 'foo3'..." << std::endl;
          test_service & foo3 = SM.grab<test_service>("foo3");
          std::clog << "Use service 'foo3' after initialization..." << std::endl;
          foo3.use_me_after_initialization();
        }
      } catch (std::exception & dyn_serv_error) {
        std::cerr << "error: " << "as expected: " << dyn_serv_error.what() << std::endl;
      }

      std::vector<std::string> serv_names;
      uint32_t serv_filter = 0;
      SM.build_list_of_services(serv_names, serv_filter);
      std::clog << "Current list of services:\n";
      for (auto name : serv_names) {
        std::clog << " - " << name << '\n';
      }
      std::clog << '\n';

      SM.tree_dump(std::clog, "Service manager : ", "");
      if (SM.can_drop("test_2")) {
        // Remove an existing service :
        std::clog << "Dropping test service 'test_2'..." << std::endl;
        SM.drop("test_2");
      }
      try {
        std::clog << "Dropping test service 'srv1'..." << std::endl;
        SM.drop("srv1");
        SM.tree_dump(std::clog, "Service manager (dropped srv1) : ", "");
      } catch (std::exception & dyn_serv_error) {
        std::cerr << "error: " << "as expected: " << dyn_serv_error.what() << std::endl;
      }
      std::clog << datatools::io::debug << "Terminating service manager..." << std::endl;
      SM.reset();
      SM.tree_dump(std::clog, "Service manager (terminated) : ", "");
    }

  } catch (exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
