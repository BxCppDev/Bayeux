// wdecay0.cc
/*
 * Copyright 2013-2020 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public 3.0 License as published by
 * the Free Software Foundation; either version 3 of the License, or(at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public 3.0 License for more details.
 *
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <genbb_help/wdecay0.h>

// Standard library:
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

// Third party:
// - Bayeux/datatools:
#include <bayeux/version.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
// - BxDecay0:
#include <bxdecay0/version.h>
#include <bxdecay0/event.h>
#include <bxdecay0/decay0_generator.h>
#include <bxdecay0/bb.h>
#include <bxdecay0/bb_utils.h>
#include <bxdecay0/genbbsub.h>

// This project:
#include <genbb_help/genbb_utils.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/pdg_particle_tools.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(wdecay0, "genbb::wdecay0")

  // static
  const int wdecay0::DBD_MODE_INVALID;
  
  /// \brief Wrapper functor for a GSL random number generator
  class gsl_random
    : public bxdecay0::i_random
  {
  public:

    /// Constructor
    gsl_random(mygsl::rng & generator_)
      : _generator_(generator_)
    {
      return;
    }

    ~gsl_random() override {};

    /// Main operator
    double operator()() override
    {
      return _generator_.uniform();
    }

  private:

    /// Handle to a GSL based Bayeux/mygsl uniform deviates random engine
    mygsl::rng & _generator_; 
 
  };

  /// \brief Private implementation of working data and objects:
  struct wdecay0::pimpl_type {

    /// Attributes:
    bxdecay0::decay0_generator  generator;  ///< BxDecay0's generator
    std::unique_ptr<gsl_random> prng;       ///< Wrapper for GSL PRNG 

    /// Destructor
    ~pimpl_type()
    {
      if (generator.is_initialized()) {
        generator.reset();
      }
      return;
    }

  };

  bool wdecay0::is_initialized() const
  {
    return _initialized_;
  }

  wdecay0::wdecay0() : i_genbb()
  {
    _initialized_ = false;
    _event_count_ = 0;
    _decay_type_ = DECAY_TYPE_UNDEFINED;
    _decay_isotope_ = "";
    _decay_version_ = 0;
    _decay_dbd_level_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;
    _seed_ = 0;
    datatools::invalidate(_energy_min_);
    datatools::invalidate(_energy_max_);
    _pimpl_.reset(new pimpl_type);
    return;
  }

  wdecay0::~wdecay0()
  {
    _pimpl_.reset();
    if (_initialized_) {
      reset();
    }
    return;
  }

  bool wdecay0::can_external_random() const
  {
    return true;
  }

  const mygsl::rng & wdecay0::get_random() const
  {
    if (has_external_random()) {
      return get_external_random();
    }
    return _random_;
  }

  mygsl::rng & wdecay0::grab_random()
  {
    if (has_external_random()) {
      return grab_external_random();
    }
    return _random_;
  }

  void wdecay0::_reset_()
  {

    if (_pimpl_.get() != nullptr) {
      _pimpl_->prng.reset();
      _pimpl_->generator.reset();
      _pimpl_.reset();
    }

    _event_count_ = 0;
    _decay_type_ = DECAY_TYPE_UNDEFINED;
    _decay_isotope_.clear();
    _decay_dbd_level_ = 0;
    _decay_version_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;
    datatools::invalidate(_energy_min_);
    datatools::invalidate(_energy_max_);
    _seed_ = 0;
    if (! has_external_random()) {
      if (_random_.is_initialized()) {
        _random_.reset();
      }
    }
    return;
  }

  void wdecay0::reset()
  {
    if (! _initialized_) {
      return;
    }
    _initialized_ = false;
    _reset_();
    return;
  }

  void wdecay0::initialize(const datatools::properties & config_,
                           datatools::service_manager & /*service_manager_*/,
                           detail::pg_dict_type & /*dictionary_*/)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());
    DT_THROW_IF(_initialized_, std::logic_error, "Already initialized !");
    _initialize_base(config_);

    std::string decay_isotope;
    double energy_unit = CLHEP::MeV;
    double emin = 0.0;
    double emax = -1.0;
    datatools::invalidate(emin);
    datatools::invalidate(emax);

    if (! has_external_random()) {
      if (config_.has_key("seed")) {
        long seed = config_.fetch_integer("seed");
        DT_THROW_IF(seed < 0, std::logic_error, "Invalid seed value(>=0) !");
        _seed_ = seed;
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing 'seed' property !");
      }
    }

    DT_THROW_IF(! config_.has_key("decay_type"),
                std::logic_error,
                "Missing 'decay_type' property !");
    std::string tmp = config_.fetch_string("decay_type");
    DT_THROW_IF((tmp != "DBD") &&(tmp != "background"),
                std::logic_error,
                "Invalid decay type '" << tmp << "' !");
    if (tmp == "background") {
      _decay_type_ = bxdecay0::decay0_generator::DECAY_CATEGORY_BACKGROUND;
    }

    if (tmp == "DBD") {
      _decay_type_ = DECAY_TYPE_DBD;
      DT_THROW_IF(! config_.has_key("decay_dbd_level"),
                  std::logic_error,
                  "Missing DBD decay level !");
      _decay_dbd_level_ = config_.fetch_integer("decay_dbd_level");
      
      if (config_.has_key("decay_dbd_mode")) {
        _decay_dbd_mode_ = config_.fetch_integer("decay_dbd_mode");
      }
      
      if (config_.has_key("decay_dbd_mode_label")) {
        std::string dbd_mode_label = config_.fetch_string("decay_dbd_mode_label");
        _decay_dbd_mode_ = bxdecay0::dbd_mode_from_label(dbd_mode_label);
      }      
      DT_THROW_IF(_decay_dbd_mode_ == DBD_MODE_INVALID,
                  std::logic_error,
                  "Missing DBD decay mode !");
    }

    DT_THROW_IF(! config_.has_key("decay_isotope"),
                std::logic_error,
                "Missing 'decay_isotope' property !");
    decay_isotope = config_.fetch_string("decay_isotope");

    _set_decay_isotope_(decay_isotope);

    if (_decay_type_ == DECAY_TYPE_DBD) {
      bxdecay0::dbd_mode_type bxdecay0_dbdm = static_cast<bxdecay0::dbd_mode_type>(_decay_dbd_mode_);
      if (bxdecay0::dbd_supports_esum_range(bxdecay0_dbdm)) {
        
        if (config_.has_key("energy_unit")) {
          std::string unit_str = config_.fetch_string("energy_unit");
          energy_unit = datatools::units::get_energy_unit_from(unit_str);
        }
        
        if (config_.has_key("energy_max")) {
          emax = config_.fetch_real("energy_max");
          DT_THROW_IF(emax < 0.0, std::logic_error, "Invalid maximum value !");
          if (! config_.has_explicit_unit("energy_max")) emax *= energy_unit;
          _energy_max_ = emax;
        }

        if (config_.has_key("energy_min")) {
          emin = config_.fetch_real("energy_min");
          DT_THROW_IF(emin < 0.0, std::logic_error, "Invalid minimum value !");
          if (! config_.has_explicit_unit("energy_min")) emin *= energy_unit;
          _energy_min_ = emin;
        }

      }
    }

    if (config_.has_key("decay_version")) {
      int decay_version = config_.fetch_integer("decay_version");
      _set_decay_version_(decay_version);
    }

    if (datatools::is_valid(_energy_min_)) {
      if (! datatools::is_valid(_energy_max_)) {
        _energy_max_ = utils::DEFAULT_ENERGY_RANGE_MAX;
      }
    }
    if (datatools::is_valid(_energy_max_)) {
      if (! datatools::is_valid(_energy_min_)) {
        _energy_min_ = utils::DEFAULT_ENERGY_RANGE_MIN;
      }
    }
    if (datatools::is_valid(_energy_max_) and datatools::is_valid(_energy_min_)) {
      DT_THROW_IF(_energy_min_ >= _energy_max_, std::logic_error, "Invalid energy range !");
    }
    
    if (! has_external_random()) {
      DT_LOG_NOTICE(get_logging_priority(),"Initializing the local PRNG...");
      _random_.init("taus2", _seed_);
    }

    DT_LOG_DEBUG(get_logging_priority(), "Using BxDecay0...");
    _init_();

    _initialized_ = true;
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }

  bool wdecay0::has_next()
  {
    return _initialized_;
  }

  void wdecay0::_set_decay_isotope_(const std::string & di_)
  {
    _decay_isotope_ = di_;
    return;
  }

  void wdecay0::_set_decay_version_(int ver_)
  {
    _decay_version_ = ver_;
    return;
  }

  void wdecay0::_load_next(primary_event & event_,
                           bool compute_classification_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF(! _initialized_, std::logic_error, "Not initialized !");
    // reset:
    event_.reset();

    // Use the external BxDecay0:
    bxdecay0::event decay;
    _pimpl_->generator.shoot(*_pimpl_->prng, decay);
    if (datatools::logger::is_debug(get_logging_priority())) {
      decay.print(std::cerr, "BxDecay0 original event:", "[debug] ");
    }
    if (decay.has_time()) {
      event_.set_time(decay.get_time() * CLHEP::second);
    }
    if (decay.has_generator()) {
      //event_.set_label(decay.get_generator());
      event_.grab_auxiliaries().store("generator.library", "BxDecay0");
      event_.grab_auxiliaries().store("generator.library_version", BXDECAY0_LIB_VERSION);
      event_.grab_auxiliaries().store("generator", decay.get_generator());
    }
    for (const auto & particle : decay.get_particles()) {
      primary_particle & new_particle = event_.add_particle();
      if (particle.has_code()) {
        bxdecay0::particle_code pcode = particle.get_code();
        if (pcode == bxdecay0::GAMMA) {
          new_particle.set_type(primary_particle::GAMMA);
          new_particle.set_pdg_code(genbb::pdg::particle::GAMMA);
        } else if (pcode == bxdecay0::POSITRON) {
          new_particle.set_type(primary_particle::POSITRON);
          new_particle.set_pdg_code(genbb::pdg::particle::POSITRON);
        } else if (pcode == bxdecay0::ELECTRON) {
          new_particle.set_type(primary_particle::ELECTRON);
          new_particle.set_pdg_code(genbb::pdg::particle::ELECTRON);
        } else if (pcode == bxdecay0::NEUTRON) {
          new_particle.set_type(primary_particle::NEUTRON);
          new_particle.set_pdg_code(genbb::pdg::particle::NEUTRON);
        } else if (pcode == bxdecay0::PROTON) {
          new_particle.set_type(primary_particle::PROTON);
          new_particle.set_pdg_code(genbb::pdg::particle::PROTON);
        } else if (pcode == bxdecay0::ALPHA) {
          new_particle.set_type(primary_particle::ALPHA);
          new_particle.set_pdg_code(genbb::pdg::particle::ALPHA);
        }
      }
      if (particle.has_time()) {
        new_particle.set_time(particle.get_time() * CLHEP::second);
      }
      if (particle.has_momentum()) {
        new_particle.set_momentum(geomtools::vector_3d(particle.get_px() * CLHEP::MeV,
                                                       particle.get_py() * CLHEP::MeV,
                                                       particle.get_pz() * CLHEP::MeV));
      }
    }
    if (datatools::logger::is_debug(get_logging_priority())) {
      boost::property_tree::ptree prtoptions;
      prtoptions.put("title", "Genbb::primary event: ");
      prtoptions.put("indent", "[debug] ");
      event_.print_tree(std::cerr, prtoptions);   
    }
   
    event_.set_label(i_genbb::get_name());
    if (compute_classification_) {
      event_.compute_classification();
    }
    event_.set_genbb_weight(1.0);
    DT_LOG_DEBUG(get_logging_priority(), "GENBB weight = " << get_to_all_events());
    if (get_to_all_events() > 1.0) {
      event_.set_genbb_weight(1.0 / get_to_all_events());
    }
    _event_count_++;
    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  size_t wdecay0::get_event_count() const
  {
    return _event_count_;
  }

  double wdecay0::get_to_all_events() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Decay0 wrapper generator is not initialized!");
    return _pimpl_->generator.get_to_all_events();
  }

  void wdecay0::_init_()
  {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    DT_THROW_IF(_decay_type_ == DECAY_TYPE_UNDEFINED, std::logic_error,
                "Decay type is not defined !");
    DT_THROW_IF(_decay_isotope_.empty(), std::logic_error,
                "Missing isotope !");
    if (_decay_type_ == DECAY_TYPE_DBD) {
      DT_THROW_IF(_decay_dbd_level_ < 0, std::logic_error,
                  "Invalid DBD level !");
      DT_THROW_IF(_decay_dbd_mode_ == DBD_MODE_INVALID, std::logic_error,
                  "Invalid DBD mode !");
    }
    
    DT_LOG_DEBUG(get_logging_priority(),"Using BxDecay0...");
    if (_decay_type_ == DECAY_TYPE_DBD) {
      DT_LOG_DEBUG(get_logging_priority(),"decay_type_ == DECAY_TYPE_DBD");
      if (datatools::logger::is_debug(get_logging_priority())) {
        _pimpl_->generator.set_debug(true);
      }
      _pimpl_->generator.set_decay_category(bxdecay0::decay0_generator::DECAY_CATEGORY_DBD);
      _pimpl_->generator.set_decay_isotope(_decay_isotope_);
      _pimpl_->generator.set_decay_dbd_level(_decay_dbd_level_);
      _pimpl_->generator.set_decay_dbd_mode(static_cast<bxdecay0::dbd_mode_type>(_decay_dbd_mode_));
      double energy_min_MeV = utils::DEFAULT_ENERGY_RANGE_MIN / CLHEP::MeV;
      double energy_max_MeV = utils::DEFAULT_ENERGY_RANGE_MAX / CLHEP::MeV;
      bool ranged_energy = false;
      if (datatools::is_valid(_energy_min_)) {
        energy_min_MeV = _energy_min_ / CLHEP::MeV;
        DT_LOG_NOTICE(get_logging_priority(), "Setting DBD energy min to "
                      << energy_min_MeV << " MeV");
        ranged_energy = true;
      }
      if (datatools::is_valid(_energy_max_)) {
        energy_max_MeV = _energy_max_ / CLHEP::MeV;
        DT_LOG_NOTICE(get_logging_priority(), "Setting DBD energy max to "
                      << energy_max_MeV << " MeV");
        ranged_energy = true;
      }
      if (ranged_energy) {
        DT_LOG_DEBUG(get_logging_priority(),"ranged_energy");
        _pimpl_->generator.set_decay_dbd_esum_range(energy_min_MeV, energy_max_MeV);
      }
    } else if (_decay_type_ == DECAY_TYPE_BACKGROUND) {
      DT_LOG_DEBUG(get_logging_priority(),"decay_type_ == DECAY_TYPE_BACKGROUND");
      _pimpl_->generator.set_decay_category(bxdecay0::decay0_generator::DECAY_CATEGORY_BACKGROUND);
      _pimpl_->generator.set_decay_isotope(_decay_isotope_);
    }
    _pimpl_->prng.reset(new gsl_random(grab_random()));
    _pimpl_->generator.initialize(*_pimpl_->prng);

    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  void wdecay0::tree_dump(std::ostream& a_out,
                          const std::string& a_title,
                          const std::string& a_indent,
                          bool a_inherit) const
  {
    this->i_genbb::tree_dump(a_out, a_title, a_indent, true);
    const std::string & indent = a_indent;
    a_out << indent << i_tree_dumpable::tag
          << "Decay type       : " << _decay_type_ << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Decay isotope    : '" << _decay_isotope_ << "'" << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Decay DBD level  : " << _decay_dbd_level_ << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Decay DBD mode   : " << _decay_dbd_mode_ << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Event count      : " << _event_count_ << std::endl;
    if (datatools::is_valid(_energy_min_)) {
      a_out << indent << i_tree_dumpable::tag
            << "Energy min       : " << _energy_min_ / CLHEP::keV << " keV" << std::endl;
    }
    if (datatools::is_valid(_energy_max_)) {
      a_out << indent << i_tree_dumpable::tag
            << "Energy max       : " << _energy_max_ / CLHEP::keV << " keV" << std::endl;
    }
    a_out << indent << i_tree_dumpable::tag
          << "Local PRNG : " << _random_.is_initialized() << std::endl;
    a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
          << "Local PRNG seed             : " << _seed_ << std::endl;
    return;
  }

  void wdecay0::dump(std::ostream & out_) const
  {
    tree_dump(out_, "C++ GENBB wrapper: ", "");
    return;
  }

} // end of namespace genbb

  /***************
   * OCD support *
   ***************/

  // OCD support for class '::genbb::wdecay0' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genbb::wdecay0,ocd_)
{
  ocd_.set_class_name("genbb::wdecay0");
  ocd_.set_class_description("The C++ wrapper of Decay0 event generator");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::wdecay0`` class is the C++ wrapper of the \n"
                               "Decay0 event generator by Vladimir Tretyak.            \n"
                               "It provides a set of event generators for:             \n"
                               "                                                       \n"
                               " * various double beta decay(DBD) processes for       \n"
                               "   most isotopes of experimental interest,             \n"
                               " * various nuclear decay processes for                 \n"
                               "   common radioactive isotopes.                        \n"
                               "                                                       \n"
                               );

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("name")
      .set_terse_description("The name of the generator")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The name of the generator. This can only be   \n"
                            "used if the name has not been set by some     \n"
                            "external process(i.e. genbb::manager).       \n"
                            "                                              \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "   name : string = \"PrimEvent\"              \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("seed")
      .set_terse_description("Embedded PRNG's seed")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("The seed of the embedded PRNG.                \n"
                            "Not used if some external PRNG is used.       \n"
                            "(i.e. genbb::manager).                        \n"
                            "                                              \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "  seed: integer = 314159                      \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("decay_type")
      .set_terse_description("The type of decay")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The type of the decay can take two values:    \n"
                            "                                              \n"
                            "  * ``\"DBD\"`` : double beta decay process,  \n"
                            "  * ``\"background\"`` : common radioactive   \n"
                            "    process.                                  \n"
                            "                                              \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "   decay_type: string = \"background\"        \n"
                            "                                              \n"
                            )
      ;
  }

  {
    const std::vector<std::string> & dbd_nuclides = genbb::utils::get_dbd_nuclides();
    std::ostringstream long_desc;
    long_desc << "The decaying isotope.                         \n"
              << "                                              \n"
              << "List of supported DBD emitter isotopes:       \n"
              << "                                              \n";
    for (size_t i = 0; i < dbd_nuclides.size(); i++) {
      long_desc << "  * ``\"" << dbd_nuclides[i] << "\"``  \n";
    }

    const std::vector<std::string> & bkg_nuclides = genbb::utils::get_background_nuclides();
    long_desc << "                                              \n"
              << "List of supported background isotopes:        \n"
              << "                                              \n";
    for (size_t i = 0; i < bkg_nuclides.size(); i++) {
      long_desc << "  * ``\"" << bkg_nuclides[i] << "\"``  \n";
    }

    long_desc <<
      "                                              \n"
      "Example of Tl208 decay: ::                    \n"
      "                                              \n"
      "   decay_type    : string = \"background\"    \n"
      "   decay_isotope : string = \"Tl208\"         \n"
      "                                              \n"
      "Example of DBD of Se82: ::                    \n"
      "                                              \n"
      "   decay_type    : string = \"DBD\"           \n"
      "   decay_isotope : string = \"Se82\"          \n"
      "                                              \n"
      ;

    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("decay_isotope")
      .set_terse_description("The decaying isotope")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description(long_desc.str()
                            )
      ;
  }

  // Not used :
  // {
  //   configuration_property_description & cpd = ocd_.add_property_info();
  //   cpd.set_name_pattern("decay_version")
  //     .set_terse_description("The version of the generator")
  //     .set_traits(datatools::TYPE_INTEGER)
  //     .set_mandatory(false)
  //     .set_default_value_integer(0)
  //     .set_long_description("The version number of the generator:          \n"
  //                           "                                              \n"
  //                           "  * ``0`` : The default version               \n"
  //                           "  * ``N`` : The version number N if available \n"
  //                           "                                              \n"
  //                           )
  //     .add_example("                                           \n"
  //               "                                              \n"
  //               "   decay_version : integer = 1                \n"
  //               "                                              \n"
  //               )
  //     ;
  // }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("decay_dbd_level")
      .set_terse_description("The daughter level for a DBD")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("decay_type", "DBD")
      .set_long_description("The daughter level for a DBD:                 \n"
                            "                                              \n"
                            "  * ``0`` : The ground state                  \n"
                            "  * ``N`` : The Nth excited state of the      \n"
                            "    daughter nucleus.                         \n"
                            "                                              \n"
                            "The number of available levels for a given    \n"
                            "DBD process depends on the emitter isotope.   \n"
                            "                                              \n"
                            "See: \"documentation/wdecay0/dbd_levels.rst\" \n"
                            "                                              \n"
                            "Example of DBD of Se82 to Kr82 ground state: ::\n"
                            "                                              \n"
                            "   decay_type      : string = \"DBD\"         \n"
                            "   decay_isotope   : string = \"Se82\"        \n"
                            "   decay_dbd_level : integer = 0              \n"
                            "                                              \n"
                            )
      ;
  }

  {
    const std::map<int,std::string> & dbd_modes = genbb::utils::get_dbd_modes();
    std::ostringstream long_desc;
    long_desc << "The mode of the DBD process.                  \n"
              << "                                              \n"
              << "List of supported DBD modes:                  \n"
              << "                                              \n";
    for (std::map<int,std::string>::const_iterator
           i = dbd_modes.begin();
         i != dbd_modes.end();
         i++) {
      long_desc << "  * ``" << i->first << "`` : " << i->second << "\n";
    }
    long_desc <<
      "                                              \n"
      "Example of DBD of Se82: ::                    \n"
      "                                              \n"
      "   decay_type      : string = \"DBD\"         \n"
      "   decay_isotope   : string = \"Se82\"        \n"
      "   decay_dbd_level : integer = 0              \n"
      "   decay_dbd_mode  : integer = 4              \n"
      "                                              \n"
      ;

    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("decay_dbd_mode")
      .set_terse_description("The mode of the DBD process")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("decay_type", "DBD")
      .set_long_description(long_desc.str()
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy_min")
      .set_terse_description("Minimum energy for DBD process")
      .set_traits(datatools::TYPE_REAL)
      .set_triggered_by_label("decay_type", "DBD")
      .set_complex_triggering_conditions(true)
      .set_long_description("The minimum energy for generating the         \n"
                            "energy spectrum of the DBD process.           \n"
                            "Only possible for DBD modes: 4, 5, 6, 8, 10, 13, 14.\n"
                            "                                              \n"
                            "Example for the DBD of Se82: ::               \n"
                            "                                              \n"
                            "   decay_type      : string  = \"DBD\"        \n"
                            "   decay_isotope   : string  = \"Se82\"       \n"
                            "   decay_dbd_level : integer = 0              \n"
                            "   decay_dbd_mode  : integer = 4              \n"
                            "   energy_min      : real    = 2.2 MeV        \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy_max")
      .set_terse_description("Maximum energy for DBD process")
      .set_traits(datatools::TYPE_REAL)
      .set_triggered_by_label("decay_type", "DBD")
      .set_complex_triggering_conditions(true)
      .set_long_description("The maximum energy for generating the         \n"
                            "energy spectrum of the DBD process.           \n"
                            "Only possible for DBD modes: 4, 5, 6, 8, 10, 13, 14.\n"
                            "                                              \n"
                            "Example for the DBD of Se82: ::               \n"
                            "                                              \n"
                            "   decay_type      : string  = \"DBD\"        \n"
                            "   decay_isotope   : string  = \"Se82\"       \n"
                            "   decay_dbd_level : integer = 0              \n"
                            "   decay_dbd_mode  : integer = 4              \n"
                            "   energy_min      : real    = 2.2 MeV        \n"
                            "   energy_max      : real    = 3.5 MeV        \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy_unit")
      .set_terse_description("The default energy unit")
      .set_traits(datatools::TYPE_STRING)
      .set_triggered_by_label("decay_type", "DBD")
      .set_long_description("The symbol of the default energy unit.        \n"
                            "                                              \n"
                            "Example: ::                                   \n"
                            "                                              \n"
                            "   energy_unit : string  = \"MeV\"            \n"
                            "   energy_min  : real    = 2.2                \n"
                            "   energy_max  : real    = 3.5                \n"
                            "                                              \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("Example: ::                                           \n"
                               "                                                      \n"
                               "  logging.priority : string  = \"notice\"             \n"
                               "  seed             : integer = 314159                 \n"
                               "  decay_type       : string  = \"background\"         \n"
                               "  decay_isotope    : string  = \"Tl208\"              \n"
                               "                                                      \n"
                               );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::wdecay0, "genbb::wdecay0")
