/* foo.cc */

#include<foo.h>

#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/exception.h>

foo::foo() {
  initialized = false;
  name = "";
  what = "";
  datatools::invalidate(width);
  datatools::invalidate(weight);
  dummy = -1;
  tmpfile = "";
  _logging_ = datatools::logger::PRIO_FATAL;
}

void foo::set_logging_priority(const datatools::logger::priority & p_) {
  _logging_ = p_;
}

void foo::initialize(const datatools::properties & config_) {
  datatools_trace(_logging_, "Entering initialization...");

  double length_unit = CLHEP::cm; // implicit length unit
  bool hello = false;

  datatools_notice(_logging_, "Parsing configuration parameters...");

  // Parse 'logging' :
  const std::string logging_key = "logging";
  if (config_.has_flag(logging_key)) {
    // Parse mandatory 'logging.level' :
    const std::string logging_level_key = "logging.level";
    DT_THROW_IF(! config_.has_key(logging_level_key),
                std::logic_error,
                "foo::initialize: Missing '" << logging_level_key << "' property!");
    std::string logging_label = config_.fetch_string(logging_level_key);
    _logging_ = datatools::logger::get_priority(logging_label);
    DT_THROW_IF(_logging_ == datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "foo::initialize: Undefined logging level '" << logging_label << "' !");
  }

  // Parse mandatory 'name' :
  const std::string name_key = "name";
  DT_THROW_IF(! config_.has_key(name_key),
              std::logic_error,
              "foo::initialize: Missing '" << name_key << "' property!");
  name = config_.fetch_string(name_key);

  // Parse 'what' :
  const std::string what_key = "what";
  if (config_.has_key(what_key)) {
    what = config_.fetch_string(what_key);
  }

  // Parse 'tmpfile' :
  const std::string tmpfile_key = "tmpfile";
  if (config_.has_key(tmpfile_key)) {
    tmpfile = config_.fetch_path(tmpfile_key);
  }

  // Parse 'width' :
  const std::string width_key = "width";
  if (config_.has_key(width_key)) {
    width = config_.fetch_real(width_key);
    width *= length_unit;
  }

  // Parse 'weight' :
  const std::string weight_key = "weight";
  if (config_.has_key(weight_key)) {
    weight = config_.fetch_real(weight_key);
    DT_THROW_IF(! config_.has_explicit_unit(weight_key),
                std::logic_error,
                "foo::initialize: Missing explicit weight unit for the '"
                << weight_key << "' property!");
  }

  // Parse 'labels' :
  const std::string labels_key = "labels";
  std::vector<std::string> labels;
  if (config_.has_key(labels_key)) {
    std::vector<std::string> labels;
    config_.fetch(labels_key, labels);
    // Parse dependees of the 'labels' property :
    for (int i = 0; i < labels.size() ; i++) {
      std::string value_key = "objects." + labels[i] + ".value";
      DT_THROW_IF(! config_.has_key(value_key),
                  std::logic_error,
                  "foo::initialize: Missing '" << value_key << "' property!");
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

  // Rather complex triggering conditions for the 'dummy" property :
  if (! name.empty() && name[0] == '_' && dict.size() >= 2) {
    const std::string dummy_key = "dummy";
    DT_THROW_IF(! config_.has_key(dummy_key),
                std::logic_error,
                "foo::initialize: Missing '" << dummy_key << "' property!");
    dummy = config_.fetch_integer(dummy_key);
  }

  // Parse 'secret' :
  int secret;
  const std::string secret_key = "secret";
  if (config_.has_key(secret_key)) {
    secret = config_.fetch_integer(secret_key);
    if (secret > 3) {
      datatools_warning(_logging_,
                        "Ha ! Ha ! "
                        << "You have found how to use my secret property !");
      hello = true;
    }
  }

  // Initialization :
  datatools_notice(_logging_, "Processing initialization operations...");
  if (hello) {
    std::clog << "                                       \n"
              << "     Welcome to the OCD system !       \n"
              << "                                       \n";
  }

  datatools_notice(_logging_, "Initialization is done.");

  initialized = true;
  datatools_trace(_logging_, "Exiting initialization.");
}

void foo::reset() {
  datatools_trace(_logging_, "Entering reset...");
  initialized = false;
  name.clear();
  what.clear();
  dict.clear();
  tmpfile.clear();
  dummy = 0;
  datatools::invalidate(width);
  datatools::invalidate(weight);
  datatools_trace(_logging_, "Exiting reset.");
  _logging_ = datatools::logger::PRIO_FATAL;
}

void foo::dump(std::ostream & out_) const {
  out_ << "|-- Initialized : " << initialized << std::endl;
  out_ << "|-- Logging threshold: \"" << datatools::logger::get_priority_label(_logging_) << '"' << std::endl;
  out_ << "|-- Name : \"" << name << "\"" << std::endl;
  out_ << "|-- What : \"" << what << "\"" << std::endl;
  out_ << "|-- Tmpfile : \"" << tmpfile << "\"" << std::endl;
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
         << " ; \"" << i->second.second << "\"}" << std::endl;
  }
  out_ << "|-- Dummy : " <<dummy << " " << std::endl;
  out_ << "|-- Width : " << width / CLHEP::mm << " mm" << std::endl;
  out_ << "`-- Weight : "
       << weight / datatools::units::get_unit(datatools::units::get_default_unit_symbol_from_label("mass"))
       << " " << datatools::units::get_default_unit_symbol_from_label("mass")
       << std::endl;
}


/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg foo  the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(foo,ocd_)
{
  // The class name :
  ocd_.set_class_name ("foo");

  // The class terse description :
  ocd_.set_class_description ("A foo class");

  // The library the class belongs to :
  ocd_.set_class_library ("datatools_ex_OCD");

  // The class detailed documentation :
  ocd_.set_class_documentation ("A example class of configurable object with OCD support");

  {
    // Description of the 'logging' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("logging")
      .set_terse_description("Flag to activate logging")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("The allowed values are 0 (false) or 1 (true). \n"
                            "                                \n"
                            "Example::                       \n"
                            "                                \n"
                            "  logging : boolean = 0         \n"
                            "                                \n"
                            )
      ;
  }

  {
    // Description of the 'debug.level' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("logging.level")
      .set_terse_description("The logging level")
      .set_traits(datatools::TYPE_STRING)
      .set_triggered_by_flag("logging")
      .set_mandatory(true)
      .set_long_description("A label taken from:             \n"
                            "                                \n"
                            "* ``fatal``                     \n"
                            "* ``critical``                  \n"
                            "* ``error``                     \n"
                            "* ``warning``                   \n"
                            "* ``notice``                    \n"
                            "* ``information``               \n"
                            "* ``debug``                     \n"
                            "* ``trace``                     \n"
                            "                                \n"
                            "It is mandatory if the 'logging' flag property is set. \n"
                            "                                      \n"
                            "Example::                             \n"
                            "                                      \n"
                            "  logging : boolean = 1               \n"
                            "  logging.level : string = \"notice\" \n"
                            "                                      \n"
                          )
      ;
  }

  {
    // Description of the 'name' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("name")
      .set_terse_description("The name of the foo object")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_const(true)
      .set_long_description("A character string that helps to identify the foo \n"
                            "instance within a program.                        \n"
                            )
      .add_example(         "The object name::                                 \n"
                            "                                                  \n"
                            "  name : string = \"bar\"                         \n"
                            "                                                  \n"
                            )
      ;
  }

  {
    // Description of the 'what' configuration property :
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
      .add_example(         "The description string::                       \n"
                            "                                               \n"
                            "  what : string = \"the description of some resource\"\n"
                            "                                               \n"
                            )
      ;
  }

  {
    // Description of the 'tmpfile' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("tmpfile")
      .set_terse_description("Some file path associated to the foo object")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(true)
      .set_const(true)
      .set_long_description("A valid filesystem path to a file. \n"
                            "instance within a program.                        \n"
                            )
      .add_example(         "The name of a temp file::                         \n"
                            "                                                  \n"
                            "  tmpfile : string as path = \"/tmp/foo.tmp\"     \n"
                            "                                                  \n"
                            )
      ;
  }

  {
    // Description of the 'width' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("width")
      .set_terse_description("The width of the foo object")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("The width of the foo object is expressed in       \n"
                            "implicit length unit (cm).                        \n"
                            )
      .add_example(         "A width::                                         \n"
                            "                                                  \n"
                            "  width : real = 1.3                              \n"
                            "                                                  \n"
                            )
      ;
  }

  {
    // Description of the 'weight' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("weight")
      .set_terse_description("The weight of the foo object")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("The weight of the foo object is explicitely       \n"
                            "passed with its units.                            \n"
                            )
      .add_example(         "A weight::                                        \n"
                            "                                                  \n"
                            "  weight : real as mass = 34.1 kg                 \n"
                            "                                                  \n"
                            )
      .set_explicit_unit(true)
      .set_unit_label("mass")
      ;
  }

  {
    // Description of the 'labels' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("labels")
      .set_terse_description("A list of labels")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("A list of string labels representing external objects      \n"
                            "addressed through their name from an external dictionnary. \n"
                            )
      .add_example(
                            "Two labels::                                               \n"
                            "                                                           \n"
                            "  labels : string[2] = \"Obj1\" \"Obj2\"                   \n"
                            "                                                           \n"
                            )
      ;
  }

  {
    // Description of some dynamic properties that depend on the "labels" property:
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("objects.${labels}.value")
      .set_terse_description("An integer value associated to the object referenced through its label")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_long_description("The label of the object is picked from the 'labels' \n"
                            "vector of properties.                               \n"
                            )
      .add_example(
                            "A set of interdependant parameters::                       \n"
                            "                                                           \n"
                            "  labels : string[2] = \"Obj1\" \"Obj2\"                   \n"
                            "  objects.Obj1.value : integer = 3                         \n"
                            "  objects.Obj2.value : integer = 5                         \n"
                            "                                                           \n"
                            "This property is mandatory if the ``labels`` has been      \n"
                            "provided.                                                  \n"
                            )
      ;
  }

  {
    // Description of some dynamic properties that depend on the "label" property:
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("objects.${labels}.color")
      .set_terse_description("A color associated to the object referenced through its label")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The label of the object is picked from the 'labels' vector     \n"
                            "of string properties. If unspecified for a given label value,  \n"
                            "a default color is used.                                       \n"
                            )
      .add_example(
                            "A set of configuration parameters::                        \n"
                            "                                                           \n"
                            "  labels : string[2] = \"Obj1\" \"Obj2\"                   \n"
                            "  # objects.Obj1.color : string = \"white\"                \n"
                            "  objects.Obj2.color : string = \"red\"                    \n"
                            "                                                           \n"
                            )
      ;
  }

  {
    // Description of the 'dummy' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("dummy")
      .set_terse_description("A complex property")
      .set_traits(datatools::TYPE_INTEGER)
      .set_complex_triggering_conditions(true)
      .set_long_description("The 'dummy' property is only required under some \n"
                            "rather complex conditions (RTFM!).               \n"
                            "It is mandatory if and only if the following     \n"
                            "conditions are fulfilled:                        \n"
                            "                                                 \n"
                            "1. The ``name`` property is not empty            \n"
                            "2. The ``labels`` property has at least 2 entries\n"
                            "                                                 \n"
                            )
      .add_example(
                            "A set of configuration parameters::              \n"
                            "                                                 \n"
                            "  name   : string = \"bar\"                      \n"
                            "  labels : string[2] = \"Obj1\" \"Obj2\"         \n"
                            "  dummy  : integer = 7                           \n"
                            "                                                 \n"
                            )
      ;
  }

  {
    // Description of the 'secret' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("secret")
      .set_traits(datatools::TYPE_INTEGER)
      .set_terse_description("A property known only by developpers")
      .set_long_description("The 'secret' property is only used during the  \n"
                            "development cycle. It is thus undocumented.    \n"
                            "Use it at your own risk !                      \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  logging : boolean = 1                          \n"
                               "  logging.level : string = \"debug\"             \n"
                               "  name : string = \"_you\"                       \n"
                               "  what : string = \"Some text\"                  \n"
                               "  tmpfile : const string as path = \"/tmp/log.tmp\"\n"
                               "  width : real = 1.23                            \n"
                               "  weight : real as mass = 1.23 mg                \n"
                               "  labels : string[2] = \"Obj0\" \"Obj1\"         \n"
                               "  objects.Obj0.value : integer = 67              \n"
                               "  objects.Obj1.value : integer = 12              \n"
                               "  objects.Obj0.color : string = \"blue\"         \n"
                               "  dummy : integer = 666                          \n"
                               "  secret : integer = 1                           \n"
                               "                                                 \n"
                               );

  /** Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides (see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'foo' :
DOCD_CLASS_SYSTEM_REGISTRATION(foo,"foo")

/* end of foo.cc */
