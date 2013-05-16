#include <dpp_ex01/raw_generator_module.h>

#include <limits>
#include <stdexcept>

#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>

#include <dpp_ex01/raw_data.h>

namespace dpp_ex01 {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(raw_generator_module, "dpp_ex01::raw_generator_module")

  /// Default name of the *raw data* bank :
  const std::string raw_generator_module::DEFAULT_RD_BANK_LABEL = "RD";

  void raw_generator_module::_set_defaults()
  {
    _raw_data_bank_label_.clear();
    datatools::invalidate(_mean_number_of_hits_);
    datatools::invalidate(_mean_energy_);
    datatools::invalidate(_sigma_energy_);
  }

  raw_generator_module::raw_generator_module(datatools::logger::priority logging_priority)
    : dpp::base_module(logging_priority)
  {
    _set_defaults();
  }

  raw_generator_module::~raw_generator_module()
  {
    // Make sure all internal resources are terminated before destruction :
    if (is_initialized()) reset();
  }

  void raw_generator_module::initialize(const datatools::properties & a_config,
                                        datatools::service_manager & a_service_manager,
                                        dpp::module_handle_dict_type & a_module_dictionnary)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Raw generator module ``" << get_name() << "`` is already initialized !");

    _common_initialize(a_config);

    int prng_seed = 0;
    double energy_unit = CLHEP::MeV;

    // Parsing configuration parameters :

    if (! dpp::base_module::is_debug()) {
      // Optional ``debug`` property :
      if (a_config.has_flag("debug")) {
        dpp::base_module::set_debug(true);
      }
    }

    if (_raw_data_bank_label_.empty()) {
      // Optional ``rd_bank_label`` property :
      if (a_config.has_key("rd_bank_label")) {
        std::string rd_bank_label = a_config.fetch_string("rd_bank_label");
        set_raw_data_bank_label(rd_bank_label);
      }
    }

    // Mandatory ``prng_seed`` property :
    if (a_config.has_key("prng_seed")) {
      prng_seed = a_config.fetch_integer("prng_seed");
    }

    // Mandatory ``mean_number_of_hits`` property :
    DT_THROW_IF(! a_config.has_key("mean_number_of_hits"),
                std::logic_error,
                "Missing ``mean_number_of_hits`` property !");
    set_mean_number_of_hits(a_config.fetch_real("mean_number_of_hits"));

    // Optional ``energy_unit`` property :
    if (a_config.has_key("energy_unit")) {
      std::string energy_unit_symbol = a_config.fetch_string("energy_unit");
      energy_unit = datatools::units::get_energy_unit_from(energy_unit_symbol);
    }

    // Mandatory ``mean_energy`` property :
    DT_THROW_IF(! a_config.has_key("mean_energy"),
                std::logic_error,
                "Missing ``mean_energy`` property !");
    double mean_energy = a_config.fetch_real("mean_energy");
    if (! a_config.has_explicit_unit("mean_energy")) {
      mean_energy *= energy_unit;
    }
    set_mean_energy(mean_energy);

    // Mandatory ``sigma_energy`` property :
    DT_THROW_IF(! a_config.has_key("sigma_energy"),
                std::logic_error,
                "Missing ``sigma_energy`` property !");
    double sigma_energy = a_config.fetch_real("sigma_energy");
    if (! a_config.has_explicit_unit("sigma_energy")) {
      sigma_energy *= energy_unit;
    }
    set_sigma_energy(sigma_energy);

    // Initialization :
    if (_raw_data_bank_label_.empty()) {
      set_raw_data_bank_label(DEFAULT_RD_BANK_LABEL);
    }

    _prng_.initialize("taus2", prng_seed);

    _set_initialized(true);
  }

  void raw_generator_module::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Raw generator module '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    _prng_.reset();
    _set_defaults();
  }

  int raw_generator_module::process(datatools::things & a_data_record)
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Raw generator module '" << get_name() << "' is not initialized !");

    // Pointer to the embeded ``dpp_ex01::raw_data`` instance :
    raw_data * RD = 0;
    if (! a_data_record.has(_raw_data_bank_label_)) {
      // Add a new bank of type ``dpp_ex01::raw_data`` with requested name :
      RD = &a_data_record.add<dpp_ex01::raw_data>(_raw_data_bank_label_);
    } else {
      // Access the existing bank of type ``dpp_ex01::raw_data`` with requested name :
      DT_THROW_IF(! a_data_record.is_a<dpp_ex01::raw_data>(_raw_data_bank_label_),
                  std::logic_error,
                  "Data bank ``" << _raw_data_bank_label_
                  << "`` is not a ``dpp_ex01::raw_data`` instance !");
      RD = &a_data_record.grab<dpp_ex01::raw_data>(_raw_data_bank_label_);
    }

    // Randomize some hits :
    int number_of_hits = _prng_.poisson(_mean_number_of_hits_);
    double energy_sum = 0.0;
    for (int i = 0; i < number_of_hits; i++) {
      {
        dpp_ex01::hit dummy;
        RD->grab_hits().push_back(dummy);
      }
      dpp_ex01::hit & h = RD->grab_hits().back();
      h.set_id(i);
      double energy = _prng_.gaussian(_mean_energy_, _sigma_energy_);
      if (energy < 0.0) energy = 1.0;
      h.set_energy(energy);
      energy_sum += energy;
      h.grab_auxiliaries().store_flag("raw");
    }

    // Check some condition and store a special flag :
    if (energy_sum > 15 * CLHEP::MeV) {
      RD->grab_auxiliaries().store_flag("high_energy");
    }

    return dpp::PROCESS_OK;
  }

  void raw_generator_module::set_raw_data_bank_label(const std::string & rd_bank_label_)
  {
    _raw_data_bank_label_ = rd_bank_label_;
  }

  const std::string & raw_generator_module::get_raw_data_bank_label() const
  {
    return _raw_data_bank_label_;
  }

  void raw_generator_module::set_mean_number_of_hits(double mnoh_)
  {
    DT_THROW_IF(mnoh_ <= 0.0,
                std::domain_error,
                "Invalid mean number of hits !");
    _mean_number_of_hits_ = mnoh_;
  }

  double raw_generator_module::get_mean_number_of_hits() const
  {
    return _mean_number_of_hits_;
  }

  void raw_generator_module::set_mean_energy(double me_)
  {
    DT_THROW_IF(me_ <= 0.0,
                std::domain_error,
                "Invalid mean energy !");
    _mean_energy_ = me_;
  }

  double raw_generator_module::get_mean_energy() const
  {
    return _mean_energy_;
  }

  void raw_generator_module::set_sigma_energy(double se_)
  {
    DT_THROW_IF(se_ <= 0.0,
                std::domain_error,
                "Invalid sigma energy !");
    _sigma_energy_ = se_;
  }

  double raw_generator_module::get_sigma_energy() const
  {
    return _sigma_energy_;
  }

} // namespace dpp_ex01


// OCD support for class 'dpp_ex01::raw_generator_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp_ex01::raw_generator_module,ocd_)
{
  ocd_.set_class_name ("dpp_ex01::raw_generator_module");
  ocd_.set_class_library ("dpp_ex01");
  ocd_.set_class_description ("A module to randomly generate a ``dpp_ex01::raw_data`` instance within each data/event record");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates debugging output.          \n"
                            "It is not recommended for a production run.    \n"
                            "Default value is false.                        \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "  debug : boolean = 0                          \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("rd_bank_label")
      .set_terse_description("The name/label of the target raw data bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value is : ``\"RD\"``.                 \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "  rd_bank_label : string = \"RD2\"             \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("prng_seed")
      .set_terse_description("The seed for the embeded random number generator")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("Default value is : ``0``.                      \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "  prng_seed : integer = 314159                 \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mean_number_of_hits")
      .set_terse_description("The mean number of raw hits to be generated")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("Example::                                      \n"
                            "                                               \n"
                            "  mean_number_of_hits : real = 3.45            \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("energy_unit")
      .set_terse_description("The symbol of the implicit energy unit")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value is : \"MeV\".                    \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "  energy_unit : string = \"keV\"               \n"
                            "                                               \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mean_energy")
      .set_terse_description("The mean energy of raw hits to be generated")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("Example with explicit unit::                   \n"
                            "                                               \n"
                            "  mean_energy : real = 1.35 MeV                \n"
                            "                                               \n"
                            "or using implicit unit::                       \n"
                            "                                               \n"
                            "  energy_unit : string = \"keV\"               \n"
                            "  mean_energy : real = 1350                    \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("sigma_energy")
      .set_terse_description("The energy standard deviation of raw hits to be generated")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("Example with explicit unit::                   \n"
                            "                                               \n"
                            "  sigma_energy : real = 26.4 keV               \n"
                            "                                               \n"
                            "or using implicit unit::                       \n"
                            "                                               \n"
                            "  energy_unit : string = \"keV\"               \n"
                            "  sigma_energy : real = 26.4                   \n"
                            "                                               \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A ``dpp_ex01::raw_generator_module`` object can be setup with the    \n"
                                "following syntax in a ``datatools::multi_properties`` configuration  \n"
                                "file, typically from a module manager object.                        \n"
                                "                                                                     \n"
                                "Example::                                                            \n"
                                "                                                                     \n"
                                "  #@description A module that generates raw data                     \n"
                                "  #@key_label   \"name\"                                             \n"
                                "  #@meta_label  \"type\"                                             \n"
                                "                                                                     \n"
                                "  [name=\"rgm\" type=\"dpp_ex01::raw_generator_module\"]             \n"
                                "  #@config A raw data generator module                               \n"
                                "  debug : boolean = 0                                                \n"
                                "  prng_seed : integer = 314159                                       \n"
                                "  rd_bank_label : string = \"RD\"                                    \n"
                                "  mean_number_of_hits : real = 3.45                                  \n"
                                "  energy_unit : string = \"keV\"                                     \n"
                                "  mean_energy : real = 1350                                          \n"
                                "  sigma_energy : real = 26.4                                         \n"
                                "                                                                     \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::dpp_ex01::raw_generator_module,"dpp_ex01::raw_generator_module")

