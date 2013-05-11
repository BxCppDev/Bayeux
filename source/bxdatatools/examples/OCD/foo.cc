/* foo.cc */

#include<foo.h>

#include <datatools/units.h>
#include <datatools/clhep_units.h>

foo::foo() {
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

void foo::initialize(const datatools::properties & config_) {

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

  // Parse mandatory 'name' :
  const std::string name_key = "name";
  if (config_.has_key(name_key)) {
    name = config_.fetch_string(name_key);
  } else {
    throw std::logic_error("foo::initialize: Missing '"+name_key+"' property!");
  }

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

  // Rather complex triggering conditions for the 'dummy" property :
  if (! name.empty() && name[0] == '_' && dict.size() >= 2) {
    const std::string dummy_key = "dummy";
    if (! config_.has_key(dummy_key)) {
      throw std::logic_error("foo::initialize: Missing '"+dummy_key+"' property!");
    }
    dummy = config_.fetch_integer(dummy_key);
  }

  // Parse 'secret' :
  int secret;
  const std::string secret_key = "secret";
  if (config_.has_key(secret_key)) {
    secret = config_.fetch_integer(secret_key);
    if (secret > 3) {
      std::cerr << "DEVEL: foo::initialize: "
                << "You have found my secret !" << std::endl;
    }
  }

  initialized = true;
}

void foo::reset() {
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

void foo::dump(std::ostream & out_) const {
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
  //ocd_.set_class_library ("dummy");

  // The class detailed documentation :
  ocd_.set_class_documentation ("A example class of configurable object");

  {
    // Description of the 'debug' configuration property :
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
    // Description of the 'debug.level' configuration property :
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
      .set_long_description("The width of the foo object is expressed in \n"
                            "implicit length unit.                       \n"
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
      .set_long_description("The weight of the foo object is explicitely \n"
                            "passed with its value.                      \n"
                            )
      .set_explicit_unit(true)
      .set_unit_label("length")
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
      ;
  }

  {
    // Description of some dynamic properties that depend on the "label" property:
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
    // Description of some dynamic properties that depend on the "label" property:
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
    // Description of the 'dummy' configuration property :
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
    // Description of the 'secret' configuration property :
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

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the     \n"
                               "``datatools::properties`` ASCII format::        \n"
                               "                                                \n"
                               " debug : boolean = 1                            \n"
                               " debug.level : integer = 3                      \n"
                               " name : string = \"_you\"                       \n"
                               " what : string = \"Some text\"                  \n"
                               " tmpfile : const string as path = \\            \n"
                               "   \"/tmp/log.tmp\"                             \n"
                               " width : real = 1.23                            \n"
                               " weight : real as mass = 1.23 mg                \n"
                               " labels : string[2] = \"obj0\" \"obj1\"         \n"
                               " objects.obj0.value : integer = 67              \n"
                               " objects.obj1.value : integer = 12              \n"
                               " objects.obj0.value : integer = 67              \n"
                               " objects.obj0.color : string = \"blue\"         \n"
                               " dummy : integer = 666                          \n"
                               " secret : integer = 5                           \n"
                               "                                                \n"
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
