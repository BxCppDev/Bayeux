/* test_module.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-09
 * Last modified : 2013-02-15
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrates how to create
 *  a module for data record processing.
 *
 * Usage:
 *
 *  test_module --debug \
 *    ${DPP_DATA_DIR}/testing/data/data_0.txt.gz \
 *    out.txt.gz
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
#include <datatools/exception.h>

#include <dpp/version.h>
#include <dpp/base_module.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_data_sink.h>

/** An data record test processing module:
 *  it adds a simple text property (namely the 'label')
 *  in a "Header" data bank of any processed
 *  data record. It also adds a new data bank named "Test"
 *  and store some properties in it.
 *
 *  The declaration of the module can also be done using dedicated macros:
 *
 *   DPP_MODULE_CLASS_DECLARE(test_module)
 *   {
 *      // Automatic declaration of the basic interface of the module:
 *      DPP_MODULE_INTERFACE();
 *
 *      // Automatic declaration of the registration interface of the module:
 *      DPP_MODULE_REGISTRATION_INTERFACE(test_module, "test_module");
 *
 *      ...
 *
 *   };
 *
 */
DPP_MODULE_CLASS_DECLARE(test_module)
//class test_module : public dpp::base_module
{
 public:

  static const std::string DEFAULT_LABEL;
  static const std::string UNINITIALIZED_LABEL;

  void set_label(const std::string & a_label);

  const std::string & get_label() const;

  // Constructor :
  test_module(datatools::logger::priority p = datatools::logger::PRIO_FATAL);

  // Destructor :
  virtual ~test_module();

  // This macro setup the module standard interface (initialize/reset/process) :
  DPP_MODULE_INTERFACE();
  /*
   * // Initialization method :
   * virtual void initialize(const datatools::properties & a_config,
   *                         datatools::service_manager & a_service_manager,
   *                         module_handle_dict_type & a_module_dictionnary);
   *
   * // Termination method :
   * virtual void reset();
   *
   * // Event processing method :
   * virtual int process(datatools::things & a_data_record);
   */

 private:

  std::string _label_;

  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(test_module);

};

/** The definition of the module can also be done using macros:
 *
 * \code
 * // Automatic instantiation of registration code :
 * DPP_MODULE_REGISTRATION_IMPLEMENT(test_module);
 *
 * // Helper macro for the constructor proposed by default :
 * DPP_MODULE_CONSTRUCTOR_IMPLEMENT_HEAD(test_module,a_logging_priority)
 * {
 *   // specific code to construct internal resources of the module :
 *   ...
 *   return;
 * }
 *
 * // Helper macro for a destructor proposed by default:
 * DPP_MODULE_DEFAULT_DESTRUCTOR_IMPLEMENT(test_module)
 *
 * // Helper macro for the initialization method :
 * DPP_MODULE_INITIALIZE_IMPLEMENT_HEAD(test_module)
 * {
 *   if (is_initialized()) {
 *     std::ostringstream message;
 *     message << "test_module::initialize: "
 *             << "Module '" << get_name () << "' is already initialized ! ";
 *     throw std::logic_error (message.str ());
 *   }
 *
 *   // specific code to initialize internal resources of the module :
 *   ...
 *
 *   _set_initialized (true);
 *   return;
 * }
 *
 * // Helper macro for the termination method :
 * DPP_MODULE_RESET_IMPLEMENT_HEAD (test_module)
 * {
 *   if (! is_initialized ()) {
 *     std::ostringstream message;
 *     message << "test_module::reset: "
 *             << "Module '" << get_name () << "' is not initialized ! ";
 *     throw std::logic_error (message.str ());
 *   }
 *   _set_initialized (false);
 *
 *   // specific code to terminate internal resources of the module :
 *   ...
 *   return;
 * }
 *
 * // Helper macro for the processing method :
 * DPP_MODULE_PROCESS_IMPLEMENT_HEAD (test_module, the_data_record)
 * {
 *   // Where: 'the_data_record' is a non-const reference
 *   // to a 'dpp::things' instance.
 *
 *   if (! is_initialized ()) {
 *     std::ostringstream message;
 *     message << "test_module::process: "
 *             << "Module '" << get_name () << "' is not initialized ! ";
 *     throw std::logic_error (message.str ());
 *   }
 *
 *   // specific code to process the event record :
 *   ...
 *
 *   return 0;
 * }
 * \endcode
 *
 */

// The string used to flag the not-initialized label of the module :
const std::string test_module::UNINITIALIZED_LABEL = "";

// The default label of the module :
const std::string test_module::DEFAULT_LABEL = "TEST";

// Setter for the label of the module :
void test_module::set_label (const std::string & a_label)
{
  _label_ = a_label;
  return;
}

// Getter for the label of the module :
const std::string & test_module::get_label () const
{
  return _label_;
}

/*** Implementation of the interface ***/

// Constructor :
test_module::test_module(::datatools::logger::priority p)
  : ::dpp::base_module(p)
{
  _label_ = UNINITIALIZED_LABEL;
  return;
}

// Destructor :
test_module::~test_module ()
{
  // Make sure all internal resources are terminated
  // before destruction :
  if (is_initialized()) reset();
  return;
}

// Initialization :
void test_module::initialize (const datatools::properties & a_config,
                              datatools::service_manager & a_service_manager,
                              dpp::module_handle_dict_type & a_module_dictionnary)
{
  DT_THROW_IF(is_initialized (),
              std::logic_error,
              "Test module '" << get_name () << "' is already initialized ! ");

  _common_initialize(a_config);

  if (_label_ == UNINITIALIZED_LABEL) {
    // If the label is no setup, pickup from the configuration list:
    if (a_config.has_key ("label")) {
      std::string label = a_config.fetch_string ("label");
      set_label (label);
    }
  }

  // Default :
  if (_label_ == UNINITIALIZED_LABEL) {
    _label_ = DEFAULT_LABEL;
  }

  // Flag the initialization status :
  _set_initialized (true);
  return;
}

// Reset :
void test_module::reset ()
{
  DT_THROW_IF(! is_initialized (),
              std::logic_error,
              "Test module '" << get_name () << "' is not initialized !");
  _label_ = "";
  _set_initialized (false);
  return;
}

// Processing :
int test_module::process (datatools::things & the_data_record)
{
  DT_THROW_IF(! is_initialized (),
              std::logic_error,
              "Test module '" << get_name () << "' is not initialized !");
  if (the_data_record.has("Header")
      && the_data_record.is_a<datatools::properties>("Header")) {
    // Find the event header :
    datatools::properties & the_properties
      = the_data_record.grab<datatools::properties>("Header");
    if (is_debug ()) {
      std::clog << datatools::io::debug
                << "test_module::at_process_: "
                << "Found the event header !"
                << std::endl;
    }
    // Add a string property in it with value '_label_' :
    if (is_debug ()) {
      std::clog << datatools::io::debug
                << "test_module::at_process_: "
                << "Adding the 'label' property..."
                << std::endl;
    }
    the_properties.store ("label", _label_);
  } else {
    std::clog << datatools::io::error
              << "test_module::at_process_: "
              << "Could not find any event header !"
              << std::endl;
    // Cannot find the event header :
    return dpp::PROCESS_ERROR;
  }

  if (! the_data_record.has ("Test")) {
    // Add another data bank :
    datatools::properties & test_properties
      = the_data_record.add<datatools::properties> ("Test");
    test_properties.store ("library", "dpp");
    test_properties.store ("library.version", DPP_LIB_VERSION);
    test_properties.store ("added_by", "test_module");
    test_properties.store ("date", (int) time(0));
  }

  return dpp::PROCESS_OK;
}

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(test_module, "test_module")

/************************************************************/

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  std::string input_file;
  std::string output_file;

  try
    {
      std::clog << "Test program for class 'dpp::base_module' !" << std::endl;

      bool debug = false;
      int max_events = 1;

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
              else
                {
                  std::clog << datatools::io::warning
                            << "Ignoring option '" << option << "'!" << std::endl;
                }
            }
          else
            {
              std::string argument = token;
              if (input_file.empty ())
                {
                  input_file = argument;
                }
              else if (output_file.empty ())
                {
                  output_file = argument;
                }
              else
                {
                  std::clog << datatools::io::warning
                            << "Ignoring argument '" << argument << "'!" << std::endl;
                }
            }
          iarg++;
        }


      if (input_file.empty()) {
        input_file = "${DPP_DATA_DIR}/testing/data/data_0.txt.gz";
      }
      if (output_file.empty()) {
        output_file = "test_module_out.txt.gz";
      }

      DT_THROW_IF(input_file.empty(),
                  std::logic_error,
                  "Missing input source !");
      DT_THROW_IF(output_file.empty(),
                  std::logic_error,
                  "Missing output sink !");

      std::clog << datatools::io::notice << "Input data source : "
                << "'" << input_file << "'" << std::endl;
      std::clog << datatools::io::notice << "Output data source : "
                << "'" << output_file << "'" << std::endl;

      // Setup the configuration parameters of the module:
      datatools::properties TM_config;
      if (debug) TM_config.store_flag ("debug");
      TM_config.store ("label", "TEST");

      // Declare a 'test_module' :
      test_module TM;

      // Initialize the event record processing module :
      TM.initialize_standalone (TM_config);

      // Setup the data output sink :
      if (debug) std::clog << datatools::io::notice
                           << "Using data sink '" << output_file << "'..." << std::endl;
      dpp::simple_data_sink sink (output_file);


      // Setup the data source :
      if (debug) std::clog << datatools::io::notice
                           << "Using data source '" << input_file << "'..." << std::endl;
      dpp::simple_data_source source (input_file);
      if (debug) source.set_logging_priority (datatools::logger::PRIO_DEBUG);

      // Loop on the event records from the data source file :
      int counts = 0;
      while (source.has_next_record ()) {
        // Declare the data record :
        datatools::things DR;

        // Loading next event record :
        source.load_next_record (DR);

        // Processing the event record :
        int status = TM.process (DR);
        if (status != dpp::PROCESS_OK) {
          std::clog << datatools::io::warning
                    << "test_module: Processing has failed for this event record !" << std::endl;
        }

        // Dump the event record :
        if (debug) DR.tree_dump (std::cout, "Event record :", "DEBUG: ");

        // Storing the event record :
        sink.store_next_record (DR);

        counts++;
        if ((max_events > 0) && (counts == max_events)) {
          break;
        }
      }

      // Terminate the event record processing module :
      TM.reset ();
    }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "Unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_module.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
