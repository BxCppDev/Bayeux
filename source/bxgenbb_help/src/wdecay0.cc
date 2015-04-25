// wdecay0.cc
/*
 * Copyright 2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public 3.0 License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

// This project:
#include <genbb_help/genbb_utils.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/bb.h>
#include <genbb_help/decay0/genbbsub.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(wdecay0,"genbb::wdecay0");


  // static
  const int wdecay0::DBD_MODE_INVALID;

  genbb::decay0::bbpars & wdecay0::bb_params()
  {
    return *_bb_params_.get();
  }

  bool wdecay0::is_initialized () const
  {
    return _initialized_;
  }

  wdecay0::wdecay0 () : i_genbb ()
  {
    _initialized_ = false;
    _event_count_ = 0;
    _decay_type_ = DECAY_TYPE_UNDEFINED;
    _decay_isotope_ = "";
    _decay_version_ = 0;
    _decay_dbd_level_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;
    _seed_ = 0;
    datatools::invalidate (_energy_min_);
    datatools::invalidate (_energy_max_);
    return;
  }

  wdecay0::~wdecay0 ()
  {
    if (_initialized_) {
      reset ();
    }
    return;
  }

  bool wdecay0::can_external_random () const
  {
    return true;
  }

  const mygsl::rng & wdecay0::get_random () const
  {
    if (has_external_random ()) {
      return get_external_random ();
    }
    return _random_;
  }

  mygsl::rng & wdecay0::grab_random ()
  {
    if (has_external_random ()) {
      return grab_external_random ();
    }
    return _random_;
  }

  void wdecay0::_reset_ ()
  {
    _event_count_ = 0;

    _decay_type_ = DECAY_TYPE_UNDEFINED;
    _decay_isotope_.clear();
    _decay_dbd_level_ = 0;
    _decay_version_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;

    datatools::invalidate (_energy_min_);
    datatools::invalidate (_energy_max_);

    _seed_ = 0;
    if (! has_external_random ()) {
      if (_random_.is_initialized ()) {
        _random_.reset ();
      }
    }

    if (_bb_params_.get() != 0) {
      _bb_params_.get()->reset();
      _bb_params_.reset(0);
    }
    return;
  }

  void wdecay0::reset ()
  {
    if (! _initialized_) {
      return;
    }
    _initialized_ = false;
    _reset_ ();
    return;
  }


  void wdecay0::initialize (const datatools::properties & config_,
                            datatools::service_manager & /*service_manager_*/,
                            detail::pg_dict_type & /*dictionary_*/)
  {
    DT_THROW_IF (_initialized_, std::logic_error, "Already initialized !");
    _initialize_base(config_);

    std::string decay_isotope;
    double energy_unit = CLHEP::MeV;
    double emin = 0.0;
    double emax = -1.0;
    datatools::invalidate (emin);
    datatools::invalidate (emax);

    if (! has_external_random ()) {
      if (config_.has_key ("seed")) {
        long seed = config_.fetch_integer ("seed");
        DT_THROW_IF (seed < 0, std::logic_error, "Invalid seed value (>=0) !");
        _seed_ = seed;
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing 'seed' property !");
      }
    }

    DT_THROW_IF (! config_.has_key ("decay_type"),
                 std::logic_error,
                 "Missing 'decay_type' property !");
    std::string tmp = config_.fetch_string ("decay_type");
    DT_THROW_IF ((tmp != "DBD") && (tmp != "background"),
                 std::logic_error,
                 "Invalid decay type '" << tmp << "' !");
    if (tmp == "background") {
      _decay_type_ = DECAY_TYPE_BACKGROUND;
    }

    if (tmp == "DBD") {
      _decay_type_ = DECAY_TYPE_DBD;
      DT_THROW_IF (! config_.has_key ("decay_dbd_level"),
                   std::logic_error,
                   "Missing DBD decay level !");
      _decay_dbd_level_ = config_.fetch_integer ("decay_dbd_level");

      DT_THROW_IF (! config_.has_key ("decay_dbd_mode"),
                   std::logic_error,
                   "Missing DBD decay mode !");
      _decay_dbd_mode_ = config_.fetch_integer ("decay_dbd_mode");
    }

    DT_THROW_IF (! config_.has_key ("decay_isotope"),
                 std::logic_error,
                 "Missing 'decay_isotope' property !");
    decay_isotope = config_.fetch_string ("decay_isotope");

    _set_decay_isotope_ (decay_isotope);

    if (_decay_type_ == DECAY_TYPE_DBD) {
      const std::vector<int> & dbdmwer
        = utils::get_dbd_modes_with_energy_range ();
      if (std::find (dbdmwer.begin (), dbdmwer.end (),_decay_dbd_mode_) != dbdmwer.end ()) {
        if (config_.has_key ("energy_unit")) {
          std::string unit_str = config_.fetch_string ("energy_unit");
          energy_unit = datatools::units::get_energy_unit_from (unit_str);
        }

        if (config_.has_key ("energy_max")) {
          emax = config_.fetch_real ("energy_max");
          DT_THROW_IF (emax < 0.0, std::logic_error, "Invalid maximum value !");
          if (! config_.has_explicit_unit("energy_max")) emax *= energy_unit;
          _energy_max_ = emax;
        }

        if (config_.has_key ("energy_min")) {
          emin = config_.fetch_real ("energy_min");
          DT_THROW_IF (emin < 0.0, std::logic_error, "Invalid minimum value !");
          if (! config_.has_explicit_unit("energy_min")) emin *= energy_unit;
          _energy_min_ = emin;
        }

      }
    }

    if (config_.has_key ("decay_version")) {
      int decay_version = config_.fetch_integer("decay_version");
      _set_decay_version_(decay_version);
    }

    if (datatools::is_valid (_energy_min_)) {
      if (! datatools::is_valid (_energy_max_)) {
        _energy_max_ = utils::DEFAULT_ENERGY_RANGE_MAX;
      }
    }
    if (datatools::is_valid (_energy_max_)) {
      if (! datatools::is_valid (_energy_min_)) {
        _energy_min_ = utils::DEFAULT_ENERGY_RANGE_MIN;
      }
    }
    DT_THROW_IF (_energy_min_ >= _energy_max_, std::logic_error, "Invalid energy range !");

    if (! has_external_random ()) {
      DT_LOG_NOTICE(get_logging_priority(),"Initializing the local PRNG...");
      _random_.init ("taus2", _seed_);
    }

    _init_ ();

    _initialized_ = true;
    return;
  }

  bool wdecay0::has_next ()
  {
    return _initialized_;
  }

  void wdecay0::_set_decay_isotope_ (const std::string & di_)
  {
    _decay_isotope_ = di_;
    return;
  }

  void wdecay0::_set_decay_version_ (int ver_)
  {
    _decay_version_ = ver_;
    return;
  }

  void wdecay0::_load_next (primary_event & event_,
                            bool compute_classification_)
  {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    DT_THROW_IF  (! _initialized_, std::logic_error, "Not initialized !");
    // reset:
    event_.reset ();

    int error = 0;
    if (_decay_type_ == DECAY_TYPE_DBD) {
      decay0::genbbsub (grab_random(),
                        event_,
                        decay0::GENBBSUB_I2BBS_DBD,
                        _decay_isotope_,
                        _decay_dbd_level_,
                        _decay_dbd_mode_,
                        decay0::GENBBSUB_ISTART_GENERATE,
                        error,
                        bb_params());
      DT_THROW_IF (error != 0, std::logic_error, "genbbsub DBD generation failed !");
    } else if (_decay_type_ == DECAY_TYPE_BACKGROUND) {
      decay0::genbbsub(grab_random(),
                       event_,
                       decay0::GENBBSUB_I2BBS_BACKGROUND,
                       _decay_isotope_,
                       -1,
                       -1,
                       decay0::GENBBSUB_ISTART_GENERATE,
                       error,
                       bb_params());
      DT_THROW_IF (error != 0, std::logic_error, "genbbsub background generation failed !");
    }
    event_.set_label (i_genbb::get_name ());
    if (compute_classification_) {
      event_.compute_classification ();
    }
    event_.set_genbb_weight (1.0);
    DT_LOG_DEBUG(get_logging_priority(), "GENBB weight = " << get_to_all_events ());
    if (get_to_all_events () > 1.0) {
      event_.set_genbb_weight (1.0 / get_to_all_events ());
    }
    _event_count_++;
    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  size_t wdecay0::get_event_count () const
  {
    return _event_count_;
  }

  double wdecay0::get_to_all_events () const
  {
    return _bb_params_.get()->toallevents;
  }

  void wdecay0::_init_ ()
  {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    DT_THROW_IF (_decay_type_ == DECAY_TYPE_UNDEFINED, std::logic_error,
                 "Decay type is not defined !");
    DT_THROW_IF (_decay_isotope_.empty(), std::logic_error,
                 "Missing isotope !");
    if (_decay_type_ == DECAY_TYPE_DBD) {
      DT_THROW_IF (_decay_dbd_level_ < 0, std::logic_error,
                   "Invalid DBD level !");
      DT_THROW_IF (_decay_dbd_mode_ == DBD_MODE_INVALID, std::logic_error,
                   "Invalid DBD mode !");
    }

    _bb_params_.reset(new genbb::decay0::bbpars);

    bb_params().reset();
    if (_decay_type_ == DECAY_TYPE_DBD) {
      bb_params().modebb   = _decay_dbd_mode_;
      bb_params().istartbb = 0;

      const std::vector<int> & dbdmwer
        = utils::get_dbd_modes_with_energy_range ();
      DT_LOG_DEBUG(get_logging_priority(), "Decay DBD mode : " << _decay_dbd_mode_);
      if (std::find (dbdmwer.begin (), dbdmwer.end (), _decay_dbd_mode_) != dbdmwer.end ()) {
        if (datatools::is_valid (_energy_min_)) {
          DT_LOG_NOTICE(get_logging_priority(), "Setting DBD energy min to "
                        << _energy_min_ / CLHEP::MeV << " MeV");
          bb_params().ebb1 = (float) (_energy_min_ / CLHEP::MeV);
        }
        if (datatools::is_valid (_energy_max_)) {
          DT_LOG_NOTICE(get_logging_priority(), "Setting DBD energy max to "
                        << _energy_max_ / CLHEP::MeV << " MeV");
          bb_params().ebb2 = (float) (_energy_max_ / CLHEP::MeV);
        }
        DT_THROW_IF (bb_params().ebb1 >= bb_params().ebb2,
                     std::logic_error,
                     "Invalid DBD energy range (Emin="<< bb_params().ebb1 << " >= Emax=" << bb_params().ebb2 << ") (MeV) !");
      } else {
        DT_LOG_DEBUG(get_logging_priority(),"Not a DBD energy range mode.");
      }
    }

    int error = 0;
    if (_decay_type_ == DECAY_TYPE_DBD) {
      primary_event dummy_event;
      decay0::genbbsub(grab_random(),
                       dummy_event,
                       decay0::GENBBSUB_I2BBS_DBD,
                       _decay_isotope_,
                       _decay_dbd_level_,
                       _decay_dbd_mode_,
                       decay0::GENBBSUB_ISTART_INIT, // initialization without event generation
                       error,
                       bb_params());
      DT_THROW_IF (error != 0, std::logic_error, "genbbsub DBD initialization failed !");
    } else if (_decay_type_ == DECAY_TYPE_BACKGROUND) {
      primary_event dummy_event;
      decay0::genbbsub(grab_random(),
                       dummy_event,
                       decay0::GENBBSUB_I2BBS_BACKGROUND,
                       _decay_isotope_,
                       -1,
                       -1,
                       decay0::GENBBSUB_ISTART_INIT, // initialization without event generation
                       error,
                       bb_params());
      DT_THROW_IF (error != 0, std::logic_error, "genbbsub background initialization failed !");
    }

    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  void wdecay0::tree_dump (std::ostream& a_out,
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
    if (datatools::is_valid (_energy_min_)) {
      a_out << indent << i_tree_dumpable::tag
            << "Energy min       : " << _energy_min_ / CLHEP::keV << " keV" << std::endl;
    }
    if (datatools::is_valid (_energy_max_)) {
      a_out << indent << i_tree_dumpable::tag
            << "Energy max       : " << _energy_max_ / CLHEP::keV << " keV" << std::endl;
    }
    a_out << indent << i_tree_dumpable::tag
          << "Local PRNG : " << _random_.is_initialized() << std::endl;
    a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
          << "Local PRNG seed             : " << _seed_ << std::endl;
    return;
  }

  void wdecay0::dump (std::ostream & out_) const
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
  ocd_.set_class_name ("genbb::wdecay0");
  ocd_.set_class_description ("The C++ port of the Genbb/Decay0 event generator");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::wdecay0`` class is the C++ port of the    \n"
                               "Genbb/Decay0 event generator by Vladimir Tretyak.      \n"
                               "It provides a set of event generators for:             \n"
                               "                                                       \n"
                               " * various double beta decay processes for             \n"
                               "   isotopes of experimental interest,                  \n"
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
                            "external process.                             \n"
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
    const std::vector<std::string> & dbd_nuclides = genbb::utils::get_dbd_nuclides ();
    std::ostringstream long_desc;
    long_desc << "The decaying isotope.                         \n"
              << "                                              \n"
              << "List of supported DBD emitter isotopes:       \n"
              << "                                              \n";
    for (size_t i = 0; i < dbd_nuclides.size(); i++) {
      long_desc << "  * ``\"" << dbd_nuclides[i] << "\"``  \n";
    }

    const std::vector<std::string> & bkg_nuclides = genbb::utils::get_background_nuclides ();
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
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("decay_version")
      .set_terse_description("The version of the generator")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_default_value_integer(0)
      .set_long_description("The version number of the generator:          \n"
                            "                                              \n"
                            "  * ``0`` : The default version               \n"
                            "  * ``N`` : The version number N if available \n"
                            "                                              \n"
                            )
      .add_example("Generator for the                                            \n"
                            "Example of DBD of Se82: ::                    \n"
                            "                                              \n"
                            "   decay_type      : string = \"DBD\"         \n"
                            "   decay_isotope   : string = \"Se82\"        \n"
                            "   decay_dbd_level : integer = 0              \n"
                            "                                              \n"
                            )
      ;
  }

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
                            "The number of available labels for a given    \n"
                            "DBD process depends on the emitter isotope.   \n"
                            "                                              \n"
                            "TODO: documentation needed.                   \n"
                            "                                              \n"
                            "Example of DBD of Se82: ::                    \n"
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

  ocd_.set_configuration_hints ("Example::                                             \n"
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
