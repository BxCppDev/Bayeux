// -*- mode: c++; -*-
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

#include <genbb_help/wdecay0.h>

#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <datatools/utils.h>
#include <datatools/units.h>

#include <genbb_help/genbb_utils.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/bb.h>
#include <genbb_help/decay0/genbbsub.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(wdecay0,"genbb::wdecay0");

  genbb::decay0::bbpars & wdecay0::bb_params()
  {
    return *_bb_params_.get();
  }

  bool wdecay0::is_initialized () const
  {
    return _initialized_;
  }

  bool wdecay0::is_debug () const
  {
    return _debug_;
  }

  void wdecay0::set_debug (bool d_)
  {
    _debug_ = d_;
    return;
  }

  // ctor:
  wdecay0::wdecay0 () : i_genbb ()
  {
    _debug_ = false;
    _initialized_ = false;
    _event_count_ = 0;
    _decay_type_ = DECAY_TYPE_UNDEFINED;
    _decay_isotope_ = "";
    _decay_dbd_level_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;
    _seed_ = 0;
    datatools::invalidate (_energy_min_);
    datatools::invalidate (_energy_max_);
    return;
  }

  // dtor:
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
                            datatools::service_manager & service_manager_,
                            detail::pg_dict_type & dictionary_)
  {
    if (_initialized_) {
      throw std::logic_error ("genbb::wdecay0::initialize: Already initialized !");
    }

    std::string decay_isotope;
    double energy_unit = CLHEP::MeV;
    double emin = 0.0;
    double emax = -1.0;
    datatools::invalidate (emin);
    datatools::invalidate (emax);

    if (config_.has_flag ("debug")) {
      _debug_ = true;
    }

    if (! has_external_random ()) {

      if (config_.has_key ("seed")) {
        long seed = config_.fetch_integer ("seed");
        if (seed < 0) {
          throw std::logic_error ("genbb::wdecay0::initialize: Invalid seed value (>=0) !");
        }
        _seed_ = seed;
      } else {
          throw std::logic_error ("genbb::wdecay0::initialize: Missing 'seed' property !");
      }
    }

    if (! config_.has_key ("decay_type")) {
      std::ostringstream message;
      message << "genbb::wdecay0::initialize: Missing 'decay_type' property !";
      throw std::logic_error (message.str());
    } else {
      std::string tmp = config_.fetch_string ("decay_type");
      if ((tmp != "DBD") && (tmp != "background")) {
        std::ostringstream message;
        message << "genbb::wdecay0::initialize: Invalid decay type '"
                << tmp << "' !";
        throw std::logic_error (message.str());
      }
      if (tmp == "background") {
        _decay_type_ = DECAY_TYPE_BACKGROUND;
      }

      if (tmp == "DBD") {
        _decay_type_ = DECAY_TYPE_DBD;

        if (! config_.has_key ("decay_dbd_level")) {
          std::ostringstream message;
          message << "genbb::wdecay0::initialize: Missing DBD decay level !";
          throw std::logic_error (message.str());
        }
        _decay_dbd_level_ = config_.fetch_integer ("decay_dbd_level");

        if (! config_.has_key ("decay_dbd_mode")) {
          std::ostringstream message;
          message << "genbb::wdecay0::initialize: Missing DBD decay mode !";
          throw std::logic_error (message.str());
        }
        _decay_dbd_mode_ = config_.fetch_integer ("decay_dbd_mode");
      }

    }

    if (! config_.has_key ("decay_isotope")) {
      std::ostringstream message;
      message << "genbb::wdecay0::initialize: Missing 'decay_isotope' property !";
      throw std::logic_error (message.str());
    } else {
      decay_isotope = config_.fetch_string ("decay_isotope");
    }

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
          if (emax < 0.0) {
            throw std::logic_error ("genbb::wdecay0::initialize: Invalid maximum value !");
          }
          if (! config_.has_explicit_unit("energy_max")) emax *= energy_unit;
          _energy_max_ = emax;
        }

        if (config_.has_key ("energy_min")) {
          emin = config_.fetch_real ("energy_min");
          if (emin < 0.0) {
            throw std::logic_error ("genbb::wdecay0::initialize: Invalid minimum value !");
          }
          if (! config_.has_explicit_unit("energy_min")) emin *= energy_unit;
          _energy_min_ = emin;
        }

      }
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
    if (_energy_min_ >= _energy_max_) {
      std::ostringstream message;
      message << "genbb::wdecay0::initialize: Invalid energy range !";
      throw std::logic_error (message.str());
    }

    if (! has_external_random ()) {
      std::clog << "NOTICE: " << "genbb::wdecay0::initialize: "
                << "Initializing the local PRNG..." << std::endl;
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

  void wdecay0::_load_next (primary_event & event_,
                            bool compute_classification_)
  {
    if (_debug_) {
      std::clog << "debug: " << "genbb::wdecay0::_load_next: "
                << "Entering..."
                << std::endl;
    }
    if (! _initialized_) {
      throw std::logic_error ("genbb::wdecay0::_load_next: Not initialized !");
    }

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
      if (error != 0) {
        throw std::logic_error ("genbb::wdecay0::_load_next: genbbsub DBD generation failed !");
      }
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
      if (error != 0) {
        throw std::logic_error ("genbb::wdecay0::_init_: genbbsub background generation failed !");
      }
    }

    if (compute_classification_) {
      event_.compute_classification ();
    }

    event_.set_genbb_weight (1.0);
    if (_debug_) {
      std::clog << "debug: " << "genbb::wdecay0::_load_next: "
                << "GENBB weight = " << get_to_all_events ()
                << std::endl;
    }

    if (get_to_all_events () > 1.0) {
      event_.set_genbb_weight (1.0 / get_to_all_events ());
    }

    _event_count_++;
    if (_debug_) {
      std::clog << "debug: " << "genbb::wdecay0::_load_next: "
                << "Exiting."
                << std::endl;
    }
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
    if (_debug_) {
      std::clog << "DEBUG: " << "genbb::wdecay0::_init_: "
                << "Entering..."
                << std::endl;
    }

    if (_decay_type_ == DECAY_TYPE_UNDEFINED) {
      throw std::logic_error ("genbb::wdecay0::_init_: Decay type is not defined !");
    }

    if (_decay_isotope_.empty()) {
      throw std::logic_error ("genbb::wdecay0::_init_: Missing isotope !");
    }

    if (_decay_type_ == DECAY_TYPE_DBD) {
      if (_decay_dbd_level_ < 0) {
        throw std::logic_error ("genbb::wdecay0::_init_: Invalid DBD level !");
      }
      if (_decay_dbd_mode_ == DBD_MODE_INVALID) {
        throw std::logic_error ("genbb::wdecay0::_init_: Invalid DBD mode !");
      }
    }

    _bb_params_.reset(new genbb::decay0::bbpars);

    bb_params().reset();
    if (_decay_type_ == DECAY_TYPE_DBD) {
      bb_params().modebb   = _decay_dbd_mode_;
      bb_params().istartbb = 0;

      const std::vector<int> & dbdmwer
        = utils::get_dbd_modes_with_energy_range ();
      if (_debug_) std::clog << "DEBUG: genbb::wdecay0::_init_: Decay DBD mode : "
                        << _decay_dbd_mode_ << std::endl;
      if (std::find (dbdmwer.begin (), dbdmwer.end (), _decay_dbd_mode_) != dbdmwer.end ()) {
        if (datatools::is_valid (_energy_min_)) {
          std::clog << "NOTICE: genbb::wdecay0::_init_: Setting DBD energy min to "
                    << _energy_min_ / CLHEP::MeV << " MeV" << std::endl;
          bb_params().ebb1 = (float) (_energy_min_ / CLHEP::MeV);
        }
        if (datatools::is_valid (_energy_max_)) {
          std::clog << "NOTICE: genbb::wdecay0::_init_: Setting DBD energy max to "
                    << _energy_max_ / CLHEP::MeV << " MeV" << std::endl;
          bb_params().ebb2 = (float) (_energy_max_ / CLHEP::MeV);
        }
        if (bb_params().ebb1 >= bb_params().ebb2) {
          std::ostringstream message;
          message << "genbb::wdecay0::_init_: "
                  << "Invalid DBD energy range (Emin="<< bb_params().ebb1 << " >= Emax=" << bb_params().ebb2 << ") (MeV) !";
          throw std::logic_error (message.str ());
        }
      } else {
        if (_debug_) std::clog << "DEBUG: genbb::wdecay0::_init_: Not a DBD energy range mode."
                               << std::endl;
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
      if (error != 0) {
        throw std::logic_error ("genbb::wdecay0::_init_: genbbsub DBD initialization failed !");
      }
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
      if (error != 0) {
        throw std::logic_error ("genbb::wdecay0::_init_: genbbsub background initialization failed !");
      }
    }

    if (_debug_) {
      std::clog << "DEBUG: " << "genbb::wdecay0::_init_: "
           << "Exiting."
           << std::endl;
    }
    return;
  }

  void wdecay0::dump (std::ostream & out_) const
  {
    const std::string tag = "|-- ";
    const std::string last_tag = "`-- ";
    out_ << "C++ GENBB wrapper: " << std::endl;
    out_ << tag << "debug            : " << _debug_ << std::endl;
    out_ << tag << "decay_type       : " << _decay_type_ << std::endl;
    out_ << tag << "decay_isotope    : '" << _decay_isotope_ << "'" << std::endl;
    out_ << tag << "decay_dbd_level  : " << _decay_dbd_level_ << std::endl;
    out_ << tag << "decay_dbd_mode   : " << _decay_dbd_mode_ << std::endl;
    out_ << tag << "event_count      : " << _event_count_ << std::endl;
    if (datatools::is_valid (_energy_min_)) {
      out_ << tag << "energy_min       : " << _energy_min_ / CLHEP::keV << " keV" << std::endl;
    }
    if (datatools::is_valid (_energy_max_)) {
      out_ << tag << "energy_max       : " << _energy_max_ / CLHEP::keV << " keV" << std::endl;
    }
    out_ << last_tag << "seed             : " << _seed_ << std::endl;
    return;
  }

} // end of namespace genbb

// end of wdecay0.cc
