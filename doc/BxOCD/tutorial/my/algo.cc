//! \file my/algo.cc Implementation file for the my::algo configurable class

// Ourselves:
#include <my/algo.h>

// Standard library:
#include <string>
#include <fstream>
#include <limits>
#include <cmath>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

namespace my {

  algo::algo()
  {
    _initialized_ = false;
    _debug_ = false;
    _max_nloops_ = 100;
    _epsilon_ = std::numeric_limits<double>::quiet_NaN(); /* Invalid default value*/
    /* _output_filename_ is left empty */
    return;
  }

  void algo::initialize(const datatools::properties & config_)
  {
    if (_initialized_) throw std::logic_error("my::algo::run: Cannot initialize an already initialized object!");

    if (config_.has_key("debug")) {
      _debug_ = config_.fetch_boolean("debug");
    }

    if (config_.has_key("max_nloops")) {
      int value = config_.fetch_integer("max_nloops");
      if (value < 1) throw std::logic_error("my::algo::run: Invalid value for 'max_nloops' parameter!");
      _max_nloops_ = value;
    }

    if (config_.has_key("epsilon")) {
      double value = config_.fetch_real("epsilon");
      if (value < 1.e-15) throw std::logic_error("my::algo::run: Invalid value for 'epsilon' parameter!");
      _epsilon_ = value;
    }
    // This is a mandatory parameter because no default is provided:
    if (_epsilon_ != _epsilon_) throw std::logic_error("my::algo::run: Missing 'epsilon' parameter!");

    if (config_.has_key("output_filename")) {
      std::string value = config_.fetch_string("output_filename");
      _output_filename_ = value;
    }

    if (_debug_) {
      std::clog << "debug: my::algo::initialize: max nloops = " << _max_nloops_ << '\n';
      std::clog << "debug: my::algo::initialize: epsilon = " << _epsilon_ << '\n';
      std::clog << "debug: my::algo::initialize: outputfilename = '" << _output_filename_ << " \n";
    }

    _initialized_ = true;
    return;
  }

  void algo::reset()
  {
    if (!_initialized_) throw std::logic_error("my::algo::run: Cannot reset an uninitialized object!");
    _initialized_ = false;
    _debug_ = false;
    _max_nloops_ = 100;
    _epsilon_ = std::numeric_limits<double>::quiet_NaN();
    _output_filename_.clear();
    return;
  }

  void algo::run()
  {
    if (!_initialized_) throw std::logic_error("my::algo::run: Object is not initialized!");

    std::ofstream * output_file = 0; // The output data stream
    if (! _output_filename_.empty()) {
      // Initializing the optional output file:
      if (_debug_) std::clog << "debug: my::algo::run: Creating output data file '" << _output_filename_<< "'...\n";
      output_file = new std::ofstream(_output_filename_.c_str());
      *output_file << "#!my::algo\n";
      *output_file << "# max_nloops=" << _max_nloops_ << '\n';
      *output_file << "# epsilon=" << _epsilon_ << '\n';
    }

    // Running the algorithm:
    double val = 666.0;
    int iloop = 0;
    while (true) {
      if (_debug_) std::clog << "debug: my::algo::run: Loop #" << iloop << '\n';
      double old_val = val;
      val = std::sqrt(old_val);
      if (output_file) {
        // Print current loop number and value:
        *output_file << iloop << ' ' << val << '\n';
      }
      if (std::abs((val - old_val)/old_val) < _epsilon_) {
        break;
      }

      if (iloop >= _max_nloops_) {
        break;
      }
      iloop++;
    }
    if (_debug_) std::clog << "debug: my::algo::run: After " << iloop << " iteration(s), we found: \n";
    std::cout << val << '\n';

    if (output_file) {
      // Terminating the optional output file:
      *output_file << "#end\n";
      delete output_file;
      if (_debug_) std::clog << "debug: my::algo::run: Output data file '" << _output_filename_<< "' is closed.\n";
    }
    return;
  }

} // end of namespace my

// OCD support from Bayeux/datatools:
#include <datatools/object_configuration_description.h>

//! \brief Open the OCD support implementation block for the my::algo class
//!
//! Opening macro for implementation of the class OCD support:
//!  @arg my::algo the full class name
//!  @arg ocd_ is the identifier of the \b datatools::object_configuration_description object
//!            to be initialized (passed by mutable reference).
//!
//!  This macro must be used outside of any namespace. It must be associated to the
//!  DOCD_CLASS_DECLARATION() macro used in the my/algo.h header file.
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(my::algo, ocd_)
{
  // The class name :
  ocd_.set_class_name("my::algo");

  // The class terse description :
  ocd_.set_class_description("A dummy algorithm class");

  // The library the class belongs to :
  ocd_.set_class_library("BxOCDTutorial");

  // The class detailed documentation :
  ocd_.set_class_documentation("A configurable dummy algorithm class with OCD support");

  {
    // Description of the 'debug' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("The debug mode")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("A flag that activates debug printing.    \n"
                            )
      .add_example("Activate debug printing::                         \n"
                   "                                                  \n"
                   "  debug : boolean = true                          \n"
                   "                                                  \n"
                   )
      ;
  }

  {
    // Description of the 'max_nloops' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("max_nloops")
      .set_terse_description("The maximum number of loops")
      .set_traits(datatools::TYPE_INTEGER)
      .set_default_value_integer(100)
      .set_mandatory(false)
      .set_long_description("This is the maximum number of loops done by the algorithm. \n"
                            )
      .add_example("Limit the algorithm to 10 loops::                 \n"
                   "                                                  \n"
                   "  max_nloops : integer = 10                       \n"
                   "                                                  \n"
                   )
      ;
  }

  {
    // Description of the 'epsilon' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("epsilon")
      .set_terse_description("The convergence tolerance of the algorithm")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This is the convergence tolerance of the algorithm.       \n"
                            "It is used to stop the main loop as soon as the tolerance \n"
                            "is reached.                                               \n"
                            )
      .add_example("Set the tolerance to one per mille::              \n"
                   "                                                  \n"
                   "  epsilon : real = 0.001                          \n"
                   "                                                  \n"
                   )
      ;
  }

  {
    // Description of the 'output_filename' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("output_filename")
      .set_terse_description("The output file name")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("This is the name of the optional file where to store    \n"
                            "intermediate results of the algorithm. If not provided, \n"
                            "no output file is generated. Only the final result will \n"
                            "be printed in the standard output.                      \n"
                            )
      .add_example("Set the output filename::                           \n"
                   "                                                    \n"
                   "  output_filename : string as path = \"loops.data\" \n"
                   "                                                    \n"
                   )
      ;
  }


  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the         \n"
                               "``datatools::properties`` ASCII format::            \n"
                               "                                                    \n"
                               "  #@description The debug flag                      \n"
                               "  debug : boolean = true                            \n"
                               "                                                    \n"
                               "  #@description The maximum number of loops         \n"
                               "  max_nloops : integer = 10                         \n"
                               "                                                    \n"
                               "  #@description The convergence tolerance           \n"
                               "  epsilon : real = 0.001                            \n"
                               "                                                    \n"
                               "  #@description The name of the output file         \n"
                               "  output_filename : string as path = \"loops.data\" \n"
                               "                                                    \n"
                               );


  /* Set the validation support flag :
   * we activate this if the description of all configuration
   * properties has been provides (see above). This enables the
   * OCD tools to check the syntax and validity of a given
   * configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
//! \brief Close the OCD support implementation block for the my::algo class)
//!
//! Closing macro for implementation of the class OCD support.
//!
//! This macro must be used outside of any namespace, to close the
//! block opend by the DOCD_CLASS_IMPLEMENT_LOAD_BEGIN macro.
DOCD_CLASS_IMPLEMENT_LOAD_END();

//! \brief OCD registration for the my::algo class (implementation)
//!
//! @arg my::algo the class full name
//! @arg "my::algo" the class registration unique id
//!
//! This macro must be used outside of any namespace, after the \a open/close
//! OCD implementation block associated to the class.
DOCD_CLASS_SYSTEM_REGISTRATION(my::algo, "my::algo");
