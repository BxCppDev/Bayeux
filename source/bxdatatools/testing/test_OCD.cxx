// test_OCD.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

// This project:
#include <datatools/datatools_config.h>
#include <datatools/object_configuration_description.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>

// using namespace std;

// Dummy class :
struct foo {

  foo() {
    initialized = false;
    debug = false;
    debug_level = -1;
    name = "";
    what = "";
    datatools::invalidate(width);
    datatools::invalidate(weight);
    dummy = -1;
    tmpfile = "";
  }

  void initialize(const datatools::properties & config_) {

    // std::cerr << "DEVEL: foo::initialize: Entering..." << std::endl;
    double length_unit = CLHEP::cm;

    // Parse 'debug' :
    const std::string debug_key = "debug";
    if (config_.has_flag(debug_key)) {
      debug = true;
      const std::string debug_level_key = "debug.level";
      if (! config_.has_key(debug_level_key)) {
        throw std::logic_error("foo::initialize: Missing '"+debug_level_key+"' property!");
      }
      debug_level = config_.fetch_integer(debug_level_key);
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 1..." << std::endl;

    // Parse mandatory 'name' :
    const std::string name_key = "name";
    if (config_.has_key(name_key)) {
      name = config_.fetch_string(name_key);
    }
    else {
      throw std::logic_error("foo::initialize: Missing '"+name_key+"' property!");
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 2..." << std::endl;

    // Parse 'what' :
    const std::string what_key = "what";
    if (config_.has_key(what_key)) {
      what = config_.fetch_string(what_key);
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 3..." << std::endl;

    // Parse 'tmpfile' :
    const std::string tmpfile_key = "tmpfile";
    if (config_.has_key(tmpfile_key)) {
      tmpfile = config_.fetch_path(tmpfile_key);
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 4..." << std::endl;

    // Parse 'width' :
    const std::string width_key = "width";
    if (config_.has_key(width_key)) {
      width = config_.fetch_real(width_key);
      width *= length_unit;
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 5..." << std::endl;

    // Parse 'weight' :
    const std::string weight_key = "weight";
    if (config_.has_key(weight_key)) {
      weight = config_.fetch_real(weight_key);
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 6..." << std::endl;

    // Parse 'labels' :
    const std::string labels_key = "labels";
    if (config_.has_key(labels_key)) {
      std::vector<std::string> labels;
      config_.fetch(labels_key, labels);
      // std::cerr << "DEVEL: foo::initialize: size of labels=" << labels.size() << std::endl;
      // Parse dependees of the 'labels' property :
      for (int i = 0; i < (int) labels.size() ; i++) {
        std::string value_key = "objects." + labels[i] + ".value";
        if (! config_.has_key(value_key)) {
          throw std::logic_error("foo::initialize: Missing '"+value_key+"' property!");
        }
        std::pair<int,std::string> entry;
        entry.first = config_.fetch_integer(value_key);
        entry.second = "white";
        std::string color_key = "objects." + labels[i] + ".color";
        if (config_.has_key(color_key)) {
          entry.second = config_.fetch_string(color_key);
        }
        dict[labels[i]] = entry;
      }
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 7..." << std::endl;

    // Rather complex triggering conditions for the 'dummy" property :
    if (! name.empty() && name[0] == '_' && dict.size() >= 2) {
      const std::string dummy_key = "dummy";
      if (! config_.has_key(dummy_key)) {
        throw std::logic_error("foo::initialize: Missing '"+dummy_key+"' property!");
      }
      dummy = config_.fetch_integer(dummy_key);
    }

    // std::cerr << "DEVEL: foo::initialize: TEST 8..." << std::endl;

    // Parse mandatory 'secret' :
    int secret;
    const std::string secret_key = "secret";
    if (config_.has_key(secret_key)) {
      secret = config_.fetch_integer(secret_key);
      if (secret > 3) {
        std::clog << "DEVEL: foo::initialize: "
                  << "You have found my secret !" << std::endl;
      }
    }

    initialized = true;
  }

  void reset() {
    initialized = false;
    debug = false;
    debug_level = -1;
    name.clear();
    what.clear();
    dict.clear();
    tmpfile.clear();
    dummy = 0;
    datatools::invalidate(width);
    datatools::invalidate(weight);
  }

  void dump(std::ostream & out_) const {
    out_ << "|-- Initialized : " << initialized << std::endl;
    out_ << "|-- Debug : " << debug << std::endl;
    out_ << "|-- Debug level : " << debug_level << std::endl;
    out_ << "|-- Name : '" << name << "'" << std::endl;
    out_ << "|-- What : '" << what << "'" << std::endl;
    out_ << "|-- Tmpfile : '" << tmpfile << "'" << std::endl;
    out_ << "|-- Dict : " << std::endl;
    for (std::map<std::string, std::pair<int,std::string> >::const_iterator i
           = dict.begin();
         i != dict.end();
         i++) {
      out_ << "|   ";
      std::map<std::string, std::pair<int,std::string> >::const_iterator j = i;
      j++;
      if (j == dict.end()) out_ << "`-- ";
      else out_ << "|-- ";
      out_ << i->first << " : {" << i->second.first
           << ";" << i->second.second << "}" << std::endl;
    }
    out_ << "|-- Dummy : " <<dummy << " " << std::endl;
    out_ << "|-- Width : " << width / CLHEP::mm << " mm" << std::endl;
    out_ << "`-- Weight : "
         << weight / datatools::units::get_unit(datatools::units::get_default_unit_symbol_from_label("mass"))
         << " " << datatools::units::get_default_unit_symbol_from_label("mass")
         << std::endl;
 }

  // Attributes (should be private but here this is just an example) :
  bool initialized;
  bool debug;
  int debug_level;
  std::string name;
  std::string what;
  std::string tmpfile;
  std::map<std::string, std::pair<int,std::string> > dict;
  int dummy;
  double width;
  double weight;
};

/***************************************************************************/

#include <datatools/ocd_macros.h>

/***************************
 * OCD support : interface *
 ***************************/

// @arg foo the name the class
DOCD_CLASS_DECLARATION(foo)


/********************************
 * OCD support : implementation *
 ********************************/

// @arg foo  the name the class
// @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
//            to be initialized passed by mutable reference.
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(foo,ocd_)
{
  ocd_.set_class_name ("foo");
  ocd_.set_class_description ("A foo class");

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("The allowed values are 0 (false) or 1 (true). \n"
                            )
      ;
  }

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("debug.level")
      .set_terse_description("The debugging level")
      .set_traits(datatools::TYPE_INTEGER)
      .set_triggered_by_flag("debug")
      .set_mandatory(true)
      .set_long_description("An integer from 0 (minimal debugging output)     \n"
                            "to 10 (maximal debugging output).                \n"
                            "It is mandatory if 'debug' flag property is set. \n"
                            )
      ;
  }

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("name")
      .set_terse_description("The name of the foo object")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_const(true)
      .set_long_description("A character string that helps to identify the foo instance \n"
                            "within a program.                                          \n"
                            )
      ;
  }

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("what")
      .set_terse_description("The foo object's embeded description string")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_const(true)
      .set_long_description("An  user-friendly description character string \n"
                            "It may spread over several lines.              \n"
                            )
      ;
  }

  {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("tmpfile")
      .set_terse_description("Some file path associated to the foo object")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_const(true)
      .set_long_description("A valid filesystem path to a file. \n"
                            )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("width")
      .set_terse_description("The width of the foo object")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("The width of the foo object is expressed in \n"
                            "implicit length unit.                       \n"
                            )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("weight")
      .set_terse_description("The weight of the foo object")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("The weight of the foo object is explicitely \n"
                            "passed with its value.                      \n"
                            )
      .set_explicit_unit(true)
      .set_unit_label("length")
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("labels")
      .set_terse_description("A list of labels")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("A list of string labels representing external objects      \n"
                            "addressed through their name from an external dictionnary. \n"
                            )
    ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("objects.${labels}.value")
      .set_terse_description("An integer value associated to the object referenced through its label")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_long_description("The label of the object is picked from the 'labels' \n"
                            "vector of properties.                               \n"
                            )
   ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("objects.${labels}.color")
      .set_terse_description("A color associated to the object referenced through its label")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The label of the object is picked from the 'labels' vector     \n"
                            "of string properties. If unspecified, a default color is used. \n"
                            )
   ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("dummy")
      .set_terse_description("A complex property")
      .set_traits(datatools::TYPE_INTEGER)
      .set_complex_triggering_conditions(true)
      .set_long_description("The 'dummy' property is only required under some \n"
                            "rather complex conditions (RTFM!).               \n"
                           )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("secret")
      .set_terse_description("A property known only by developpers")
      .set_complex_triggering_conditions(true)
      .set_long_description("The 'secret' property is only used during the  \n"
                            "development cycle. It is thus undocumented.    \n"
                            "Use it at your own risk !                      \n"
                           )
      ;
  }

  ocd_.set_configuration_hints("Nothing special.\n");
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(foo,"foo")

#include <datatools/service_manager.h>

int main (int /*argc_*/, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      std::clog << "Test program for the 'OCD' tools." << std::endl;

      {
        std::clog << std::endl;
        datatools::object_configuration_description OCD;
        if (! datatools::load_ocd<datatools::properties>(OCD)) {
          std::clog << "No OCD support for the 'datatools::properties' class." << std::endl;
        }
      }

      {
        std::clog << std::endl;
        datatools::object_configuration_description OCD;
        if (datatools::load_ocd<foo>(OCD)) {
          std::clog << "Found OCD support for the 'foo' class." << std::endl;
          OCD.print(std::clog, "*** ");
          OCD.dump(std::clog, "OCD: ");

          std::string foo_config_file
            = "${DATATOOLS_TESTING_DIR}/config/test_OCD_foo.conf";
          datatools::fetch_path_with_env(foo_config_file);
          datatools::properties foo_config;
          datatools::read_config(foo_config_file, foo_config);

          if (OCD.has_validation_support()) {
            std::string message;
            if (OCD.validate(foo_config, message)) {
              std::clog << "The property file '" << foo_config_file << "' is valid !\n";
            }
            else {
              std::clog << "The property file '" << foo_config_file
                        << "' does not conform the OCD rules ! ";
              std::clog << "Message is : " << message << "\n";
            }
          }
          foo f;
          f.initialize(foo_config);
          std::clog << "foo object is : " << std::endl;
          f.dump(std::clog);
          std::ofstream fscf ("test_OCD_foo.sample.conf");
          OCD.generate_sample_configuration(fscf, "the configuration of a 'foo' test object");
        }
      }

      {
        std::clog << std::endl;
        datatools::object_configuration_description OCD;
        if (datatools::load_ocd<datatools::service_manager>(OCD)) {
          std::clog << "Found OCD support for the 'datatools::service_manager' class." << std::endl;
          OCD.print(std::clog, "=== ");
          std::ofstream fscf ("test_OCD_SM.sample.conf");
          OCD.generate_sample_configuration(fscf, "the configuration of a 'datatools::service_manager' object");
        }
      }

      // Print the list of class ID with OCD system support :
      DOCD_GET_SYSTEM_REGISTRATION().smart_dump(std::clog, "The OCD system registration : ");

      // Declare an OCD register (prefilled with the OCD system register):
      datatools::detail::ocd::ocd_registration OCDreg(true);
      OCDreg.smart_dump(std::clog, "An OCD registration : ");

      if (OCDreg.has_id("foo")) {
        std::clog << "Class 'foo' has OCD support." << std::endl;
        const datatools::object_configuration_description & foo_OCD = OCDreg.get("foo");
        foo_OCD.dump(std::clog, "foo OCD: ");
      }

      std::clog << "The end." << std::endl;
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error !" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}
